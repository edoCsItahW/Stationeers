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
#include <unordered_set>

namespace stationeers::ic10 {

    // Link

    ShallowErrorable<Link> NodeParser<Link>::parse(Parser& p) noexcept {
        auto tokenBeforeError = p.current();
        Link result{tokenBeforeError->pos};

        p.consume();  // DOT

        tokenBeforeError = p.current();

        try {
            // DOT ( DIV Identifier )+ ( DOT Identifier )*
            do {  // 至少一个 DIV Identifier 即/xxx
                tokenBeforeError = p.expect(TokenType::DIV);

                Identifier identifier;
                if (tokenBeforeError = p.current();
                    tokenBeforeError
                    && tokenBeforeError->type == TokenType::IDENTIFIER)  // 预检Identifier
                    identifier = NodeParser<Identifier>::parse(p);
                else [[unlikely]]
                    tokenBeforeError = p.expect(TokenType::IDENTIFIER);  // 引发错误，中断

                result.endPos = identifier.end();

                result.paths.push_back(std::move(identifier.value));

            } while (p.current() && p.current()->type == TokenType::DIV);

            while (p.current() && p.current()->type == TokenType::DOT) {
                tokenBeforeError = p.expect(TokenType::DOT);

                Identifier id;
                if (tokenBeforeError = p.current();
                    tokenBeforeError
                    && tokenBeforeError->type == TokenType::IDENTIFIER)  // 预检Identifier
                        id = NodeParser<Identifier>::parse(p);
                else [[unlikely]]
                    tokenBeforeError = p.expect(TokenType::IDENTIFIER);  // 引发错误，中断

                result.endPos = id.end();

                result.fields.push_back(std::move(id.value));
            }

        } catch (const Error& e) { return ErrorNode{*tokenBeforeError, std::string(e.message())}; }

        return result;
    }

    // TypeHintDefault

    TypeHintDefault NodeParser<TypeHintDefault>::parse(Parser& p) {
        /** 分组只允许这三种：与设备注解的成员行标签一致 */
        const auto isCategory = [](const std::string& name) {
            return name == "logic" || name == "logic-slot" || name == "slot";
        };

        /** 默认值的取值可以是字面量（整数/浮点/十六进制/二进制）或常量标识符 */
        const auto isValue = [](TokenType type) {
            return type == TokenType::INTEGER || type == TokenType::FLOAT || type == TokenType::HEX_NUMBER
                || type == TokenType::BINARY_NUMBER || type == TokenType::IDENTIFIER;
        };

        // 已通过前瞻确定 @default TAG，无需try-catch
        const auto tag = p.expect(TokenType::TAG);

        TypeHintDefault result{tag->pos};

        result.endPos = endPos(*tag);

        // `@default 分组 字段 值`（设备成员）或 `@default 值`（寄存器）：
        // 两个标识符在前是设备形式，否则整条就是寄存器的一个取值
        if (const auto first = p.current(); first && first->type == TokenType::IDENTIFIER)
            if (const auto second = p.peek(); second && second->type == TokenType::IDENTIFIER) {
                const auto category = first->lexeme;

                p.consume();

                if (!isCategory(category))
                    p.reporter_.errorWith<ICMsgId::IEP34_1>(
                        first->pos, endPos(*first), "logic, logic-slot, slot"
                    );

                result.category = category;
                result.name = std::move(NodeParser<Identifier>::parse(p).value);
            }

        // 负值：单独的 '-' 也算取值的一部分
        if (const auto current = p.current(); current && current->type == TokenType::SUB) {
            result.endPos = endPos(*current);
            result.value = current->lexeme;

            p.consume();
        }

        if (const auto current = p.current(); current && isValue(current->type)) {
            result.endPos = endPos(*current);
            result.value += current->lexeme;

            p.consume();
        } else [[unlikely]]
            p.expect(TokenType::INTEGER);  // 引发错误（缺少默认值）

        return result;
    }

