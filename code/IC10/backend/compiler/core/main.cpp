// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file main.cpp
 * @author edocsitahw
 * @version 2.0
 * @date 2026/07/19
 * @if zh
 * @brief IC10命令行编译器入口
 * @details ic10c - IC10脚本语言命令行编译器。支持词法分析、语法分析、
 *          语义分析各阶段的独立输出，以及完整的编译流程。
 *          链接模式（--link）支持多个编译单元的符号合并与前向引用解析。
 * @par 基本用法:
 * @code
 * ic10c input.ic                    # 编译并输出符号表JSON
 * ic10c --emit-tokens input.ic      # 输出词法Token流
 * ic10c --emit-ast input.ic         # 输出语法树AST
 * ic10c -o out.json input.ic        # 输出到文件
 * ic10c --link a.ic b.ic c.ic       # 链接多个单元并输出合并后的符号表
 * @endcode
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 command-line compiler entry point
 * @details ic10c - IC10 scripting language command-line compiler. Supports independent
 *          output of lexical analysis, parsing, and semantic analysis phases, as well
 *          as the complete compilation pipeline.
 *          Link mode (--link) supports symbol merging and forward reference resolution
 *          across multiple compilation units.
 * @par Basic usage:
 * @code
 * ic10c input.ic                    # Compile and output symbol table JSON
 * ic10c --emit-tokens input.ic      # Output lexical token stream
 * ic10c --emit-ast input.ic         # Output syntax tree AST
 * ic10c -o out.json input.ic        # Output to file
 * ic10c --link a.ic b.ic c.ic       # Link multiple units and output merged symbol table
 * @endcode
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */

