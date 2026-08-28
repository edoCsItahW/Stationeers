// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file token.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:04
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/lexer/token.hpp"
#include "common/utils/enum_to_str.hpp"
#include "common/utils/json.hpp"

#include <iostream>  // 避免`std::ostream& operator<<(std::ostream& os, const Token& token)`中出现错误
#include <format>

namespace stationeers::ic10 {

    const std::unordered_map<std::string, TokenType> OTHER_KEYWORD_MAP = {
        {"HASH", TokenType::KEYWORD_HASH},
        {"STR", TokenType::KEYWORD_STR},
        {"alias", TokenType::KEYWORD_ALIAS},
        {"define", TokenType::KEYWORD_DEFINE}
    };

const std::unordered_map<std::string, InstructionKeyword> INS_KEYWORD_MAP = {
    // 空指令
        {"hcf"    ,InstructionKeyword::HCF},

    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        {"yield"  ,InstructionKeyword::YIELD},
    #endif

    // 一元指令
        {"peek"   ,InstructionKeyword::PEEK},

    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        {"pop"    ,InstructionKeyword::POP},
        {"push"   ,InstructionKeyword::PUSH},
        {"clr"    ,InstructionKeyword::CLR},
        {"clrd"   ,InstructionKeyword::CLRD},
        {"j"      ,InstructionKeyword::J},
        {"jal"    ,InstructionKeyword::JAL},
        {"jr"     ,InstructionKeyword::JR},
        {"rand"   ,InstructionKeyword::RAND},
        {"sleep"  ,InstructionKeyword::SLEEP},
    #endif

    // 二元指令
        {"abs"    ,InstructionKeyword::ABS},
    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE

        {"acos"   ,InstructionKeyword::ACOS},
        {"asin"   ,InstructionKeyword::ASIN},
        {"atan"   ,InstructionKeyword::ATAN},
        {"atan2"  ,InstructionKeyword::ATAN2},
        {"ceil"   ,InstructionKeyword::CEIL},
        {"cos"    ,InstructionKeyword::COS},
        {"div"    ,InstructionKeyword::DIV},
        {"exp"    ,InstructionKeyword::EXP},
        {"floor"  ,InstructionKeyword::FLOOR},
        {"log"    ,InstructionKeyword::LOG},
        {"max"    ,InstructionKeyword::MAX},
        {"min"    ,InstructionKeyword::MIN},
        {"mod"    ,InstructionKeyword::MOD},
        {"mul"    ,InstructionKeyword::MUL},
        {"pow"    ,InstructionKeyword::POW},
        {"round"  ,InstructionKeyword::ROUND},
        {"sin"    ,InstructionKeyword::SIN},
        {"sqrt"   ,InstructionKeyword::SQRT},
        {"sgn"    ,InstructionKeyword::SGN},
        {"sub"    ,InstructionKeyword::SUB},
        {"tan"    ,InstructionKeyword::TAN},
        {"trunc"  ,InstructionKeyword::TRUNC},
        {"not"    ,InstructionKeyword::NOT},
        {"move"   ,InstructionKeyword::MOVE},
        {"poke"   ,InstructionKeyword::POKE},
        {"beqz"   ,InstructionKeyword::BEQZ},
        {"beqzal" ,InstructionKeyword::BEQZAL},
        {"bnez"   ,InstructionKeyword::BNEZ},
        {"bnezal" ,InstructionKeyword::BNEZAL},
        {"bgez"   ,InstructionKeyword::BGEZ},
        {"bgezal" ,InstructionKeyword::BGEZAL},
        {"bgtz"   ,InstructionKeyword::BGTZ},
        {"bgtzal" ,InstructionKeyword::BGTZAL},
        {"blez"   ,InstructionKeyword::BLEZ},
        {"blezal" ,InstructionKeyword::BLEZAL},
        {"bltz"   ,InstructionKeyword::BLTZ},
        {"bltzal" ,InstructionKeyword::BLTZAL},
        {"bnan"   ,InstructionKeyword::BNAN},
        {"bdns"   ,InstructionKeyword::BDNS},
        {"bdnsal" ,InstructionKeyword::BDNSAL},
        {"bdse"   ,InstructionKeyword::BDSE},
        {"bdseal" ,InstructionKeyword::BDSEAL},
        {"breqz"  ,InstructionKeyword::BREQZ},
        {"brgez"  ,InstructionKeyword::BRGEZ},
        {"brgtz"  ,InstructionKeyword::BRGTZ},
        {"brlez"  ,InstructionKeyword::BRLEZ},
        {"brltz"  ,InstructionKeyword::BRLTZ},
        {"brnan"  ,InstructionKeyword::BRNAN},
        {"brnez"  ,InstructionKeyword::BRNEZ},
        {"brdns"  ,InstructionKeyword::BRDNS},
        {"brdse"  ,InstructionKeyword::BRDSE},
        {"seqz"   ,InstructionKeyword::SEQZ},
        {"snez"   ,InstructionKeyword::SNEZ},
        {"sgez"   ,InstructionKeyword::SGEZ},
        {"sgtz"   ,InstructionKeyword::SGTZ},
        {"slez"   ,InstructionKeyword::SLEZ},
        {"sltz"   ,InstructionKeyword::SLTZ},
        {"snan"   ,InstructionKeyword::SNAN},
        {"snanz"  ,InstructionKeyword::SNANZ},
        {"sdns"   ,InstructionKeyword::SDNS},
        {"sdse"   ,InstructionKeyword::SDSE},
    #endif

    // 三元指令
        {"add"    ,InstructionKeyword::ADD},

    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        {"and"    ,InstructionKeyword::AND},
        {"nor"    ,InstructionKeyword::NOR},
        {"or"     ,InstructionKeyword::OR},
        {"sla"    ,InstructionKeyword::SLA},
        {"sll"    ,InstructionKeyword::SLL},
        {"sra"    ,InstructionKeyword::SRA},
        {"srl"    ,InstructionKeyword::SRL},
        {"xor"    ,InstructionKeyword::XOR},
        {"get"    ,InstructionKeyword::GET},
        {"put"    ,InstructionKeyword::PUT},
        {"l"      ,InstructionKeyword::L},
        {"ls"     ,InstructionKeyword::LS},
        {"lr"     ,InstructionKeyword::LR},
        {"s"      ,InstructionKeyword::S},
        {"sb"     ,InstructionKeyword::SB},
        {"rol"    ,InstructionKeyword::ROL},
        {"ror"    ,InstructionKeyword::ROR},
        {"rmap"   ,InstructionKeyword::RMAP},
        {"beq"    ,InstructionKeyword::BEQ},
        {"beqal"  ,InstructionKeyword::BEQAL},
        {"bne"    ,InstructionKeyword::BNE},
        {"bneal"  ,InstructionKeyword::BNEAL},
        {"bge"    ,InstructionKeyword::BGE},
        {"bgeal"  ,InstructionKeyword::BGEAL},
        {"bgt"    ,InstructionKeyword::BGT},
        {"bgtal"  ,InstructionKeyword::BGTAL},
        {"ble"    ,InstructionKeyword::BLE},
        {"bleal"  ,InstructionKeyword::BLEAL},
        {"blt"    ,InstructionKeyword::BLT},
        {"bltal"  ,InstructionKeyword::BLTAL},
        {"bapz"   ,InstructionKeyword::BAPZ},
        {"bapzal" ,InstructionKeyword::BAPZAL},
        {"bnaz"   ,InstructionKeyword::BNAZ},
        {"bnazal" ,InstructionKeyword::BNAZAL},
        {"bdnvl"  ,InstructionKeyword::BDNVL},
        {"bdnvs"  ,InstructionKeyword::BDNVS},
        {"breq"   ,InstructionKeyword::BREQ},
        {"brne"   ,InstructionKeyword::BRNE},
        {"brge"   ,InstructionKeyword::BRGE},
        {"brgt"   ,InstructionKeyword::BRGT},
        {"brle"   ,InstructionKeyword::BRLE},
        {"brlt"   ,InstructionKeyword::BRLT},
        {"brapz"  ,InstructionKeyword::BRAPZ},
        {"brnaz"  ,InstructionKeyword::BRNAZ},
        {"sapz"   ,InstructionKeyword::SAPZ},
        {"snaz"   ,InstructionKeyword::SNAZ},
        {"seq"    ,InstructionKeyword::SEQ},
        {"sne"    ,InstructionKeyword::SNE},
        {"sge"    ,InstructionKeyword::SGE},
        {"sgt"    ,InstructionKeyword::SGT},
        {"sle"    ,InstructionKeyword::SLE},
        {"slt"    ,InstructionKeyword::SLT},
    #endif

    // 四元指令
        {"clamp"  ,InstructionKeyword::CLAMP},

    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        {"lerp"   ,InstructionKeyword::LERP},
        {"ext"    ,InstructionKeyword::EXT},
        {"ins"    ,InstructionKeyword::INS},
        {"ss"     ,InstructionKeyword::SS},
        {"lb"     ,InstructionKeyword::LB},
        {"sbn"    ,InstructionKeyword::SBN},
        {"sbs"    ,InstructionKeyword::SBS},
        {"bap"    ,InstructionKeyword::BAP},
        {"bapal"  ,InstructionKeyword::BAPAL},
        {"bna"    ,InstructionKeyword::BNA},
        {"bnaal"  ,InstructionKeyword::BNAAL},
        {"brap"   ,InstructionKeyword::BRAP},
        {"brna"   ,InstructionKeyword::BRNA},
        {"sap"    ,InstructionKeyword::SAP},
        {"sna"    ,InstructionKeyword::SNA},
        {"select" ,InstructionKeyword::SELECT},
    #endif

    // 五元指令
        {"lbn"    ,InstructionKeyword::LBN},

    #ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        {"lbs"    ,InstructionKeyword::LBS},
    #endif

    // 六元指令
        {"lbns"   ,InstructionKeyword::LBNS},
    };


    const BiMap<TokenType, std::string> SYMBOL_MAP = {
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::COLON,  ":"},
        {TokenType::SUB, "-"},
        {TokenType::DOT,  "."},
        {TokenType::DIV, "/"}
    };

    const std::unordered_map<char, char> WHITESPACE_MAP = {
        {'\t', 't'},
        {'\n', 'n'},
        {'\r', 'r'},
        {'\b', 'b'}
    };

    const std::string SYMBOLS = "`~!@#$%^&*()-=+[{]}\\|;:'\",<.>/?";  // exclude '_'

    // Token

    std::string Token::toString() const {
        return std::format(
            "Token<{},{}>[{}]('{}')", pos.line(), pos.column(), enumToStr(type), lexeme
        );
    }

    std::string Token::toJSON() const {
        return toJson<"type", "pos", "lexeme", "category">(
            static_cast<int>(type),
            pos.toJSON(),
            lexeme,
            static_cast<int>(category)
        );
    }

    std::ostream& operator<<(std::ostream& os, const Token& token) {
        return os << token.toString();
    }

    Pos endPos(const Token& token) {
        auto end = token.pos;

        end.move(charLength(token.lexeme), token.lexeme.size());

        return end;
    }

}  // namespace ic10