    // TypeHint

    TypeHint NodeParser<TypeHint>::parse(Parser& p) noexcept {
        // 已通过前瞻确定TokenType::TYPE_HINT_PREFIX，无需try-catch
        auto c = p.expect(TokenType::TYPE_HINT_PREFIX);

        TypeHint result{c->pos};
        result.endPos = endPos(*c);

        NodeParserDispatcher units{p, result};
        using Cardinality = decltype(units)::Cardinality;

        units.add<"type", Cardinality::OPTIONAL>(
            [](Parser& parser) {
                const auto& tokenPtr = parser.current();

                return tokenPtr && tokenPtr->type == TokenType::TAG
                    && tokenPtr->lexeme.substr(1) == "type";
            },
            [](Parser& parser, auto& result) {
                parser.consume();  // TAG

                if (const auto& tokenPtr = parser.current();
                    tokenPtr && tokenPtr->type == TokenType::IDENTIFIER)
                    [[likely]] {  // 预检Identifier
                    auto identifier = NodeParser<Identifier>::parse(parser);

                    result.endPos = identifier.end();

                    result.type = std::move(identifier.value);
                }
            }
        );

        units.add<"desc", Cardinality::OPTIONAL>(
            [](Parser& parser) {
                const auto& tokenPtr = parser.current();

                return tokenPtr && tokenPtr->type == TokenType::TAG
                    && tokenPtr->lexeme.substr(1) == "desc";
            },
            [](Parser& parser, auto& result) {
                parser.consume();  // TAG

                result.desc = parser.matchVariant<Description>();

                result.endPos = call(*result.desc, [](const auto& v) { return v.end(); });
            }
        );

        units.add<"builtin", Cardinality::OPTIONAL>(
            [](Parser& parser) {
                const auto& tokenPtr = parser.current();

                return tokenPtr && tokenPtr->type == TokenType::TAG
                    && tokenPtr->lexeme.substr(1) == "builtin";
            },
            [](Parser& parser, auto& result) {
                // 不为空
                auto token = parser.expect(TokenType::TAG);

                result.endPos = token->pos;

                result.builtin = true;
            }
        );

        units.add<"defaults", Cardinality::REPEATED>(
            [](Parser& parser) {
                const auto& tokenPtr = parser.current();

                return tokenPtr && tokenPtr->type == TokenType::TAG && tokenPtr->lexeme.substr(1) == "default";
            },
            [](Parser& parser, auto& result) noexcept {
                TypeHintDefault entry;

                // 取值缺失时 expect 会抛出：本 lambda 与 TypeHint::parse 都是 noexcept，
                // 抛出会直接 terminate（用户才敲到 `#: @default` 就会崩掉语言服务），
                // 因此就地捕获——诊断已由 expect 上报，这里只需丢弃这一条
                try {
                    entry = NodeParser<TypeHintDefault>::parse(parser);
                } catch (const Error&) { return; }

                // 同一分组的同一字段只允许一次：重复时报错并丢弃后一条
                if (entry.name)
                    if (const auto& it = std::ranges::find_if(
                            result.defaults, [&](const auto& d) {
                                return d.category == entry.category && d.name == entry.name;
                            }
                        );
                        it != result.defaults.end()) {
                        parser.reporter_.errorWith<ICMsgId::IEP37_1>(
                            entry.start(), entry.end(), std::format("{} {}", *entry.category, *entry.name)
                        );

                        return;
                    }

                result.endPos = entry.end();

                result.defaults.push_back(std::move(entry));
            }
        );

        units.until([](const Parser& parser) {
            const auto& tokenPtr = parser.current();

            static const std::unordered_set<std::string> set{"type", "desc", "default", "builtin"};


            return !tokenPtr || tokenPtr->type != TokenType::TAG
                || !set.contains(tokenPtr->lexeme.substr(1));
        });

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
        if (p.isVariantMatch<Description>())
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
