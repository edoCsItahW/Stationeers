// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file expand_node_parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/01 18:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/parser/expand_node_parser.hpp"
#include "ic10_compiler/parser/parser.hpp"

namespace stationeers::ic10 {

    // Link

    ShallowErrorable<Link> NodeParser<Link>::parse(Parser& p) noexcept {
        auto tokenBeforeError = p.current();
        Link result{tokenBeforeError->pos};

        p.consume();  // DOT

        std::vector<std::vector<std::string>> references;

        tokenBeforeError = p.current();

        try {
            // DOT ( DIV Identifier ( DOT Identifier )* )+
            do {  // 至少一个 DIV Identifier ( DOT Identifier )* 即/xxx.xxx
                tokenBeforeError = p.expect(TokenType::DIV);

                Identifier identifier;
                if (tokenBeforeError = p.current(); tokenBeforeError && tokenBeforeError->type == TokenType::IDENTIFIER)  // 预检Identifier
                    identifier = NodeParser<Identifier>::parse(p);
                else [[unlikely]]
                    tokenBeforeError = p.expect(TokenType::IDENTIFIER);  // 引发错误，中断

                std::vector reference({std::move(identifier.value)});

                result.endPos = identifier.end();

                while (p.current() && p.current()->type == TokenType::DOT) {
                    tokenBeforeError = p.expect(TokenType::DOT);

                    Identifier id;
                    if (tokenBeforeError = p.current(); tokenBeforeError && tokenBeforeError->type == TokenType::IDENTIFIER)  // 预检Identifier
                        id = NodeParser<Identifier>::parse(p);
                    else [[unlikely]]
                        tokenBeforeError = p.expect(TokenType::IDENTIFIER);  // 引发错误，中断

                    result.endPos = id.end();

                    reference.push_back(std::move(id.value));
                }

                references.push_back(std::move(reference));

                tokenBeforeError = p.current();

            } while (tokenBeforeError && tokenBeforeError->type == TokenType::DIV);

        } catch (const Error& e) { return ErrorNode{*tokenBeforeError, std::string(e.message())}; }

        result.references = std::move(references);

        return result;
    }

    // TypeHintType

    TypeHintType NodeParser<TypeHintType>::parse(Parser& p) {
        // 已通过前瞻确定TokenType::TAG，无需try-catch
        TypeHintType result{p.expect(TokenType::TAG)->pos};

        if (const auto& tokenPtr = p.current();
            tokenPtr && tokenPtr->type == TokenType::IDENTIFIER)  // 预检Identifier
            result.value = NodeParser<Identifier>::parse(p).value;
        else [[unlikely]]
            p.expect(TokenType::IDENTIFIER);  // 引发错误，交给TypeHint

        return result;
    }

    bool NodeParser<TypeHintType>::is(const Parser& p) noexcept {
        const auto& tokenPtr = p.current();

        return tokenPtr->type == TokenType::TAG && tokenPtr->lexeme.substr(1) == "type";
    }

    // TypeHintDesc

    TypeHintDesc NodeParser<TypeHintDesc>::parse(Parser& p) noexcept {
        // 已通过前瞻确定TokenType::TAG，无需try-catch
        TypeHintDesc result{p.expect(TokenType::TAG)->pos};

        result.desc = p.matchVariant<decltype(TypeHintDesc::desc)>();

        return result;
    }

    bool NodeParser<TypeHintDesc>::is(const Parser& p) noexcept {
        const auto& t = p.current();

        return t->type == TokenType::TAG && t->lexeme.substr(1) == "desc";
    }

    // TypeHint

