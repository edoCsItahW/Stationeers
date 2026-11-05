// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file bench_compiler.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/10/05
 * @if zh
 * @brief IC10 编译器核心性能基准
 * @details 对编译器核心的完整流水线逐阶段计时：
 *          `Lexer::tokenize` → `Parser::parsing` → `Analyser::visit` → `Linker::addUnit` / `Linker::link`。
 *          输出各阶段的最小值、中位数、均值、标准差与吞吐，用于建立可回归对比的性能基线。
 *
 * @note `link` 阶段被拆成两行：`Linker::addUnit` 按值接收 @ref Program，而 @ref Program 的基类
 *       `AST<T>` 声明了虚析构函数，隐式移动构造因此被抑制 —— 该行度量的是 AST 深拷贝的代价，
 *       与 `Linker::link` 的真实链接代价分开呈现。
 *
 * @par 用法:
 * @code
 * IC10_Compiler_Benchmark <source.ic> [--iterations N] [--seconds S]
 * @endcode
 * - `<source.ic>`：IC10 源文件路径（必填）
 * - `--iterations N`：计时轮数，默认 5
 * - `--seconds S`：按墙上时钟预算反复执行取代轮数上限，用于给 Intel VTune Profiler 等采样器
 *   制造足够长的稳态区间（例如 `--seconds 30`）
 *
 * @warning 每轮都会完整重建 AST 并在轮末释放，因此峰值内存约为单轮 AST 的两倍（轮次之间不累加）。
 *
 * @elseif en
 * @brief IC10 compiler core performance benchmark
 * @details Times each stage of the compiler core pipeline:
 *          `Lexer::tokenize` → `Parser::parsing` → `Analyser::visit` → `Linker::addUnit` / `Linker::link`.
 *          Reports min / median / mean / standard deviation and throughput per stage, establishing a
 *          regression-comparable performance baseline.
 *
 * @note The `link` stage is split across two rows: `Linker::addUnit` takes @ref Program by value, and
 *       @ref Program's base class `AST<T>` declares a virtual destructor, which suppresses the implicit
 *       move constructor — so that row measures the cost of a deep AST copy, kept separate from the
 *       actual linking cost in `Linker::link`.
 *
 * @par Usage:
 * @code
 * IC10_Compiler_Benchmark <source.ic> [--iterations N] [--seconds S]
 * @endcode
 * - `<source.ic>`: path to an IC10 source file (required)
 * - `--iterations N`: number of timed passes, defaults to 5
 * - `--seconds S`: repeat passes until the wall-clock budget elapses instead of a pass cap, useful for
 *   giving samplers such as Intel VTune Profiler a long steady-state region (e.g. `--seconds 30`)
 *
 * @warning Every pass rebuilds the complete AST and frees it at the end of the pass, so peak memory is
 *          about two passes' worth of AST (it does not accumulate across passes).
 * @endif
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */

#include "common/locals/common_local.hpp"
#include "common/locals/languages/en_us.hpp"
#include "common/locals/languages/zh_hans.hpp"
#include "common/utils/file.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/link/linker.hpp"
#include "ic10_compiler/locals/languages/en_us.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace stationeers::ic10 {

    namespace {

        /// 计时时钟（单调，不受系统时间调整影响）
        using Clock = std::chrono::steady_clock;

        /**
         * @if zh
         * @brief 计算两个时间点之间的毫秒数
         * @param from 起点
         * @param to 终点
         * @return 以毫秒为单位的间隔
         * @elseif en
         * @brief Milliseconds between two time points
         * @param from Start point
         * @param to End point
         * @return The interval in milliseconds
         * @endif
         */
        double elapsedMs(Clock::time_point from, Clock::time_point to) {
            return std::chrono::duration<double, std::milli>(to - from).count();
        }

        /// 单轮编译的观测结果（各阶段耗时单位为毫秒）
        struct PassResult {
            double lex     = 0.0;  ///< 词法分析耗时
            double parse   = 0.0;  ///< 语法分析耗时
            double analyse = 0.0;  ///< 语义分析耗时
            double linkAdd = 0.0;  ///< `Linker::addUnit`（AST 深拷贝）耗时
            double linkRun = 0.0;  ///< `Linker::link`（真实链接）耗时

            std::size_t tokens      = 0;  ///< 产出的 Token 数
            std::size_t statements  = 0;  ///< 产出的顶层语句数
            std::size_t diagnostics = 0;  ///< 语义阶段与链接阶段的诊断总数
        };

        /**
         * @if zh
         * @brief 执行一轮完整编译流水线并按阶段计时
         * @details 与 `ic10c` 的生产路径保持一致：词法与语法走静态便捷入口
         *          （`Lexer::tokenize` / `Parser::parsing`），语义阶段构造 `Analyser` 实例以便读取诊断，
         *          链接阶段构造 `Linker` 并依次调用 `addUnit` 与 `link`。
         * @param source 源代码视图
         * @param path 源文件路径（用于链接阶段的诊断定位）
         * @return 本轮各阶段的耗时与结果规模
         * @elseif en
         * @brief Run one complete compilation pass, timing every stage
         * @details Mirrors the production path taken by `ic10c`: lexing and parsing use the static
         *          convenience entries (`Lexer::tokenize` / `Parser::parsing`), the semantic stage
         *          constructs an `Analyser` instance so diagnostics can be read, and the linking stage
         *          constructs a `Linker` and calls `addUnit` then `link` in turn.
         * @param source The source code view
         * @param path Source file path (used for diagnostic locations in the linking stage)
         * @return Per-stage timings and the sizes of the produced results
         * @endif
         */
        PassResult runPass(std::string_view source, const std::string& path) {
            PassResult result;

            const auto tLex0 = Clock::now();
            auto tokens      = Lexer::tokenize(source);
            const auto tLex1 = Clock::now();

            auto program      = Parser::parsing(tokens);
            const auto tParse = Clock::now();

            Analyser analyser;
            auto task = analyser.visit(program);
            (void)task.getFuture().get();
            const auto tAnalyse = Clock::now();

            Linker linker;
            linker.addUnit(program, path);
            const auto tLinkAdd = Clock::now();

            linker.link();
            const auto tLinkRun = Clock::now();

            result.lex     = elapsedMs(tLex0, tLex1);
            result.parse   = elapsedMs(tLex1, tParse);
            result.analyse = elapsedMs(tParse, tAnalyse);
            result.linkAdd = elapsedMs(tAnalyse, tLinkAdd);
            result.linkRun = elapsedMs(tLinkAdd, tLinkRun);

            result.tokens      = tokens.size();
            result.statements  = program.statements.size();
            result.diagnostics = analyser.getDiagnostics().size() + linker.getDiagnostics().size();

            return result;
        }

        /// 一组观测值的描述统计量（单位毫秒）
        struct Stats {
            double min    = 0.0;
            double median = 0.0;
            double mean   = 0.0;
            double sd     = 0.0;
        };

        /**
         * @if zh
         * @brief 计算一组耗时样本的描述统计量
         * @param samples 耗时样本（会被就地排序）
         * @return 最小值、中位数、均值与样本标准差
         * @note 偶数个样本时中位数取中间两个值的平均；样本数小于 2 时标准差记为 0。
         * @elseif en
         * @brief Compute descriptive statistics for a set of timing samples
         * @param samples The timing samples (sorted in place)
         * @return Minimum, median, mean and sample standard deviation
         * @note With an even number of samples the median averages the two middle values; with fewer
         *       than two samples the standard deviation is reported as 0.
         * @endif
         */
        Stats summarize(std::vector<double> samples) {
            Stats stats;
            if (samples.empty()) return stats;

            std::sort(samples.begin(), samples.end());

            const std::size_t n = samples.size();
            stats.min           = samples.front();
            stats.median        = (n % 2 == 1)
                                    ? samples[n / 2]
                                    : 0.5 * (samples[n / 2 - 1] + samples[n / 2]);

            double sum = 0.0;
            for (const double sample : samples) sum += sample;
            stats.mean = sum / static_cast<double>(n);

            if (n > 1) {
                double sq = 0.0;
                for (const double sample : samples) sq += (sample - stats.mean) * (sample - stats.mean);
                stats.sd = std::sqrt(sq / static_cast<double>(n - 1));
            }

            return stats;
        }

        /// 以固定小数位格式化数值
        std::string fmt(double value, int precision = 2) {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(precision) << value;

            return stream.str();
        }

        void printUsage(std::string_view programName) {
            std::cout << "usage: " << programName << " <source.ic> [--iterations N] [--seconds S]\n"
                      << '\n'
                      << "  <source.ic>        IC10 source file to compile (required)\n"
                      << "  --iterations N     number of timed passes (default 5)\n"
                      << "  --seconds S        repeat passes until S wall-clock seconds elapse,\n"
                      << "                     overriding --iterations (for profiler attachment)\n"
                      << "  -h, --help         show this message\n";
        }

        /**
         * @if zh
         * @brief 打印单阶段统计行
         * @param name 阶段名称
         * @param samples 该阶段的耗时样本
         * @param megabytes 输入源文件的兆字节数（用于折算吞吐）
         * @elseif en
         * @brief Print one stage's statistics row
         * @param name Stage name
         * @param samples That stage's timing samples
         * @param megabytes Size of the input source in megabytes (for throughput)
         * @endif
         */
        void printStage(std::string_view name, std::vector<double> samples, double megabytes) {
            const Stats stats = summarize(std::move(samples));
            const double throughput =
                stats.mean > 0.0 ? megabytes * 1000.0 / stats.mean : 0.0;

            std::cout << std::left << std::setw(12) << name << std::right << std::setw(9)
                      << fmt(stats.min) << std::setw(11) << fmt(stats.median) << std::setw(11)
                      << fmt(stats.mean) << std::setw(10) << fmt(stats.sd) << std::setw(9)
                      << fmt(throughput, 1) << '\n';
        }

        /// 基准程序主逻辑（本地化注册、命令行解析、测量循环、结果输出）
        int run(int argc, char* argv[]) {
            // 与 ic10c 入口保持一致的本地化注册顺序：公共消息包与编译器消息包都要注册，
            // 否则早期诊断在取消息时会拿不到语言表。
            CLoc::registerLanguage<stationeers::EnUs>("en-us");
            CLoc::registerLanguage<stationeers::ZhHans>("zh-hans");

            ICLoc::registerLanguage<EnUs>("en-us");
            ICLoc::registerLanguage<ZhHans>("zh-hans");

            CLoc::setLanguage("en-us");
            ICLoc::setLanguage("en-us");

            std::string sourcePath;
            std::size_t iterations = 5;
            double seconds         = 0.0;

            for (int i = 1; i < argc; ++i) {
                const std::string_view arg = argv[i];

                if (arg == "-h" || arg == "--help") {
                    printUsage(argv[0]);

                    return 0;
                }

                if (arg == "--iterations" || arg == "--seconds") {
                    if (i + 1 >= argc) {
                        std::cerr << arg << " requires a value\n";

                        return 2;
                    }

                    const std::string value = argv[++i];

                    if (arg == "--iterations")
                        iterations = static_cast<std::size_t>(std::stoull(value));

                    else
                        seconds = std::stod(value);

                    continue;
                }

                if (arg.starts_with("-")) {
                    std::cerr << "unknown option: " << arg << '\n';

                    return 2;
                }

                if (!sourcePath.empty()) {
                    std::cerr << "only one source file is supported\n";

                    return 2;
                }

                sourcePath = arg;
            }

            if (sourcePath.empty()) {
                printUsage(argv[0]);

                return 2;
            }

            if (iterations == 0) {
                std::cerr << "--iterations must be at least 1\n";

                return 2;
            }

            auto fileResult = stationeers::readFile(sourcePath);
            if (!fileResult) {
                std::cerr << "failed to read " << sourcePath << ": " << fileResult.error() << '\n';

                return 1;
            }

            const std::string& source = fileResult.value();
            const double megabytes     = static_cast<double>(source.size()) / (1024.0 * 1024.0);

            // 预热一轮：排除首次运行的一次性代价（惰性初始化、冷堆、页错误），不计入统计。
            (void)runPass(source, sourcePath);

            std::vector<double> lex, parse, analyse, linkAdd, linkRun;
            PassResult last{};

            std::size_t passes = 0;
            const bool budgeted = seconds > 0.0;

            const auto wallStart = Clock::now();

            for (std::size_t i = 0;; ++i) {
                if (budgeted) {
                    // 至少执行一轮，之后按预算收敛
                    if (i > 0 && elapsedMs(wallStart, Clock::now()) >= seconds * 1000.0) break;

                } else if (i >= iterations)
                    break;

                last = runPass(source, sourcePath);

                lex.push_back(last.lex);
                parse.push_back(last.parse);
                analyse.push_back(last.analyse);
                linkAdd.push_back(last.linkAdd);
                linkRun.push_back(last.linkRun);
                ++passes;
            }

            const double wallMs = elapsedMs(wallStart, Clock::now());

            std::cout << "=== IC10 compiler core benchmark ===\n"
                      << "file          : " << sourcePath << '\n'
                      << "source bytes  : " << source.size() << '\n'
                      << "timed passes  : " << passes << '\n'
                      << "wall clock ms : " << fmt(wallMs) << '\n'
                      << "tokens        : " << last.tokens << '\n'
                      << "statements    : " << last.statements << '\n'
                      << "diagnostics   : " << last.diagnostics << '\n'
                      << '\n';

            std::cout << std::left << std::setw(12) << "stage" << std::right << std::setw(9)
                      << "min(ms)" << std::setw(11) << "median(ms)" << std::setw(11) << "mean(ms)"
                      << std::setw(10) << "sd(ms)" << std::setw(9) << "MB/s" << '\n';

            printStage("lex", std::move(lex), megabytes);
            printStage("parse", std::move(parse), megabytes);
            printStage("analyse", std::move(analyse), megabytes);
            printStage("link.add", std::move(linkAdd), megabytes);
            printStage("link.link", std::move(linkRun), megabytes);

            return 0;
        }

    }  // namespace

}  // namespace stationeers::ic10

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    return stationeers::ic10::run(argc, argv);
}
