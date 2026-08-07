// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/01
 * @if zh
 * @brief IC10 Java 绑定模块入口
 * @details 定义 JNI_OnLoad 入口函数，注册所有 HybridClass 的 native 方法，
 *          并初始化本地化语言支持。注册顺序遵循依赖关系：
 *          fbjni HybridData -> common(JPos/JToken/JDiagnostic) -> JProgram ->
 *          JSymbolTable -> JTypeTable -> JLexer -> JParser -> JAnalyser ->
 *          JIC10Local -> JTypeOfNode -> JLinker -> JIncLexer -> JIncParser ->
 *          JIncCompiler
 *          枚举类型(TokenType/TokenCategory/OperandType/BasicType/TypeCategory)
 *          直接在 Java 层定义,无需 JNI 注册。
 *          注意：fbjni 以静态库形式链接,需在此手动调用 HybridDataOnLoad()
 *          注册 HybridData.Destructor.deleteNative 等内部 native 方法。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 Java binding module entry
 * @details Defines JNI_OnLoad entry function, registers all HybridClass native methods,
 *          and initializes localization language support. Registration order follows
 *          dependencies: fbjni HybridData -> common(JPos/JToken/JDiagnostic) ->
 *          JProgram -> JSymbolTable -> JTypeTable -> JLexer -> JParser ->
 *          JAnalyser -> JIC10Local -> JTypeOfNode -> JLinker -> JIncLexer ->
 *          JIncParser -> JIncCompiler
 *          Enum types (TokenType/TokenCategory/OperandType/BasicType/TypeCategory)
 *          are defined directly in Java layer, no JNI registration needed.
 *          Note: fbjni is statically linked, so HybridDataOnLoad() must be called
 *          manually here to register HybridData.Destructor.deleteNative and other
 *          internal native methods.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include "common_java/base.hpp"
#include "common_java/build.hpp"
#include "ic10_compiler_java/lexer/j_token.hpp"
#include "ic10_compiler_java/lexer/j_lexer.hpp"
#include "ic10_compiler_java/locals/j_local.hpp"
#include "ic10_compiler_java/parser/j_program.hpp"
#include "ic10_compiler_java/parser/j_parser.hpp"
#include "ic10_compiler_java/semantic/j_symbol_table.hpp"
#include "ic10_compiler_java/semantic/j_type_table.hpp"
#include "ic10_compiler_java/semantic/j_analyser.hpp"
#include "ic10_compiler_java/semantic/j_type_of_node.hpp"
#include "ic10_compiler_java/link/j_linker.hpp"
#include "ic10_compiler_java/incremental/j_inc_lexer.hpp"
#include "ic10_compiler_java/incremental/j_inc_parser.hpp"
#include "ic10_compiler_java/incremental/j_inc_compiler.hpp"

namespace ic = stationeers::ic10;

/**
 * @if zh
 * @brief JNI 库加载入口
 * @details JVM 加载 ic10c_java 动态库时调用，完成以下初始化：
 *          1. 注册所有 HybridClass 的 native 方法
 *          2. 注册本地化语言（中文简体、美式英语）
 *          3. 设置默认语言为美式英语
 * @param vm Java 虚拟机指针
 * @param reserved 保留参数
 * @return 所需的 JNI 版本
 * @elseif en
 * @brief JNI library load entry
 * @details Called when JVM loads ic10c_java dynamic library, performs:
 *          1. Register all HybridClass native methods
 *          2. Register localization languages (Simplified Chinese, American English)
 *          3. Set default language to American English
 * @param vm Java virtual machine pointer
 * @param reserved Reserved parameter
 * @return Required JNI version
 * @endif
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
     return java::initialize(vm, [] {
        // fbjni 以静态库链接,需手动注册其内部 native 方法
        // (HybridData.Destructor.deleteNative 等)
        java::HybridDataOnLoad();

        stationeers::registerNatives();
         ic::JToken::registerNatives();
         ic::JProgram::registerNatives();
         ic::JSymbolTable::registerNatives();
         ic::JTypeTable::registerNatives();
         ic::JLexer::registerNatives();
         ic::JParser::registerNatives();
         ic::JAnalyser::registerNatives();
         ic::JIC10Local::registerNatives();
         ic::JTypeOfNode::registerNatives();
         ic::JLinker::registerNatives();
         ic::JIncLexer::registerNatives();
         ic::JIncParser::registerNatives();
         ic::JIncCompiler::registerNatives();
     });
}