    TypeHint NodeParser<TypeHint>::parse(Parser& p) noexcept {
        // 已通过前瞻确定TokenType::TYPE_HINT_PREFIX，无需try-catch
        auto c = p.expect(TokenType::TYPE_HINT_PREFIX);

        TypeHint result{c->pos};
        result.endPos = endPos(*c);

        while (p.inScope()) {
            // @type
            if (!result.type && NodeParser<TypeHintType>::is(p)) {
                try {
                    result.type = NodeParser<TypeHintType>::parse(p);

                    result.endPos = result.type->end();
                } catch (const Error&) {
                    // 解析失败则放空result.type，错误已被except上报
                    result.type = std::nullopt;
                }

            }
            // @desc
            else if (!result.desc && NodeParser<TypeHintDesc>::is(p)) {
                result.desc = NodeParser<TypeHintDesc>::parse(p);

                result.endPos = result.desc->end();

            }
            // @builtin
            else if (
                auto tokenPtr = p.current(); !result.builtin && tokenPtr
                                             && tokenPtr->type == TokenType::TAG
                                             && tokenPtr->lexeme.substr(1) == "builtin"
            ) {
                result.endPos = tokenPtr->pos;

                result.builtin = true;

                p.consume();

            } else [[unlikely]]
                break;
        }

        return result;
    }

    // EnumAnnotationValue

    ShallowErrorable<EnumAnnotationValue> NodeParser<EnumAnnotationValue>::parse(
        Parser& p
    ) noexcept {
        // 已通过前瞻确定TokenType::TAG，无需try-catch
        EnumAnnotationValue result{p.expect(TokenType::TAG)->pos};

        const auto& tokenPtr = p.current();

        // name
        if (tokenPtr && tokenPtr->type == TokenType::IDENTIFIER) {  // 预检Identifier
            auto identifier = NodeParser<Identifier>::parse(p);

            result.endPos = identifier.end();

            result.name = std::move(identifier.value);
        } else [[unlikely]]
            try {
                p.expect(TokenType::IDENTIFIER);  // 引发错误，中断
            } catch (const Error& e) { return ErrorNode{*tokenPtr, std::string(e.message())}; }

        std::optional<ErrorNode> failResult;
        // value
        call(p.match<Integer>(), [&]<typename T, typename U = std::decay_t<T>>(T&& v) {
            if constexpr (std::is_same_v<U, ErrorNode>) {
                failResult = std::move(v);
            } else {
                result.endPos = v.end();

                result.value = std::move(v.value);
            }
        });

        if (failResult) return *failResult;

        // desc
        call(p.matchVariant<Description>(), [&]<typename T, typename U = std::decay_t<T>>(T&& v) {
            if constexpr (std::is_same_v<U, ErrorNode>) {
                failResult = std::move(v);
            } else {
                result.endPos = v.end();

                result.desc = std::move(v);
            }
        });

        if (failResult) return *failResult;

        return result;
    }

