// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file doc_parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/16 16:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
// doc_parser.cpp
#include "ic10/pch/pch.hpp"
#include "ic10/parser/doc_parser.hpp"

#include <format>

namespace stationeers::ic10 {

    std::unordered_map<std::string, DocTagType> DocParser::tags_ = {
        {"device",     DocTagType::DEVICE    },
        {"end-device", DocTagType::END_DEVICE},
        {"enum",       DocTagType::ENUM      },
        {"end-enum",   DocTagType::END_ENUM  },
        {"name",       DocTagType::NAME      },
        {"desc",       DocTagType::DESC      },
        {"value",      DocTagType::VALUE     },
        {"slot",       DocTagType::SLOT      },
        {"logic",      DocTagType::LOGIC     },
        {"mode",       DocTagType::MODE      },
        {"logicSlot",  DocTagType::LOGIC_SLOT},
        {"connect",    DocTagType::CONNECT   }
    };

    DocParser::DocParser(
        const std::vector<std::shared_ptr<Token>>& tokens, std::size_t& idx,
        DiagnosticReporter<IC10MsgPack>& reporter
    )
        : tokens_(tokens)
        , idx_(idx)
        , reporter_(reporter) {
        initDeviceHandlers();
        initEnumHandlers();
    }

    // 解析描述文本（公共辅助）
    std::optional<DescValue> DocParser::parseDesc(
        const std::vector<std::string>& args, std::size_t index
    ) {
        if (index >= args.size()) return std::nullopt;

        const auto& arg = args[index];

        if (!arg.empty() && arg[0] == '$') return DescValue{DescValue::Kind::LINK, arg.substr(1)};

        return DescValue{DescValue::Kind::TEXT, arg};
    }

    // -------- 设备标签处理映射 --------
    void DocParser::initDeviceHandlers() {
        deviceHandlers_[DocTagType::NAME] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (!args.empty()) doc.name = args[0];
        };

