/**
 * @file expand_node_parser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/19 14:18
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_COMPILER_CORE_EXPAND_NODE_PARSER_INL
#define IC10_COMPILER_CORE_EXPAND_NODE_PARSER_INL
#pragma once

namespace stationeers::ic10 {

    // 注：NodeParser<TypeHintDefault>::parse 是**显式特化**（非模板成员），定义放在
    // expand_node_parser.cpp 中，避免被多个 TU 各自实例化而重复定义（LNK2005）

    // TypeAnnotationLineBase<Name, Tag>

    template<FString Name, FString Tag>
    TypeAnnotationLineBase<Name, Tag> NodeParser<TypeAnnotationLineBase<Name, Tag>>::parse(
        Parser& p
    ) {
        TypeAnnotationLineBase<Name, Tag> result{p.expect(TokenType::TAG)->pos};

        if (p.current() && p.current()->type == TokenType::IDENTIFIER)
            result.name = std::move(NodeParser<Identifier>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::IDENTIFIER);

        if (p.isMatch<Integer>()) {
            auto integer = NodeParser<Integer>::parse(p);
            result.endPos = integer.end();
            result.value = std::move(integer.value);
        }
        else [[unlikely]]
            p.expect(TokenType::INTEGER);

        // 追加的默认值：`@logic Setting 12 1`；不写就是没有默认值（而非 0）
        if (p.isMatch<Integer>()) {
            auto defaultValue = NodeParser<Integer>::parse(p);

            result.endPos = defaultValue.end();
            result.defaultValue = std::move(defaultValue.value);
        }

        if (p.isVariantMatch<Description>()) {
            auto desc = p.matchVariant<Description>();
            result.endPos = call(desc, [](auto&& d) { return d.end(); });
            result.desc = std::move(desc);
        }

        return result;
    }

    template<FString Name, FString Tag>
    bool NodeParser<TypeAnnotationLineBase<Name, Tag>>::is(const Parser& p) noexcept {
        const auto& tokenPtr = p.current();

        return tokenPtr && tokenPtr->type == TokenType::TAG && std::string_view(Tag) == tokenPtr->lexeme.substr(1);
    }

    // TypeAnnotationValueBase<Name, Tag>

    template<FString Name, FString Tag>
    TypeAnnotationValueBase<Name, Tag> NodeParser<TypeAnnotationValueBase<Name, Tag>>::parse(
        Parser& p
    ) {
        TypeAnnotationValueBase<Name, Tag> result{p.expect(TokenType::TAG)->pos};

        if (p.isMatch<Integer>())
            result.value = std::move(NodeParser<Integer>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::INTEGER);

        return result;
    }

    template<FString Name, FString Tag>
    bool NodeParser<TypeAnnotationValueBase<Name, Tag>>::is(const Parser& p) noexcept {
        const auto& tokenPtr = p.current();

        return tokenPtr && tokenPtr->type == TokenType::TAG && std::string_view(Tag) == tokenPtr->lexeme.substr(1);
    }

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_EXPAND_NODE_PARSER_INL