    bool NodeParser<EnumAnnotationValue>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "value";
    }

    // EnumAnnotation

    bool NodeParser<EnumAnnotation>::is(const Parser& p) noexcept {
        const auto& tokenPtr = p.peek(1);

        return tokenPtr && tokenPtr->type == TokenType::TAG && tokenPtr->lexeme.substr(1) == "enum";
    }

    ShallowErrorable<EnumAnnotation> NodeParser<EnumAnnotation>::parse(Parser& p) {
        // 已通过前瞻确定TokenType::TYPE_ANNOTATION_PREFIX，无需try-catch
        auto tokenBeforeError = p.expect(TokenType::TYPE_ANNOTATION_PREFIX);
        EnumAnnotation result{tokenBeforeError->pos};

        if (tokenBeforeError = p.current(); tokenBeforeError
                                            && tokenBeforeError->type == TokenType::TAG
                                            && tokenBeforeError->lexeme.substr(1) == "enum") {
            result.endPos = tokenBeforeError->pos;

            p.consume();
        } else [[unlikely]] {
            p.reporter_.error<ICMsgId::IEP30>(p.current()->pos, endPos(*p.current()));

            return ErrorNode{*tokenBeforeError, ICLoc::msgStr<ICMsgId::IEP30>()};
        }

        NodeParserDispatcher units{p, result};
        using Cardinality = decltype(units)::Cardinality;

        units.setPrefix(TokenType::TYPE_ANNOTATION_PREFIX);

        units.add<"name", Cardinality::REQUIRED>(
            [](const Parser& parser) {
                const auto& token = parser.current();

                return token->type == TokenType::TAG && token->lexeme.substr(1) == "name";
            },
            [](Parser& parser, auto& result) {
                parser.consume();  // TAG

                if (const auto& tokenPtr = parser.current();
                    tokenPtr && tokenPtr->type == TokenType::IDENTIFIER) {  // 预检Identifier
                    auto identifier = NodeParser<Identifier>::parse(parser);

                    result.endPos = identifier.end();

                    result.name = std::move(identifier.value);

                } else [[unlikely]]
                    result.name = "";  // 置空作为标志
            }
        );

        units.add<"desc", Cardinality::OPTIONAL>(
            [](Parser& parser) {
                auto token = parser.current();

                return token->type == TokenType::TAG && token->lexeme.substr(1) == "desc";
            },
            [](Parser& parser, auto& result) {
                parser.consume();  // TAG

                result.desc = parser.matchVariant<Description>();

                result.endPos = call(*result.desc, [](const auto& v) { return v.end(); });
            }
        );

        units.add<"values", Cardinality::AT_LEAST_ONCE>(
            [](const Parser& p) { return NodeParser<EnumAnnotationValue>::is(p); },
            [](Parser& p, auto& result) {
                auto value = NodeParser<EnumAnnotationValue>::parse(p);

                // 有则用，无则抛弃，错误由EnumAnnotationValue报告
                if (auto* v = std::get_if<EnumAnnotationValue>(&value); v) [[likely]] {
                    result.endPos = v->end();

                    result.values.push_back(std::move(*v));
                }
            }
        );

        units.until([](const Parser& parser) {
            const auto& tokenPtr = parser.current();

            // until() 内部已通过 expect(prefix) 消费了 #>，此时 current 应为 @end-enum TAG
            return tokenPtr && tokenPtr->type == TokenType::TAG
                && tokenPtr->lexeme.substr(1) == "end-enum";
        });

        if (result.name.empty()) {  // name违反REQUIRED
            p.reporter_.errorWith<ICMsgId::IEP33_1>(p.current()->pos, endPos(*p.current()), "name");

            return ErrorNode{*tokenBeforeError, ICLoc::msgFormat<ICMsgId::IEP33_1>("name")};
        }

        if (tokenBeforeError = p.current(); tokenBeforeError
                                            && tokenBeforeError->type == TokenType::TAG
                                            && tokenBeforeError->lexeme.substr(1) == "end-enum") {
            result.endPos = tokenBeforeError->pos;

            p.consume();
        } else [[unlikely]] {
            p.reporter_.error<ICMsgId::IEP29>(p.current()->pos, endPos(*p.current()));

            return ErrorNode{*tokenBeforeError, ICLoc::msgStr<ICMsgId::IEP29>()};
        }

        return result;
    }

    // DeviceAnnotationLogic

    DeviceAnnotationLogic NodeParser<DeviceAnnotationLogic>::parse(Parser& p) {
        DeviceAnnotationLogic result{p.expect(TokenType::TAG)->pos};

        if (p.current() && p.current()->type == TokenType::IDENTIFIER)
            result.value = std::move(NodeParser<Identifier>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::IDENTIFIER);  // 引发错误，交给DeviceAnnotation

        return result;
    }

    bool NodeParser<DeviceAnnotationLogic>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "logic";
    }

    // DeviceAnnotationLogicSlot

    DeviceAnnotationLogicSlot NodeParser<DeviceAnnotationLogicSlot>::parse(Parser& p) {
        DeviceAnnotationLogicSlot result{p.expect(TokenType::TAG)->pos};

        if (p.current() && p.current()->type == TokenType::IDENTIFIER)
            result.value = std::move(NodeParser<Identifier>::parse(p).value);

        else [[unlikely]]
            p.expect(TokenType::IDENTIFIER);  // 引发错误，交给DeviceAnnotation

        return result;
    }

    bool NodeParser<DeviceAnnotationLogicSlot>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "logic-slot";
    }

    // DeviceAnnotationDeviceHash

    DeviceAnnotationDeviceHash NodeParser<DeviceAnnotationDeviceHash>::parse(Parser& p) {
        DeviceAnnotationDeviceHash result{p.expect(TokenType::TAG)->pos};

        if (p.isMatch<Integer>())
            result.value = std::move(NodeParser<Integer>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::INTEGER);  // 引发错误，交给DeviceAnnotation

        return result;
    }

    bool NodeParser<DeviceAnnotationDeviceHash>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "device-hash";
    }

    // DeviceAnnotationNameHash

    DeviceAnnotationNameHash NodeParser<DeviceAnnotationNameHash>::parse(Parser& p) {
        DeviceAnnotationNameHash result{p.expect(TokenType::TAG)->pos};

        if (p.isMatch<Integer>())
            result.value = std::move(NodeParser<Integer>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::INTEGER);  // 引发错误，交给DeviceAnnotation

        return result;
    }

    bool NodeParser<DeviceAnnotationNameHash>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "name-hash";
    }

    // DeviceAnnotationReagentHash

    DeviceAnnotationReagentHash NodeParser<DeviceAnnotationReagentHash>::parse(Parser& p) {
        DeviceAnnotationReagentHash result{p.expect(TokenType::TAG)->pos};

        if (p.isMatch<Integer>())
            result.value = std::move(NodeParser<Integer>::parse(p).value);
        else [[unlikely]]
            p.expect(TokenType::INTEGER);  // 引发错误，交给DeviceAnnotation

        return result;
    }

    bool NodeParser<DeviceAnnotationReagentHash>::is(const Parser& p) {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "reagent-hash";
    }

    // DeviceAnnotationSlot

    DeviceAnnotationSlot NodeParser<DeviceAnnotationSlot>::parse(Parser& p) {
        DeviceAnnotationSlot result{p.expect(TokenType::TAG)->pos};

        if (p.current() && p.current()->type == TokenType::INTEGER)
            result.value = std::move(NodeParser<Integer>::parse(p).value);

        return result;
    }

    bool NodeParser<DeviceAnnotationSlot>::is(const Parser& p) noexcept {
        return p.current() && p.current()->type == TokenType::TAG
            && p.current()->lexeme.substr(1) == "slot";
    }

    // DeviceAnnotation

    bool NodeParser<DeviceAnnotation>::is(const Parser& p) noexcept {
        auto next = p.peek(1);

        return next && next->type == TokenType::TAG && next->lexeme.substr(1) == "device";
    }

    ShallowErrorable<DeviceAnnotation> NodeParser<DeviceAnnotation>::parse(Parser& p) {
        // 已通过前瞻确定TokenType::TYPE_ANNOTATION_PREFIX，无需try-catch
        auto tokenBeforeError = p.expect(TokenType::TYPE_ANNOTATION_PREFIX);
        DeviceAnnotation result{tokenBeforeError->pos};

        if (tokenBeforeError = p.current(); tokenBeforeError
                                            && tokenBeforeError->type == TokenType::TAG
                                            && tokenBeforeError->lexeme.substr(1) == "device") {
            result.endPos = tokenBeforeError->pos;

            p.consume();
        } else {
            p.reporter_.error<ICMsgId::IEP30>(p.current()->pos, endPos(*p.current()));

            return ErrorNode{*tokenBeforeError, ICLoc::msgStr<ICMsgId::IEP30>()};
        }

        NodeParserDispatcher units{p, result};
        using Cardinality = decltype(units)::Cardinality;

        units.setPrefix(TokenType::TYPE_ANNOTATION_PREFIX);

        units.add<"name", Cardinality::REQUIRED>(
            [](Parser& parser) {
                auto token = parser.current();

                return token->type == TokenType::TAG && token->lexeme.substr(1) == "name";
            },
            [](Parser& parser, auto& result) noexcept {
                parser.consume();

                if (const auto& tokenPtr = parser.current();
                    tokenPtr && tokenPtr->type == TokenType::IDENTIFIER) {  // 预检Identifier
                    auto identifier = NodeParser<Identifier>::parse(parser);

                    result.endPos = identifier.end();

                    result.name = std::move(identifier.value);

                } else [[unlikely]]
                    result.name = "";  // 置空作为标志
            }
        );

        units.add<"desc", Cardinality::OPTIONAL>(
            [](Parser& parser) {
                auto token = parser.current();

                return token->type == TokenType::TAG && token->lexeme.substr(1) == "desc";
            },
            [](Parser& parser, auto& result) noexcept {
                parser.consume();

                result.desc = parser.matchVariant<Description>();

                result.endPos = call(*result.desc, [](const auto& v) { return v.end(); });
            }
        );

        units.add<"device-hash", Cardinality::OPTIONAL>(
            [](Parser& parser) { return NodeParser<DeviceAnnotationDeviceHash>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto deviceHash = NodeParser<DeviceAnnotationDeviceHash>::parse(parser);

                    result.endPos = deviceHash.end();

                    result.deviceHash = std::move(deviceHash);
                } catch (const Error&) { result.deviceHash = std::nullopt; }
            }
        );

        units.add<"name-hash", Cardinality::OPTIONAL>(
            [](Parser& parser) { return NodeParser<DeviceAnnotationNameHash>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto nameHash = NodeParser<DeviceAnnotationNameHash>::parse(parser);

                    result.endPos = nameHash.end();

                    result.nameHash = std::move(nameHash);
                } catch (const Error&) { result.nameHash = std::nullopt; }
            }
        );

        units.add<"reagent-hash", Cardinality::REQUIRED>(
            [](Parser& parser) { return NodeParser<DeviceAnnotationReagentHash>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto reagentHash = NodeParser<DeviceAnnotationReagentHash>::parse(parser);

                    result.endPos = reagentHash.end();

                    result.reagentHashes.push_back(std::move(reagentHash));
                } catch (const Error&) {
                    // 解析失败则放空result.reagentHashes，错误已被except上报
                }
            }
        );

        units.add<"logics", Cardinality::REPEATED>(
            [](const Parser& parser) { return NodeParser<DeviceAnnotationLogic>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto logic = NodeParser<DeviceAnnotationLogic>::parse(parser);

                    result.endPos = logic.end();

                    result.logics.push_back(std::move(logic));
                } catch (const Error&) {
                    // 解析失败则放空result.logics，错误已被except上报
                }
            }
        );

        units.add<"logic-slots", Cardinality::REPEATED>(
            [](const Parser& parser) { return NodeParser<DeviceAnnotationLogicSlot>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto logicSlot = NodeParser<DeviceAnnotationLogicSlot>::parse(parser);

                    result.endPos = logicSlot.end();

                    result.logicSlots.push_back(std::move(logicSlot));
                } catch (const Error&) {
                    // 解析失败则放空result.logics，错误已被except上报
                }
            }
        );

        units.add<"slots", Cardinality::REPEATED>(
            [](const Parser& parser) { return NodeParser<DeviceAnnotationSlot>::is(parser); },
            [](Parser& parser, auto& result) noexcept {
                try {
                    auto slot = NodeParser<DeviceAnnotationSlot>::parse(parser);

                    result.endPos = slot.end();

                    result.slots.push_back(std::move(slot));

                } catch (const Error&) {
                    // 解析失败则放空result.slots，错误已被except上报
                }
            }
        );

        units.until([](const Parser& parser) {
            const auto& tokenPtr = parser.current();

            return tokenPtr && tokenPtr->type == TokenType::TAG
                && tokenPtr->lexeme.substr(1) == "end-device";
        });

        if (result.name.empty()) {  // name违反REQUIRED
            p.reporter_.errorWith<ICMsgId::IEP33_1>(p.current()->pos, endPos(*p.current()), "name");

            return ErrorNode{*tokenBeforeError, ICLoc::msgFormat<ICMsgId::IEP33_1>("name")};
        }

        if (auto tokenPtr = p.current(); tokenPtr && tokenPtr->type == TokenType::TAG
                                         && tokenPtr->lexeme.substr(1) == "end-device") {
            result.endPos = tokenPtr->pos;

            p.consume();
        } else {
            p.reporter_.error<ICMsgId::IEP28>(p.current()->pos, endPos(*p.current()));

            return ErrorNode{*tokenBeforeError, ICLoc::msgStr<ICMsgId::IEP28>()};
        }

        return result;
    }

}  // namespace stationeers::ic10