        deviceHandlers_[DocTagType::DESC] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (auto d = parseDesc(args, 0)) doc.desc = *d;
        };

        deviceHandlers_[DocTagType::SLOT] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (args.size() < 2) return;
            DeviceSlot slot{};
            slot.index = args[0];
            if (args[1] == "input")
                slot.direction = SlotDirection::INPUT;
            else if (args[1] == "output")
                slot.direction = SlotDirection::OUTPUT;
            if (auto d = parseDesc(args, 2)) slot.desc = *d;
            doc.slots.push_back(std::move(slot));
        };

        deviceHandlers_[DocTagType::LOGIC] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (args.size() < 2) return;
            DeviceLogic logic;
            logic.name = args[0];
            if (args[1] == "r")
                logic.access = LogicAccess::R;
            else if (args[1] == "w")
                logic.access = LogicAccess::W;
            else if (args[1] == "rw" || args[1] == "wr")
                logic.access = LogicAccess::RW;
            doc.logics.push_back(std::move(logic));
        };

        deviceHandlers_[DocTagType::MODE] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (args.empty()) return;
            DeviceMode mode;
            mode.index = args[0];
            if (auto d = parseDesc(args, 1)) mode.desc = *d;
            doc.modes.push_back(std::move(mode));
        };

        deviceHandlers_[DocTagType::LOGIC_SLOT] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (!args.empty()) {
                DeviceLogicSlot slot{args[0]};
                doc.logicSlots.push_back(std::move(slot));
            }
        };

        deviceHandlers_[DocTagType::CONNECT] = [](const std::vector<std::string>& args, DeviceDocComment& doc) {
            if (args.empty()) return;
            DeviceConnect conn{args[0], {}};
            if (auto d = parseDesc(args, 1)) conn.desc = *d;
            doc.connects.push_back(std::move(conn));
        };
    }

    // -------- 枚举标签处理映射 --------
    void DocParser::initEnumHandlers() {
        enumHandlers_[DocTagType::NAME] = [](const std::vector<std::string>& args, EnumDocComment& doc) {
            if (!args.empty()) doc.name = args[0];
        };
        enumHandlers_[DocTagType::DESC] = [](const std::vector<std::string>& args, EnumDocComment& doc) {
            if (auto d = parseDesc(args, 0)) doc.desc = *d;
        };
        enumHandlers_[DocTagType::VALUE] = [](const std::vector<std::string>& args, EnumDocComment& doc) {
            if (args.size() < 2) return;
            EnumValueEntry entry{args[0], args[1], {}};
            if (auto d = parseDesc(args, 2)) entry.desc = *d;
            doc.values.push_back(std::move(entry));
        };
    }

    // -------- 解析入口 --------
    Errorable<DocComment> DocParser::parseDocCommentBlock() {
        if (!inScope() || current()->type != TokenType::DOC_COMMENT) {
            reporter_.error<IMsgId::IEP27>(current()->pos, endPos(*current()));
            return ErrorNode{*current(), ILoc::msgStr<IMsgId::IEP27>()};
        }

        auto firstTag = tryParseDocTag(current()->lexeme);
        if (!firstTag) {
            reporter_.error<IMsgId::IEP27>(current()->pos, endPos(*current()));
            auto err = *current();
            consume();
            return ErrorNode{err, ILoc::msgStr<IMsgId::IEP27>()};
        }

        auto [tagType, _] = *firstTag;

        if (tagType == DocTagType::DEVICE)
            return wide_cast<DocComment>(parseDeviceDocCommentBlock());

        if (tagType == DocTagType::ENUM)
            return wide_cast<DocComment>(parseEnumDocCommentBlock());

        reporter_.errorWith<IMsgId::IEP30>(current()->pos, endPos(*current()));
        auto err = *current();
        consume();
        return ErrorNode{err, ILoc::msgFormat<IMsgId::IEP30>()};
    }

    // -------- 设备块解析（简化后） --------
    ShallowErrorable<DeviceDocComment> DocParser::parseDeviceDocCommentBlock() {
        DeviceDocComment doc{current()->pos};
        consume();


        bool ended = false;
        while (inScope()) {
            if (current()->type == TokenType::NEWLINE) {
                consume();
                continue;
            }

            if (current()->type != TokenType::DOC_COMMENT) break;

            auto tagInfo = tryParseDocTag(current()->lexeme);
            if (!tagInfo) {
                // 无效标签 → 报告错误，消耗 token，继续扫描
                reporter_.error<IMsgId::IEP27>(current()->pos, endPos(*current()));
                consume();
                continue;
            }

            auto [tagType, args] = *tagInfo;
            if (tagType == DocTagType::END_DEVICE) {
                consume();
                ended = true;
                break;
            }

            if (auto it = deviceHandlers_.find(tagType); it != deviceHandlers_.end())
                it->second(args, doc);
            else
                // 未知标签 → 报告错误（不中断循环）
                reporter_.errorWith<IMsgId::IEP30>(current()->pos, endPos(*current()));

            consume();  // 消费当前 DOC_COMMENT
        }

        if (!ended) reporter_.error<IMsgId::IEP28>(current()->pos, endPos(*current()));
        return doc;
    }

    ShallowErrorable<EnumDocComment> DocParser::parseEnumDocCommentBlock() {
        EnumDocComment doc{current()->pos};
        consume();

        bool ended = false;
        while (inScope()) {
            if (current()->type == TokenType::NEWLINE) {
                consume();
                continue;
            }

            if (current()->type != TokenType::DOC_COMMENT) break;

            auto tagInfo = tryParseDocTag(current()->lexeme);
            if (!tagInfo) {
                consume();
                continue;
            }

            auto [tagType, args] = *tagInfo;
            if (tagType == DocTagType::END_ENUM) {
                consume();
                ended = true;
                break;
            }

            auto it = enumHandlers_.find(tagType);
            if (it != enumHandlers_.end()) it->second(args, doc);

            consume();
        }

        if (!ended) reporter_.error<IMsgId::IEP29>(current()->pos, endPos(*current()));
        return doc;
    }

    bool DocParser::inScope() const {
        return idx_ < tokens_.size() && tokens_[idx_]->type != TokenType::END;
    }

    std::shared_ptr<Token> DocParser::current(bool consume) const {
        if (idx_ < tokens_.size()) return tokens_[consume ? idx_++ : idx_];
        return nullptr;
    }

    void DocParser::consume() const {
        if (inScope()) ++idx_;
    }

    std::optional<std::pair<DocTagType, std::vector<std::string>>> DocParser::tryParseDocTag(
        const std::string& lexeme
    ) {
        auto content = lexeme;

        // 1. 去除首尾空白字符（包括 \r, \n, 空格, \t）
        auto start = content.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return std::nullopt;
        content.erase(0, start);

        auto end = content.find_last_not_of(" \t\r\n");
        if (end != std::string::npos)
            content.erase(end + 1);

        // 2. 检查是否有 "#>"
        if (content.size() < 3 || content[0] != '#' || content[1] != '>')
            return std::nullopt;

        // 3. 提取标签主体部分（跳过 "#>"）
        content = content.substr(2);

        // 4. 查找 '@' 符号
        auto firstNonSpace = content.find_first_not_of(" \t");
        if (firstNonSpace == std::string::npos || content[firstNonSpace] != '@')
            return std::nullopt;

        // 5. 提取标签名（直到遇到空格或结尾）
        auto tagStart = firstNonSpace + 1;
        auto tagEnd   = content.find_first_of(" \t", tagStart);
        if (tagEnd == std::string::npos)
            tagEnd = content.size();

        auto tagName = content.substr(tagStart, tagEnd - tagStart);

        // 去除标签名尾部可能残留的空白（如 '\r'）
        while (!tagName.empty() && std::isspace(static_cast<unsigned char>(tagName.back())))
            tagName.pop_back();

        // 6. 查询标签类型映射
        auto it = tags_.find(tagName);
        if (it == tags_.end())
            return std::nullopt;

        // 7. 提取参数
        std::vector<std::string> args;
        auto pos = tagEnd;
        while (pos < content.size()) {
            auto argStart = content.find_first_not_of(" \t", pos);
            if (argStart == std::string::npos)
                break;

            auto argEnd = content.find_first_of(" \t", argStart);
            if (argEnd == std::string::npos)
                argEnd = content.size();

            auto arg = content.substr(argStart, argEnd - argStart);
            // 去除参数尾部空白
            while (!arg.empty() && std::isspace(static_cast<unsigned char>(arg.back())))
                arg.pop_back();

            args.push_back(std::move(arg));
            pos = argEnd;
        }

        return std::make_pair(it->second, std::move(args));
    }

}  // namespace stationeers::ic10
