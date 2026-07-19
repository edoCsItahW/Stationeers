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
 * @date 2026/07/09
 * @if zh
 * @brief IC10命令行编译器入口
 * @details ic10c - IC10脚本语言命令行编译器。支持词法分析、语法分析、
 *          语义分析各阶段的独立输出，以及完整的编译流程。
 * @par 基本用法:
 * @code
 * ic10c input.ic                    # 编译并输出符号表JSON
 * ic10c --emit-tokens input.ic      # 输出词法Token流
 * ic10c --emit-ast input.ic         # 输出语法树AST
 * ic10c -o out.json input.ic        # 输出到文件
 * @endcode
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 command-line compiler entry point
 * @details ic10c - IC10 scripting language command-line compiler. Supports independent
 *          output of lexical analysis, parsing, and semantic analysis phases, as well
 *          as the complete compilation pipeline.
 * @par Basic usage:
 * @code
 * ic10c input.ic                    # Compile and output symbol table JSON
 * ic10c --emit-tokens input.ic      # Output lexical token stream
 * ic10c --emit-ast input.ic         # Output syntax tree AST
 * ic10c -o out.json input.ic        # Output to file
 * @endcode
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */

#include "common/utils/file.hpp"
#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/en_us.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"
#include <cstdlib>
#include <expected>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>

namespace stationeers::ic10 {

    // -------------------------------------------------------------------------
    //  命令行选项
    // -------------------------------------------------------------------------

    namespace {
        struct Options {
            std::string inputFile;         ///< 输入源文件路径
            std::string outputFile;        ///< 输出文件路径（空表示stdout）
            std::string locale = "en-us";  ///< 本地化语言
            bool emitTokens    = false;    ///< 输出Token流
            bool emitAst       = false;    ///< 输出AST
            bool emitSymbols   = false;    ///< 输出符号表
            bool pretty        = false;    ///< 美化JSON输出
            bool showHelp      = false;    ///< 显示帮助
            bool showVersion   = false;    ///< 显示版本
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
                    return std::unexpected(ILoc::msgFormat<IMsgId::IIO1_1>(arg));
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
            if (arg == "--pretty") {
                opts.pretty = true;
                continue;
            }
            if (arg == "--locale") {
                if (i + 1 >= args.size())
                    return std::unexpected(ILoc::msgFormat<IMsgId::IIO1_1>(arg));
                opts.locale = args[++i];
                continue;
            }
            if (arg.starts_with("-")) return std::unexpected(ILoc::msgFormat<IMsgId::IIO2_1>(arg));
            if (opts.inputFile.empty())
                opts.inputFile = arg;
            else
                return std::unexpected(ILoc::msgFormat<IMsgId::IIO3_1>(arg));
        }

        if (!opts.showHelp && !opts.showVersion && opts.inputFile.empty())
            return std::unexpected(ILoc::msgStr<IMsgId::IIO4>());

        // 未指定任何emit选项时，默认执行完整编译（emit-symbols行为）
        if (!opts.emitTokens && !opts.emitAst && !opts.emitSymbols) opts.emitSymbols = true;

        return opts;
    }

    // -------------------------------------------------------------------------
    //  帮助信息
    // -------------------------------------------------------------------------

    static void printHelp(std::string_view programName) {
        std::cout << ILoc::msgFormat<IMsgId::IIO0_1>(programName);
    }

    static void printVersion() { std::cout << "ic10c version 2.0.0\n"; }

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
        for (const auto& diag : diagnostics) std::cerr << diag.message << '\n';

        return diagnostics.empty() ? 0 : 2;
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
        ILoc::registerLanguage<EnUs>("en-us");
        ILoc::registerLanguage<ZhHans>("zh-hans");
        ILoc::setLanguage("en-us");

        // 解析命令行参数
        std::span args(argv + 1, static_cast<std::size_t>(argc - 1));
        auto parseResult = parseArgs(args);
        if (!parseResult) {
            std::cerr << ILoc::msgFormat<IMsgId::IIO5_1>(parseResult.error());
            return 1;
        }

        const auto& opts = parseResult.value();

        // 根据用户指定的locale切换语言
        if (opts.locale == "zh-hans" || opts.locale == "zh_hans") {
            ILoc::setLanguage("zh-hans");

        } else if (opts.locale != "en-us" && opts.locale != "en_us") {
            std::cerr << ILoc::msgFormat<IMsgId::IIO6_1>(opts.locale);

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

        // 读取输入文件
        auto sourceResult = readFile(opts.inputFile);

        // 根据选项执行对应编译阶段
        if (opts.emitTokens) return runEmitTokens(sourceResult, opts);

        if (opts.emitAst) return runEmitAst(sourceResult, opts);

        return runEmitSymbols(sourceResult, opts);
    }

}  // namespace stationeers::ic10

// 全局入口点
int main(int argc, char* argv[]) { return stationeers::ic10::main(argc, argv); }