#include "common/locals/languages/en_us.hpp"
#include "common/locals/languages/zh_hans.hpp"
#include "common/utils/file.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/link/linker.hpp"
#include "ic10_compiler/locals/languages/en_us.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include <cstdlib>
#include <expected>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace stationeers::ic10 {

    // -------------------------------------------------------------------------
    //  命令行选项
    // -------------------------------------------------------------------------

    namespace {
        struct Options {
            std::vector<std::string> inputFiles;  ///< 输入源文件路径列表（link 模式下可多个）
            std::string outputFile;               ///< 输出文件路径（空表示stdout）
            std::string locale = "en-us";         ///< 本地化语言
            bool emitTokens    = false;           ///< 输出Token流
            bool emitAst       = false;           ///< 输出AST
            bool emitSymbols   = false;           ///< 输出符号表
            bool link          = false;           ///< 链接模式：合并多个编译单元的符号表
            bool pretty        = false;           ///< 美化JSON输出
            bool showHelp      = false;           ///< 显示帮助
            bool showVersion   = false;           ///< 显示版本
        };
    }  // namespace

    // -------------------------------------------------------------------------
    //  文件IO
    // -------------------------------------------------------------------------

    /**
     * @brief 读取文件内容为字符串
     * @param filename 文件路径
     * @return 文件内容或错误信息
     */
    static std::string readFile(const std::string& filename) {
        if (auto result = stationeers::readFile(filename); result)
            return result.value();

        else {
            std::cerr << result.error();

            std::exit(1);
        }
    }

    /**
     * @brief 将内容写入文件或stdout
     * @param content 输出内容
     * @param filename 文件路径（空则输出到stdout）
     */
    static void writeOutput(const std::string& content, const std::string& filename) {
        if (filename.empty()) {
            std::cout << content << '\n';

            return;
        }

        if (auto result = writeFile(content, filename); !result) {
            std::cerr << result.error();

            std::exit(1);
        }
    }

    // -------------------------------------------------------------------------
    //  JSON美化（简单实现）
    // -------------------------------------------------------------------------

    /**
     * @brief 对紧凑JSON字符串进行简单的缩进格式化
     * @param json 紧凑JSON字符串
     * @return 格式化后的JSON字符串
     */
    static std::string prettyJSON(std::string_view json) {
        std::string result;
        result.reserve(json.size() * 2);
        int indent    = 0;
        bool inString = false;

        for (std::size_t i = 0; i < json.size(); ++i) {
            const char ch = json[i];

            if (ch == '"') {
                inString = !inString;
                result += ch;
                continue;
            }

            if (inString) {
                result += ch;
                continue;
            }

            if (ch == '{' || ch == '[') {
                result += ch;
                result += '\n';
                indent += 4;
                result.append(indent, ' ');

            } else if (ch == '}' || ch == ']') {
                result += '\n';
                indent -= 4;
                result.append(indent, ' ');
                result += ch;

            } else if (ch == ',') {
                result += ch;
                result += '\n';
                result.append(indent, ' ');

            } else if (ch == ':') {
                result += ch;
                result += ' ';

            } else if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') {
                // 跳过空白字符

            } else
                result += ch;
        }
        return result;
    }

    // -------------------------------------------------------------------------
    //  参数解析
    // -------------------------------------------------------------------------

    /**
     * @brief 解析命令行参数
     * @param args 参数列表（不包含程序名）
     * @return 解析后的选项或错误信息
     */
    static auto parseArgs(std::span<char*> args) -> std::expected<Options, std::string> {
        Options opts;

        for (std::size_t i = 0; i < args.size(); ++i) {
            std::string_view arg = args[i];

            if (arg == "-h" || arg == "--help") {
                opts.showHelp = true;
                return opts;
            }
            if (arg == "-v" || arg == "--version") {
                opts.showVersion = true;
                return opts;
            }
            if (arg == "-o" || arg == "--output") {
                if (i + 1 >= args.size())
                    return std::unexpected(ICLoc::msgFormat<ICMsgId::IIO1_1>(arg));
                opts.outputFile = args[++i];
                continue;
            }
            if (arg == "--emit-tokens") {
                opts.emitTokens = true;
                continue;
            }
            if (arg == "--emit-ast") {
                opts.emitAst = true;
                continue;
            }
            if (arg == "--emit-symbols") {
                opts.emitSymbols = true;
                continue;
            }
            if (arg == "--link") {
                opts.link = true;
                continue;
            }
            if (arg == "--pretty") {
                opts.pretty = true;
                continue;
            }
            if (arg == "--locale") {
                if (i + 1 >= args.size())
                    return std::unexpected(ICLoc::msgFormat<ICMsgId::IIO1_1>(arg));
                opts.locale = args[++i];
                continue;
            }
            if (arg.starts_with("-")) return std::unexpected(ICLoc::msgFormat<ICMsgId::IIO2_1>(arg));
            // link 模式下允许多个输入文件；非 link 模式仅允许一个
            // Multiple input files are allowed in link mode; only one is allowed otherwise.
            if (!opts.link && !opts.inputFiles.empty())
                return std::unexpected(ICLoc::msgFormat<ICMsgId::IIO3_1>(arg));
            opts.inputFiles.emplace_back(arg);
        }

        if (!opts.showHelp && !opts.showVersion && opts.inputFiles.empty())
            return std::unexpected(ICLoc::msgStr<ICMsgId::IIO4>());

        // --link 模式强制执行符号表输出，忽略 emit-tokens/emit-ast
        // --link mode forces symbol-table output; emit-tokens/emit-ast are ignored.
        if (opts.link) {
            opts.emitTokens  = false;
            opts.emitAst     = false;
            opts.emitSymbols = true;
        }
        // 未指定任何emit选项时，默认执行完整编译（emit-symbols行为）
        else if (!opts.emitTokens && !opts.emitAst && !opts.emitSymbols)
            opts.emitSymbols = true;

        return opts;
    }

    // -------------------------------------------------------------------------
    //  帮助信息
    // -------------------------------------------------------------------------

    static void printHelp(std::string_view programName) {
        std::cout << ICLoc::msgFormat<ICMsgId::IIO0_1>(programName);
    }

    static void printVersion() { std::cout << "ic10c version 2.0.0\n"; }

    // -------------------------------------------------------------------------
    //  诊断输出（MSVC 风格）
    // -------------------------------------------------------------------------

    /**
     * @if zh
     * @brief 获取源代码中指定行的内容（行号从 1 开始）
     * @param source 源代码字符串
     * @param line 行号（从 1 开始）
     * @return 该行内容（不含行尾换行符）；行号越界返回空串
     * @elseif en
     * @brief Get the content of a specific line in source code (1-based)
     * @param source Source code string
     * @param line Line number (1-based)
     * @return Content of that line (without trailing newline); empty string if out of range
     * @endif
     */
    static std::string_view getLine(std::string_view source, int line) {
        if (line < 1) return {};
        std::string_view remaining = source;
        int current                = 1;
        while (current < line) {
            auto pos = remaining.find('\n');
            if (pos == std::string_view::npos) return {};
            remaining.remove_prefix(pos + 1);
            ++current;
        }
        auto end = remaining.find('\n');
        return remaining.substr(0, end);
    }

    /**
     * @if zh
     * @brief 以 MSVC 风格输出单条诊断
     * @details 格式:
     * @code
     * 路径(行,列): level id: 消息
     *     源代码行内容
     *     ^~~~~~~（caret 指示范围）
     * @endcode
     * 若路径为空或源代码不可用，则只输出首行（路径/源代码行/caret 省略）。
     * @param diag 诊断信息
     * @param filePath 源文件路径（可为空）
     * @param source 源代码内容（可为空）
     * @elseif en
     * @brief Print a single diagnostic in MSVC style
     * @details Format:
     * @code
     * path(line,col): level id: message
     *     source line content
     *     ^~~~~~~ (caret indicating the range)
     * @endcode
     * If path is empty or source is unavailable, only the first line is printed
     * (path/source-line/caret omitted).
     * @param diag Diagnostic info
     * @param filePath Source file path (may be empty)
     * @param source Source code content (may be empty)
     * @endif
     */
    static void printDiagnostic(
        const Diagnostic& diag, std::string_view filePath, std::string_view source
    ) {
        const int line = diag.start.line(), column = diag.start.column();

        if (!filePath.empty()) std::cerr << filePath << '(' << line << ',' << column << "): ";

        std::cerr << enumToStr(diag.level) << ' ' << diag.id << ": " << diag.message << '\n';

        if (source.empty()) return;

        auto lineContent = getLine(source, line);
        if (lineContent.empty()) return;

        std::cerr << lineContent << '\n';

        const int startCol = column;
        const int endCol   = diag.end.line() == line ? diag.end.column() : startCol + 1;

        std::string caret;
        caret.reserve(static_cast<std::size_t>(std::max(endCol, startCol + 1)));
        caret.append(static_cast<std::size_t>(std::max(startCol - 1, 0)), ' ');
        caret.push_back('^');

        if (const int tildeCount = std::max(endCol - startCol - 1, 0); tildeCount > 0)
            caret.append(static_cast<std::size_t>(tildeCount), '~');

        std::cerr << caret << '\n';
    }

    /**
     * @if zh
     * @brief 以 MSVC 风格输出诊断列表（针对单文件）
     * @param diagnostics 诊断列表
     * @param filePath 源文件路径
     * @param source 源代码内容
     * @elseif en
     * @brief Print a list of diagnostics in MSVC style (for a single file)
     * @param diagnostics Diagnostic list
     * @param filePath Source file path
     * @param source Source code content
     * @endif
     */
    static void printDiagnostics(
        const std::vector<Diagnostic>& diagnostics, std::string_view filePath,
        std::string_view source
    ) {
        for (const auto& diag : diagnostics) printDiagnostic(diag, filePath, source);
    }

    // -------------------------------------------------------------------------
    //  编译阶段执行
    // -------------------------------------------------------------------------

    /**
     * @brief 执行词法分析并输出Token JSON数组
     */
    static int runEmitTokens(const std::string& source, const Options& opts) {
        const auto tokens = Lexer::tokenize(source);

        std::stringstream ss;
        ss << "[\n";
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            ss << "  " << tokens[i]->toJSON();
            if (i + 1 < tokens.size()) ss << ",";
            ss << "\n";
        }
        ss << "]";

        std::string output = ss.str();
        if (opts.pretty) output = prettyJSON(output);

        writeOutput(output, opts.outputFile);

        return 0;
    }

    /**
     * @brief 执行语法分析并输出AST JSON
     */
    static int runEmitAst(const std::string& source, const Options& opts) {
        const auto tokens = Lexer::tokenize(source);
        const auto ast    = Parser::parsing(tokens);

        std::string output = ast.toJSON();
        if (opts.pretty) output = prettyJSON(output);

        writeOutput(output, opts.outputFile);

        return 0;
    }

    /**
     * @brief 执行完整编译（含语义分析）并输出符号表JSON和诊断信息
     */
    static int runEmitSymbols(const std::string& source, const Options& opts) {
        const auto tokens = Lexer::tokenize(source);
        const auto ast    = Parser::parsing(tokens);

        Analyser analyser;
        auto task = analyser.visit(ast);
        (void)task.getFuture().get();

        std::string output = analyser.getSymbolTable().toJSON();
        if (opts.pretty) output = prettyJSON(output);
        writeOutput(output, opts.outputFile);

        const auto& diagnostics = analyser.getDiagnostics();
        printDiagnostics(diagnostics, opts.inputFiles.front(), source);

        return diagnostics.empty() ? 0 : 2;
    }

    /**
     * @if zh
     * @brief 执行链接模式：合并多个编译单元的符号表
     * @details 将每个输入文件作为独立编译单元加入 Linker，调用 link() 合并符号表，
     *          支持跨单元前向引用解析。输出合并后的符号表 JSON 和按文件分组的
     *          MSVC 风格诊断信息。
     * @param opts 已解析的命令行选项，opts.inputFiles 至少含一个文件
     * @return 退出码：0 成功，1 文件错误，2 存在诊断信息
     * @elseif en
     * @brief Run link mode: merge symbol tables of multiple compilation units
     * @details Adds each input file as an independent compilation unit to the Linker,
     *          calls link() to merge symbol tables with cross-unit forward reference
     *          resolution. Outputs the merged symbol table JSON and per-file MSVC-style
     *          diagnostics.
     * @param opts Parsed command-line options; opts.inputFiles must contain at least one file
     * @return Exit code: 0 success, 1 file error, 2 diagnostics present
     * @endif
     */
    static int runLink(const Options& opts) {
        Linker linker;

        auto pairs = opts.inputFiles | std::views::transform([&](const std::string& path) -> std::pair<std::string, std::string> {
            auto source = readFile(path);

            linker.addUnit(source, path);

            return {path, std::move(source)};
        }) | std::ranges::to<std::vector<std::pair<std::string, std::string>>>();

        std::unordered_map files(pairs.begin(), pairs.end());

        auto& symtab = linker.link();

        std::string output = symtab.toJSON();
        if (opts.pretty) output = prettyJSON(output);
        writeOutput(output, opts.outputFile);

        bool hasDiag = std::ranges::any_of(linker.getUnits(), [&](const auto& unit) {
            bool flag = unit.diagnostics && !unit.diagnostics->empty();
            if (flag)
                printDiagnostics(*unit.diagnostics, unit.path, files[unit.path]);
            return flag;
        });

        return hasDiag ? 2 : 0;
    }

    // -------------------------------------------------------------------------
    //  主函数
    // -------------------------------------------------------------------------

    static int main(int argc, char* argv[]) {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
        std::cout.tie(nullptr);

#ifdef _MSC_VER
        system("chcp 65001>nul");
#endif

        // 预先注册并设置默认语言，确保早期调用（如parseArgs中的错误消息）可用

        CLoc::registerLanguage<stationeers::EnUs>("en-us");
        CLoc::registerLanguage<stationeers::ZhHans>("zh-hans");

        ICLoc::registerLanguage<EnUs>("en-us");
        ICLoc::registerLanguage<ZhHans>("zh-hans");

        CLoc::setLanguage("en-us");
        ICLoc::setLanguage("en-us");

        // 解析命令行参数
        std::span args(argv + 1, static_cast<std::size_t>(argc - 1));
        auto parseResult = parseArgs(args);
        if (!parseResult) {
            std::cerr << ICLoc::msgFormat<ICMsgId::IIO5_1>(parseResult.error());
            return 1;
        }

        const auto& opts = parseResult.value();

        // 根据用户指定的locale切换语言
        if (opts.locale == "zh-hans" || opts.locale == "zh_hans") {
            CLoc::setLanguage("zh-hans");
            ICLoc::setLanguage("zh-hans");

        } else if (opts.locale != "en-us" && opts.locale != "en_us") {
            std::cerr << ICLoc::msgFormat<ICMsgId::IIO6_1>(opts.locale);

            return 1;
        }

        if (opts.showHelp) {
            printHelp(argv[0]);
            return 0;
        }

        if (opts.showVersion) {
            printVersion();
            return 0;
        }

        // 链接模式：合并多个编译单元，无需读取单一 source
        // Link mode: merge multiple compilation units, no single source needed.
        if (opts.link) return runLink(opts);

        // 非 link 模式：inputFiles 此时必含且仅含一个文件
        // Non-link mode: inputFiles contains exactly one file at this point.
        auto sourceResult = readFile(opts.inputFiles.front());

        // 根据选项执行对应编译阶段
        if (opts.emitTokens) return runEmitTokens(sourceResult, opts);

        if (opts.emitAst) return runEmitAst(sourceResult, opts);

        return runEmitSymbols(sourceResult, opts);
    }

}  // namespace stationeers::ic10

// 全局入口点
int main(int argc, char* argv[]) { return stationeers::ic10::main(argc, argv); }
