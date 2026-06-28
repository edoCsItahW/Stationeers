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
#include "ic10/lexer/token.hpp"
#include "common/utils/enum_to_str.hpp"

#include <iostream>  // 避免`std::ostream& operator<<(std::ostream& os, const Token& token)`中出现错误
#include <format>

namespace stationeers::ic10 {

    const BiMap<TokenType, std::string> KEYWORD_MAP = {
        // 宏与函数
        {TokenType::KEYWORD_HASH,         "HASH"   },
        {TokenType::KEYWORD_STR,          "STR"    },

        // 预处理指令
        {TokenType::KEYWORD_ALIAS,        "alias"  },
        {TokenType::KEYWORD_DEFINE,       "define" },

        // 空指令
        {TokenType::KEYWORD_HCF,          "hcf"    },
        {TokenType::KEYWORD_YIELD,        "yield"  },

        // 一元指令
        {TokenType::KEYWORD_PEEK,         "peek"   },
        {TokenType::KEYWORD_POP,          "pop"    },
        {TokenType::KEYWORD_PUSH,         "push"   },
        {TokenType::KEYWORD_CLR,          "clr"    },
        {TokenType::KEYWORD_J,            "j"      },
        {TokenType::KEYWORD_JAL,          "jal"    },
        {TokenType::KEYWORD_JR,           "jr"     },
        {TokenType::KEYWORD_RAND,         "rand"   },
        {TokenType::KEYWORD_SLEEP,        "sleep"  },

        // 二元指令
        {TokenType::KEYWORD_ABS,          "abs"    },
        {TokenType::KEYWORD_ACOS,         "acos"   },
        {TokenType::KEYWORD_ADD,          "add"    },
        {TokenType::KEYWORD_ASIN,         "asin"   },
        {TokenType::KEYWORD_ATAN,         "atan"   },
        {TokenType::KEYWORD_ATAN2,        "atan2"  },
        {TokenType::KEYWORD_CEIL,         "ceil"   },
        {TokenType::KEYWORD_COS,          "cos"    },
        {TokenType::KEYWORD_DIV,          "div"    },
        {TokenType::KEYWORD_EXP,          "exp"    },
        {TokenType::KEYWORD_FLOOR,        "floor"  },
        {TokenType::KEYWORD_LOG,          "log"    },
        {TokenType::KEYWORD_MAX,          "max"    },
        {TokenType::KEYWORD_MIN,          "min"    },
        {TokenType::KEYWORD_MOD,          "mod"    },
        {TokenType::KEYWORD_MUL,          "mul"    },
        {TokenType::KEYWORD_POW,          "pow"    },
        {TokenType::KEYWORD_ROUND,        "round"  },
        {TokenType::KEYWORD_SIN,          "sin"    },
        {TokenType::KEYWORD_SQRT,         "sqrt"   },
        {TokenType::KEYWORD_SUB,          "sub"    },
        {TokenType::KEYWORD_TAN,          "tan"    },
        {TokenType::KEYWORD_TRUNC,        "trunc"  },
        {TokenType::KEYWORD_NOT,          "not"    },
        {TokenType::KEYWORD_MOVE,         "move"   },
        {TokenType::KEYWORD_POKE,         "poke"   },
        {TokenType::KEYWORD_BEQZ,         "beqz"   },
        {TokenType::KEYWORD_BEQZAL,       "beqzal" },
        {TokenType::KEYWORD_BNEZ,         "bnez"   },
        {TokenType::KEYWORD_BNEZAL,       "bnezal" },
        {TokenType::KEYWORD_BGEZ,         "bgez"   },
        {TokenType::KEYWORD_BGEZAL,       "bgezal" },
        {TokenType::KEYWORD_BGTZ,         "bgtz"   },
        {TokenType::KEYWORD_BGTZAL,       "bgtzal" },
        {TokenType::KEYWORD_BLEZ,         "blez"   },
        {TokenType::KEYWORD_BLEZAL,       "blezal" },
        {TokenType::KEYWORD_BLTZ,         "bltz"   },
        {TokenType::KEYWORD_BLTZAL,       "bltzal" },
        {TokenType::KEYWORD_BNAN,         "bnan"   },
        {TokenType::KEYWORD_BNANZ,        "bnanz"  },
        {TokenType::KEYWORD_BDNS,         "bdns"   },
        {TokenType::KEYWORD_BDNSAL,       "bdnsal" },
        {TokenType::KEYWORD_BDSE,         "bdse"   },
        {TokenType::KEYWORD_BDSEAL,       "bdseal" },
        {TokenType::KEYWORD_BREQZ,        "breqz"  },
        {TokenType::KEYWORD_BRNZ,         "brnz"   },
        {TokenType::KEYWORD_BRGEZ,        "brgez"  },
        {TokenType::KEYWORD_BRGTZ,        "brgtz"  },
        {TokenType::KEYWORD_BRLEZ,        "brlez"  },
        {TokenType::KEYWORD_BRLTZ,        "brltz"  },
        {TokenType::KEYWORD_BRNAN,        "brnan"  },
        {TokenType::KEYWORD_BRDNS,        "brdns"  },
        {TokenType::KEYWORD_BRDSE,        "brdse"  },
        {TokenType::KEYWORD_SEQZ,         "seqz"   },
        {TokenType::KEYWORD_SNEZ,         "snez"   },
        {TokenType::KEYWORD_SGEZ,         "sgez"   },
        {TokenType::KEYWORD_SGTZ,         "sgtz"   },
        {TokenType::KEYWORD_SLEZ,         "slez"   },
        {TokenType::KEYWORD_SLTZ,         "sltz"   },
        {TokenType::KEYWORD_SNAN,         "snan"   },
        {TokenType::KEYWORD_SNANZ,        "snanz"  },
        {TokenType::KEYWORD_SDNS,         "sdns"   },
        {TokenType::KEYWORD_SDSE,         "sdse"   },

        // 三元指令
        {TokenType::KEYWORD_AND,          "and"    },
        {TokenType::KEYWORD_NOR,          "nor"    },
        {TokenType::KEYWORD_OR,           "or"     },
        {TokenType::KEYWORD_SLA,          "sla"    },
        {TokenType::KEYWORD_SLL,          "sll"    },
        {TokenType::KEYWORD_SRA,          "sra"    },
        {TokenType::KEYWORD_SRL,          "srl"    },
        {TokenType::KEYWORD_XOR,          "xor"    },
        {TokenType::KEYWORD_GET,          "get"    },
        {TokenType::KEYWORD_PUT,          "put"    },
        {TokenType::KEYWORD_L,            "l"      },
        {TokenType::KEYWORD_LS,           "ls"     },
        {TokenType::KEYWORD_LR,           "lr"     },
        {TokenType::KEYWORD_S,            "s"      },
        {TokenType::KEYWORD_SB,           "sb"     },
        {TokenType::KEYWORD_RMAP,         "rmap"   },
        {TokenType::KEYWORD_BEQ,          "beq"    },
        {TokenType::KEYWORD_BEQAL,        "beqal"  },
        {TokenType::KEYWORD_BNE,          "bne"    },
        {TokenType::KEYWORD_BNEAL,        "bneal"  },
        {TokenType::KEYWORD_BGE,          "bge"    },
        {TokenType::KEYWORD_BGEAL,        "bgeal"  },
        {TokenType::KEYWORD_BGT,          "bgt"    },
        {TokenType::KEYWORD_BGTAL,        "bgtal"  },
        {TokenType::KEYWORD_BLE,          "ble"    },
        {TokenType::KEYWORD_BLEAL,        "bleal"  },
        {TokenType::KEYWORD_BLT,          "blt"    },
        {TokenType::KEYWORD_BLTAL,        "bltal"  },
        {TokenType::KEYWORD_BAPZ,         "bapz"   },
        {TokenType::KEYWORD_BAPZAL,       "bapzal" },
        {TokenType::KEYWORD_BNAZ,         "bnaz"   },
        {TokenType::KEYWORD_BNAZAL,       "bnazal" },
        {TokenType::KEYWORD_BDNVL,        "bdnvl"  },
        {TokenType::KEYWORD_BDNVS,        "bdnvs"  },
        {TokenType::KEYWORD_BREQ,         "breq"   },
        {TokenType::KEYWORD_BRNE,         "brne"   },
        {TokenType::KEYWORD_BRGE,         "brge"   },
        {TokenType::KEYWORD_BRGT,         "brgt"   },
        {TokenType::KEYWORD_BRLE,         "brle"   },
        {TokenType::KEYWORD_BRLT,         "brlt"   },
        {TokenType::KEYWORD_BRAPZ,        "brapz"  },
        {TokenType::KEYWORD_BRNAZ,        "brnaz"  },
        {TokenType::KEYWORD_SAPZ,         "sapz"   },
        {TokenType::KEYWORD_SNAZ,         "snaz"   },
        {TokenType::KEYWORD_SEQ,          "seq"    },
        {TokenType::KEYWORD_SNE,          "sne"    },
        {TokenType::KEYWORD_SGE,          "sge"    },
        {TokenType::KEYWORD_SGT,          "sgt"    },
        {TokenType::KEYWORD_SLE,          "sle"    },
        {TokenType::KEYWORD_SLT,          "slt"    },

        // 四元指令
        {TokenType::KEYWORD_LERP,         "lerp"   },
        {TokenType::KEYWORD_EXT,          "ext"    },
        {TokenType::KEYWORD_INS,          "ins"    },
        {TokenType::KEYWORD_SS,           "ss"     },
        {TokenType::KEYWORD_LB,           "lb"     },
        {TokenType::KEYWORD_SBN,          "sbn"    },
        {TokenType::KEYWORD_SBS,          "sbs"    },
        {TokenType::KEYWORD_BAP,          "bap"    },
        {TokenType::KEYWORD_BAPAL,        "bapal"  },
        {TokenType::KEYWORD_BNA,          "bna"    },
        {TokenType::KEYWORD_BNAAL,        "bnaal"  },
        {TokenType::KEYWORD_BRAP,         "brap"   },
        {TokenType::KEYWORD_BRNA,         "brna"   },
        {TokenType::KEYWORD_SAP,          "sap"    },
        {TokenType::KEYWORD_SNA,          "sna"    },
        {TokenType::KEYWORD_SELECT,       "select" },

        // 五元指令
        {TokenType::KEYWORD_LBN,          "lbn"    },
        {TokenType::KEYWORD_LBS,          "lbs"    },

        // 六元指令
        {TokenType::KEYWORD_LBNS,         "lbns"   },

        // 常量
        {TokenType::KEYWORD_NAN,          "nan"    },
        {TokenType::KEYWORD_PINF,         "pinf"   },
        {TokenType::KEYWORD_NINF,         "ninf"   },
        {TokenType::KEYWORD_PI,           "pi"     },
        {TokenType::KEYWORD_TAU,          "tau"    },
        {TokenType::KEYWORD_DEG2RAD,      "deg2rad"},
        {TokenType::KEYWORD_RAD2DEG,      "rad2deg"},
        {TokenType::KEYWORD_EPSILON,      "epsilon"},
        {TokenType::KEYWORD_GAS_CONSTANT, "19as"   }
    };

    const BiMap<TokenType, std::string> SYMBOL_MAP = {
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::COLON,  ":"}
    };

    const std::unordered_map<char, char> WHITESPACE_MAP = {
        {'\t', 't'},
        {'\n', 'n'},
        {'\r', 'r'},
        {'\r', 'r'},
        {'\b', 'b'}
    };

    const std::string SYMBOLS = "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?";

    // Token

    std::string Token::toString() const {
        return std::format(
            "Token<{},{}>[{}]('{}')", pos.line(), pos.column(), enumToStr(type), lexeme
        );
    }

    std::string Token::toJSON() const {
        return std::format(R"({{"type": "{}", "lexeme": "{}"}})", enumToStr(type), lexeme);
    }

    std::ostream& operator<<(std::ostream& os, const Token& token) {
        return os << token.toString();
    }

    Pos endPos(const Token& token) {
        auto end = token.pos;

        end.move(token.lexeme.size());

        return end;
    }

}  // namespace ic10