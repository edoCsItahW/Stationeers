// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file doc_parser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/16 16:44
 * @brief 文档注释解析器
 * @details 解析IC10代码中的文档注释块，支持@device、@enum等标签，用于提取设备和枚举的元数据信息
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DOC_PARSER_HPP
#define COMPILER_DOC_PARSER_HPP
#pragma once

#include "ic10_compiler/locals/local.hpp"
#include "ic10_compiler/parser/ast/ast_node.hpp"
#include "common/exception/diagnostic.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @brief 文档注释标签类型
     * @elseif en
     * @brief Doc comment tag type
     * @endif
     */
    enum class DocTagType {
        DEVICE,       ///< @device
        END_DEVICE,   ///< @end-device
        ENUM,         ///< @enum
        END_ENUM,     ///< @end-enum
        NAME,         ///< @name
        DESC,         ///< @desc
        VALUE,        ///< @value
        SLOT,         ///< @slot
        LOGIC,        ///< @logic
        MODE,         ///< @mode
        LOGIC_SLOT,   ///< @logicSlot
        CONNECT,      ///< @connect
    };

    class DocParser {
    public:
        DocParser(
            const std::vector<std::shared_ptr<Token>>& tokens, std::size_t& idx,
            DiagnosticReporter<IC10CompilerMsgPack>& reporter
        );

        Errorable<DocComment> parseDocCommentBlock();

    private:
        const std::vector<std::shared_ptr<Token>>& tokens_;

        std::size_t& idx_;

        DiagnosticReporter<IC10CompilerMsgPack>& reporter_;

        static std::unordered_map<std::string, DocTagType> tags_;

        // 标签处理函数类型（用于 DeviceDocComment）
        using DeviceTagHandler =
            std::function<void(const std::vector<std::string>&, DeviceDocComment&)>;

        // 标签处理函数类型（用于 EnumDocComment）
        using EnumTagHandler =
            std::function<void(const std::vector<std::string>&, EnumDocComment&)>;

        std::unordered_map<DocTagType, DeviceTagHandler> deviceHandlers_;

        std::unordered_map<DocTagType, EnumTagHandler> enumHandlers_;

        // 初始化映射表
        void initDeviceHandlers();

        void initEnumHandlers();

        // 解析描述文本（处理 $link）
        static std::optional<DescValue> parseDesc(
            const std::vector<std::string>& args, std::size_t index
        );

        // 核心解析函数
        ShallowErrorable<DeviceDocComment> parseDeviceDocCommentBlock();

        ShallowErrorable<EnumDocComment> parseEnumDocCommentBlock();

        // 辅助工具
        [[nodiscard]] bool inScope() const;

        std::shared_ptr<Token> current(bool consume = false) const;

        void consume() const;

        static std::optional<std::pair<DocTagType, std::vector<std::string>>> tryParseDocTag(
            const std::string& lexeme
        );
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_DOC_PARSER_HPP
