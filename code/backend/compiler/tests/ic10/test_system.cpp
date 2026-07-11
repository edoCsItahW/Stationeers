// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

/**
 * @file test_system.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/11
 * @brief IC10系统测试
 * @details 端到端测试完整的IC10程序编译流程，使用真实的IC10程序验证
 *          编译器在复杂场景下的正确性，包括从文件读取fixture、完整流水线编译、
 *          符号表验证、诊断检测和各类指令覆盖。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"

using namespace stationeers::ic10;


class SystemTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        Loc::registerLanguage<ZhHans>("zh-hans");
        Loc::setLanguage("zh-hans");
    }

    /// 完整编译流水线
    struct CompileResult {
        std::vector<std::shared_ptr<Token>> tokens;
        Program ast;
        std::shared_ptr<Analyser> analyser;
        std::vector<Diagnostic> lexerDiags;
        std::vector<Diagnostic> parserDiags;
        std::vector<Diagnostic> analyserDiags;
    };

    static CompileResult compile(const std::string& source) {
        CompileResult result;

        Lexer lexer(source);
        result.tokens = lexer.scan();
        result.lexerDiags = lexer.getDiagnostics();

        Parser parser(result.tokens);
        result.ast = parser.parse();
        result.parserDiags = parser.getDiagnostics();

        result.analyser = std::make_shared<Analyser>();
        auto task = result.analyser->visit(result.ast);
        (void)task.getFuture().get();
        result.analyserDiags = result.analyser->getDiagnostics();

        return result;
    }

    /// 从文件读取IC10源码
    static std::string readFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file) return {};
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    /// 获取 grammarTest.ic 的绝对路径（基于 __FILE__，不依赖工作目录）
    static std::filesystem::path grammarTestPath() {
        return std::filesystem::path(__FILE__).parent_path() / ".." / "grammarTest.ic";
    }

    /// 统计所有阶段的诊断总数
    static std::size_t totalDiags(const CompileResult& result) {
        return result.lexerDiags.size() + result.parserDiags.size() + result.analyserDiags.size();
    }

    /// 格式化诊断列表为字符串（调试用）
    static std::string formatDiags(const std::string& stage, const std::vector<Diagnostic>& diags) {
        std::ostringstream ss;
        ss << stage << " (" << diags.size() << " diags):\n";
        for (const auto& d : diags) {
            ss << "  [" << d.id << "] L" << d.start.line() << ":" << d.start.column()
               << " - " << d.message << "\n";
        }
        return ss.str();
    }
};

// ============================================================
// grammarTest.ic Fixture 测试
// ============================================================

TEST_F(SystemTestFixture, GrammarTestFileCompilesSuccessfully) {
    // grammarTest.ic 是一个完整的IC10程序，包含alias、define、标签、各类指令
    auto source = readFile(grammarTestPath());
    ASSERT_FALSE(source.empty()) << "无法读取 grammarTest.ic";

    auto result = compile(source);

    // 应生成有效AST
    EXPECT_GE(result.ast.statements.size(), 20u);

    // 符号表应包含所有alias和define定义的符号
    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("filter"));
    EXPECT_TRUE(symtab.contains("analyzer"));
    EXPECT_TRUE(symtab.contains("led"));
    EXPECT_TRUE(symtab.contains("display"));
    EXPECT_TRUE(symtab.contains("analyzerMixture"));
    EXPECT_TRUE(symtab.contains("minMoles"));
    EXPECT_TRUE(symtab.contains("typeOxygen"));
    EXPECT_TRUE(symtab.contains("typeNitrogen"));

    // 标签应被定义
    EXPECT_TRUE(symtab.contains("loop"));
    EXPECT_TRUE(symtab.contains("updateDisplay"));
    EXPECT_TRUE(symtab.contains("checkFilters"));
    EXPECT_TRUE(symtab.contains("checkInput"));
}

TEST_F(SystemTestFixture, GrammarTestFileHasNoLexerErrors) {
    auto source = readFile(grammarTestPath());
    ASSERT_FALSE(source.empty());

    Lexer lexer(source);
    (void)lexer.scan();
    EXPECT_TRUE(lexer.getDiagnostics().empty());
}

TEST_F(SystemTestFixture, GrammarTestFileHasNoParserErrors) {
    auto source = readFile(grammarTestPath());
    ASSERT_FALSE(source.empty());

    auto tokens = Lexer::tokenize(source);
    Parser parser(tokens);
    (void)parser.parse();
    EXPECT_TRUE(parser.getDiagnostics().empty());
}

TEST_F(SystemTestFixture, GrammarTestFileSymbolTableValid) {
    auto source = readFile(grammarTestPath());
    ASSERT_FALSE(source.empty());

    auto result = compile(source);

    auto json = result.analyser->getSymbolTable().toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find('{'), std::string::npos);
}

// ============================================================
// 真实IC10程序测试
// ============================================================

TEST_F(SystemTestFixture, CountingLoopProgram) {
    // 计数器循环程序
    std::string source =
        "alias counter r0\n"
        "define LIMIT 10\n"
        "start:\n"
        "move r0 0\n"
        "loop:\n"
        "add r0 r0 1\n"
        "blt r0 LIMIT loop\n"
        "yield\n"
        "j start\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 7u);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("counter"));
    EXPECT_TRUE(symtab.contains("LIMIT"));
    EXPECT_TRUE(symtab.contains("start"));
    EXPECT_TRUE(symtab.contains("loop"));
}

TEST_F(SystemTestFixture, AirlockControlProgram) {
    // 气闸控制程序
    // NOTE: s指令的第三个操作数必须是registerOrIdentifier，不能用数字字面量
    std::string source =
        "alias door d0\n"
        "alias sensor d1\n"
        "alias vent d2\n"
        "\n"
        "define PRESSURE_MIN 10\n"
        "define PRESSURE_MAX 50\n"
        "define STATE_OPEN 1\n"
        "define STATE_CLOSED 0\n"
        "\n"
        "main:\n"
        "l r0 sensor Pressure\n"
        "blt r0 PRESSURE_MIN open_door\n"
        "bgt r0 PRESSURE_MAX close_door\n"
        "j main\n"
        "\n"
        "open_door:\n"
        "s door Open STATE_OPEN\n"
        "s vent On STATE_CLOSED\n"
        "j main\n"
        "\n"
        "close_door:\n"
        "s door Open STATE_CLOSED\n"
        "s vent On STATE_OPEN\n"
        "j main\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty()) << formatDiags("Lexer", result.lexerDiags);
    EXPECT_TRUE(result.parserDiags.empty()) << formatDiags("Parser", result.parserDiags);
    EXPECT_GE(result.ast.statements.size(), 10u);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("door"));
    EXPECT_TRUE(symtab.contains("sensor"));
    EXPECT_TRUE(symtab.contains("vent"));
    EXPECT_TRUE(symtab.contains("main"));
    EXPECT_TRUE(symtab.contains("open_door"));
    EXPECT_TRUE(symtab.contains("close_door"));
}

TEST_F(SystemTestFixture, GasFilterProgram) {
    // 气体过滤器程序（简化版）
    // NOTE: s指令的第三个操作数必须是registerOrIdentifier，不能用数字字面量
    std::string source =
        "alias filter d0\n"
        "alias analyzer d1\n"
        "define MIN_MOLES 10\n"
        "define STATE_OFF 0\n"
        "define STATE_ON 1\n"
        "\n"
        "loop:\n"
        "l r0 analyzer Pressure\n"
        "round r0 r0\n"
        "bge r0 MIN_MOLES activate\n"
        "s filter On STATE_OFF\n"
        "j loop\n"
        "activate:\n"
        "s filter On STATE_ON\n"
        "j loop\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty()) << formatDiags("Lexer", result.lexerDiags);
    EXPECT_TRUE(result.parserDiags.empty()) << formatDiags("Parser", result.parserDiags);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("filter"));
    EXPECT_TRUE(symtab.contains("analyzer"));
    EXPECT_TRUE(symtab.contains("MIN_MOLES"));
    EXPECT_TRUE(symtab.contains("loop"));
    EXPECT_TRUE(symtab.contains("activate"));
}

// ============================================================
// 所有指令元数覆盖测试
// ============================================================

TEST_F(SystemTestFixture, AllInstructionArities) {
    // 程序包含0-6元指令
    // NOTE: lbn/lbns的deviceHash/nameHash需为registerOrNumber(r?|num)，不能用device
    //       logicType/batchMode需为identifier|number，不能用register
    std::string source =
        "# 零元\n"
        "hcf\n"
        "yield\n"
        "\n"
        "# 一元\n"
        "sleep 100\n"
        "j main\n"
        "\n"
        "# 二元\n"
        "move r0 0\n"
        "abs r0 r1\n"
        "\n"
        "# 三元\n"
        "add r0 r1 r2\n"
        "sub r0 r1 r2\n"
        "mul r0 r1 r2\n"
        "\n"
        "# 四元\n"
        "lerp r0 r1 r2 r3\n"
        "\n"
        "# 五元: lbn registerRef registerOrNumber registerOrNumber logicType batchMode\n"
        "lbn r0 100 200 2 Average\n"
        "\n"
        "# 六元: lbns registerRef registerOrNumber registerOrNumber slotIndex logicSlotType batchMode\n"
        "lbns r0 100 200 3 Quantity Average\n"
        "\n"
        "main:\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty()) << formatDiags("Lexer", result.lexerDiags);
    EXPECT_TRUE(result.parserDiags.empty()) << formatDiags("Parser", result.parserDiags);
    EXPECT_GE(result.ast.statements.size(), 12u);
}

// ============================================================
// 预处理指令完整覆盖测试
// ============================================================

TEST_F(SystemTestFixture, AllPreprocessorDirectives) {
    // NOTE: define的值必须是operand(register/device/number/identifier/constant/macroCall)，
    //       不能是string字面量
    std::string source =
        "alias devA d0\n"
        "alias regA r0\n"
        "define COUNT 42\n"
        "define HEX_VAL $FF\n"
        "define HASH_VAL HASH(\"StructureLiquidVolumePump\")\n"
        "define STR_VAL STR(\"SomeHash\")\n"
        "main:\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty()) << formatDiags("Lexer", result.lexerDiags);
    EXPECT_TRUE(result.parserDiags.empty()) << formatDiags("Parser", result.parserDiags);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("devA"));
    EXPECT_TRUE(symtab.contains("regA"));
    EXPECT_TRUE(symtab.contains("COUNT"));
    EXPECT_TRUE(symtab.contains("HEX_VAL"));
    EXPECT_TRUE(symtab.contains("HASH_VAL"));
    EXPECT_TRUE(symtab.contains("STR_VAL"));
}

// ============================================================
// 常量引用测试
// ============================================================

TEST_F(SystemTestFixture, ConstantsUsedInInstructions) {
    std::string source =
        "move r0 pi\n"
        "move r1 tau\n"
        "move r2 nan\n"
        "move r3 pinf\n"
        "move r4 ninf\n"
        "move r5 rgas\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 7u);
}

// ============================================================
// 数字字面量完整覆盖测试
// ============================================================

TEST_F(SystemTestFixture, AllNumberTypes) {
    std::string source =
        "move r0 42\n"        // 整数
        "move r1 $FF\n"       // 十六进制
        "move r2 %1010\n"     // 二进制
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 4u);
}

// ============================================================
// Unicode标识符测试
// ============================================================

TEST_F(SystemTestFixture, UnicodeIdentifiers) {
    std::string source =
        "alias 中文变量 d0\n"
        "define 计数 10\n"
        "主循环:\n"
        "move r0 计数\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("中文变量"));
    EXPECT_TRUE(symtab.contains("计数"));
    EXPECT_TRUE(symtab.contains("主循环"));
}

// ============================================================
// 错误场景系统测试
// ============================================================

TEST_F(SystemTestFixture, UndefinedVariableProducesDiagnostic) {
    std::string source = "move r0 undefined_var\nhcf\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyserDiags.empty());
}

TEST_F(SystemTestFixture, RedefinedAliasProducesDiagnostic) {
    std::string source =
        "alias foo r0\n"
        "alias foo r1\n"
        "hcf\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyserDiags.empty());
}

TEST_F(SystemTestFixture, RedefinedDefineProducesDiagnostic) {
    std::string source =
        "define MAX 10\n"
        "define MAX 20\n"
        "hcf\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyserDiags.empty());
}

TEST_F(SystemTestFixture, RedefinedLabelProducesDiagnostic) {
    std::string source =
        "start:\n"
        "start:\n"
        "hcf\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyserDiags.empty());
}

TEST_F(SystemTestFixture, UndefinedLabelInJumpProducesDiagnostic) {
    std::string source =
        "j nonexistent\n"
        "hcf\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyserDiags.empty());
}

TEST_F(SystemTestFixture, MultipleErrorsAllReported) {
    std::string source =
        "alias foo r0\n"
        "alias foo r1\n"            // 重定义
        "move r0 undef1\n"          // 未定义
        "move r1 undef2\n"          // 未定义
        "hcf\n";
    auto result = compile(source);

    // 应有至少2个诊断（重定义 + 未定义）
    EXPECT_GE(result.analyserDiags.size(), 2u);
}

// ============================================================
// 边界场景测试
// ============================================================

TEST_F(SystemTestFixture, EmptyFileCompiles) {
    auto result = compile("");

    EXPECT_EQ(result.ast.statements.size(), 0u);
    EXPECT_TRUE(totalDiags(result) == 0);
}

TEST_F(SystemTestFixture, OnlyNewlinesCompiles) {
    auto result = compile("\n\n\n\n\n");

    EXPECT_EQ(result.ast.statements.size(), 0u);
}

TEST_F(SystemTestFixture, OnlyCommentsCompiles) {
    auto result = compile("# comment\n// comment\n# another\n");

    EXPECT_EQ(result.ast.statements.size(), 0u);
}

TEST_F(SystemTestFixture, SingleInstructionNoTrailingNewline) {
    auto result = compile("hcf");

    EXPECT_GE(result.ast.statements.size(), 1u);
    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
}

TEST_F(SystemTestFixture, TrailingNewlineDoesNotCreateExtraStatement) {
    auto result1 = compile("hcf\n");
    auto result2 = compile("hcf\n\n\n");

    EXPECT_EQ(result1.ast.statements.size(), result2.ast.statements.size());
}

// ============================================================
// 大型程序测试
// ============================================================

TEST_F(SystemTestFixture, LargeProgramWithManyStatements) {
    std::string source;
    source.reserve(4096);

    source += "alias counter r0\n";
    source += "define LIMIT 100\n";
    source += "start:\n";
    source += "move r0 0\n";

    // 生成100条指令
    for (int i = 0; i < 100; ++i) {
        source += "add r0 r0 1\n";
        source += "yield\n";
    }

    source += "blt r0 LIMIT start\n";
    source += "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 200u);
}

TEST_F(SystemTestFixture, ManyAliasesAndDefines) {
    std::string source;

    // 生成50个别名
    for (int i = 0; i < 50; ++i) {
        source += "alias dev" + std::to_string(i) + " d" + std::to_string(i % 6) + "\n";
    }

    // 生成50个定义
    for (int i = 0; i < 50; ++i) {
        source += "define VAL" + std::to_string(i) + " " + std::to_string(i) + "\n";
    }

    source += "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("dev0"));
    EXPECT_TRUE(symtab.contains("dev49"));
    EXPECT_TRUE(symtab.contains("VAL0"));
    EXPECT_TRUE(symtab.contains("VAL49"));
}

// ============================================================
// 注释混合测试
// ============================================================

TEST_F(SystemTestFixture, CommentsEverywhere) {
    std::string source =
        "# 文件头注释\n"
        "alias foo r0  # 行内注释\n"
        "# 独立注释\n"
        "define MAX 10  // 另一种注释\n"
        "// C风格注释\n"
        "main:  # 标签后注释\n"
        "move r0 0  # 初始化\n"
        "hcf  # 结束\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 4u);
}

// ============================================================
// 前向引用测试
// ============================================================

TEST_F(SystemTestFixture, ForwardReferenceToLabel) {
    std::string source =
        "j end\n"
        "move r0 1\n"
        "end:\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.analyser->getSymbolTable().contains("end"));
}

TEST_F(SystemTestFixture, MultipleForwardReferences) {
    std::string source =
        "j label2\n"
        "j label3\n"
        "j label1\n"
        "label1:\n"
        "hcf\n"
        "label2:\n"
        "hcf\n"
        "label3:\n"
        "hcf\n";

    auto result = compile(source);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("label1"));
    EXPECT_TRUE(symtab.contains("label2"));
    EXPECT_TRUE(symtab.contains("label3"));
}

// ============================================================
// HASH和STR宏调用测试
// ============================================================

TEST_F(SystemTestFixture, HashMacroInDefine) {
    std::string source =
        "define PUMP HASH(\"StructureLiquidVolumePump\")\n"
        "move r0 PUMP\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_TRUE(result.analyser->getSymbolTable().contains("PUMP"));
}

TEST_F(SystemTestFixture, StrMacroInDefine) {
    std::string source =
        "define LABEL STR(\"SomeHash\")\n"
        "move r0 LABEL\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_TRUE(result.analyser->getSymbolTable().contains("LABEL"));
}

// ============================================================
// 设备引用测试
// ============================================================

TEST_F(SystemTestFixture, AllDeviceReferences) {
    std::string source =
        "alias d0_alias d0\n"
        "alias d1_alias d1\n"
        "alias d2_alias d2\n"
        "alias d3_alias d3\n"
        "alias d4_alias d4\n"
        "alias d5_alias d5\n"
        "l r0 d0 Pressure\n"
        "l r1 d1 Temperature\n"
        "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("d0_alias"));
    EXPECT_TRUE(symtab.contains("d5_alias"));
}

// ============================================================
// 寄存器边界测试
// ============================================================

TEST_F(SystemTestFixture, AllRegistersUsed) {
    std::string source;
    for (int i = 0; i < 16; ++i) {
        source += "move r" + std::to_string(i) + " 0\n";
    }
    source += "hcf\n";

    auto result = compile(source);

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_GE(result.ast.statements.size(), 17u);
}

TEST_F(SystemTestFixture, RegisterOutOfRangeBecomesIdentifier) {
    // r16 超出寄存器范围，应被识别为标识符而非寄存器
    std::string source = "move r16 0\nhcf\n";
    auto result = compile(source);

    // r16作为标识符在语义分析中应产生未定义错误
    EXPECT_FALSE(result.analyserDiags.empty());
}
