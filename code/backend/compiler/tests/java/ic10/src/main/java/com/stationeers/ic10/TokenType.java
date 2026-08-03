// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package com.stationeers.ic10;

/**
 * Token type classification for IC10 lexer tokens.
 * <p>
 * Mirrors the C++ {@code enum class TokenType} in {@code ic10/lexer/token.hpp}.
 * Values are contiguous starting from 0, matching the C++ enum layout exactly.
 * Use {@link #getValue()} to compare with the integer returned by {@link Token#getType()}.
 * <p>
 * The 178 members are grouped by category:
 * <ul>
 *   <li>Literals (0-4): INTEGER, FLOAT, HEX_NUMBER, BINARY_NUMBER, STRING</li>
 *   <li>Names (5): IDENTIFIER</li>
 *   <li>Registers/Devices (6-7): REGISTER, DEVICE</li>
 *   <li>Symbols (8-10): LPAREN, RPAREN, COLON</li>
 *   <li>Comments (11-12): HEX_COMMENT, SLASH_COMMENT</li>
 *   <li>Annotations (13-14): DOC_COMMENT, TYPE_HINT</li>
 *   <li>Whitespace (15): NEWLINE</li>
 *   <li>Macro keywords (16-17): KEYWORD_HASH, KEYWORD_STR</li>
 *   <li>Constant keywords (18-26): KEYWORD_NAN ... KEYWORD_RGAS</li>
 *   <li>Special (27-28): END, UNKNOWN</li>
 *   <li>No-op instructions (29-30): KEYWORD_HCF, KEYWORD_YIELD</li>
 *   <li>Preprocessor (31-32): KEYWORD_ALIAS, KEYWORD_DEFINE</li>
 *   <li>Unary instructions (33-42): KEYWORD_PEEK ... KEYWORD_CLRD</li>
 *   <li>Binary instructions (43-105): KEYWORD_ABS ... KEYWORD_SDSE</li>
 *   <li>Ternary instructions (106-157): KEYWORD_AND ... KEYWORD_SLT</li>
 *   <li>Quaternary instructions (158-174): KEYWORD_LERP ... KEYWORD_SELECT</li>
 *   <li>Quinary instructions (175-176): KEYWORD_LBN, KEYWORD_LBS</li>
 *   <li>Senary instructions (177): KEYWORD_LBNS</li>
 * </ul>
 *
 * @author edocsitahw
 * @version 1.0.0
 * @since 1.0.0
 */
public enum TokenType {
    // ---- Literals ----
    /** Integer literal */
    INTEGER(0),
    /** Float literal */
    FLOAT(1),
    /** Hexadecimal number (0x...) */
    HEX_NUMBER(2),
    /** Binary number (0b...) */
    BINARY_NUMBER(3),
    /** String literal */
    STRING(4),

    // ---- Names ----
    /** Identifier */
    IDENTIFIER(5),

    // ---- Registers / Devices ----
    /** Register (r0-r15) */
    REGISTER(6),
    /** Device (@ prefix) */
    DEVICE(7),

    // ---- Symbols ----
    /** Left parenthesis ( */
    LPAREN(8),
    /** Right parenthesis ) */
    RPAREN(9),
    /** Colon : */
    COLON(10),

    // ---- Comments ----
    /** Hash comment (#) */
    HEX_COMMENT(11),
    /** Slash comment (//) */
    SLASH_COMMENT(12),

    // ---- Annotations ----
    /** Doc comment (#> prefix) */
    DOC_COMMENT(13),
    /** Type hint (#: prefix) */
    TYPE_HINT(14),

    // ---- Whitespace ----
    /** Newline */
    NEWLINE(15),

    // ---- Macro / function keywords ----
    /** Keyword: hash */
    KEYWORD_HASH(16),
    /** Keyword: str */
    KEYWORD_STR(17),

    // ---- Constant keywords ----
    /** Keyword: nan */
    KEYWORD_NAN(18),
    /** Keyword: pinf */
    KEYWORD_PINF(19),
    /** Keyword: ninf */
    KEYWORD_NINF(20),
    /** Keyword: pi */
    KEYWORD_PI(21),
    /** Keyword: tau */
    KEYWORD_TAU(22),
    /** Keyword: deg2rad */
    KEYWORD_DEG2RAD(23),
    /** Keyword: rad2deg */
    KEYWORD_RAD2DEG(24),
    /** Keyword: epsilon */
    KEYWORD_EPSILON(25),
    /** Keyword: rgas */
    KEYWORD_RGAS(26),

    // ---- Special ----
    /** End of file */
    END(27),
    /** Unknown token (error recovery) */
    UNKNOWN(28),

    // ---- No-op instructions ----
    /** Keyword: hcf */
    KEYWORD_HCF(29),
    /** Keyword: yield */
    KEYWORD_YIELD(30),

    // ---- Preprocessor directives ----
    /** Keyword: alias */
    KEYWORD_ALIAS(31),
    /** Keyword: define */
    KEYWORD_DEFINE(32),

    // ---- Unary instructions ----
    /** Keyword: peek */
    KEYWORD_PEEK(33),
    /** Keyword: pop */
    KEYWORD_POP(34),
    /** Keyword: push */
    KEYWORD_PUSH(35),
    /** Keyword: clr */
    KEYWORD_CLR(36),
    /** Keyword: j */
    KEYWORD_J(37),
    /** Keyword: jal */
    KEYWORD_JAL(38),
    /** Keyword: jr */
    KEYWORD_JR(39),
    /** Keyword: rand */
    KEYWORD_RAND(40),
    /** Keyword: sleep */
    KEYWORD_SLEEP(41),
    /** Keyword: clrd */
    KEYWORD_CLRD(42),

    // ---- Binary instructions ----
    /** Keyword: abs */
    KEYWORD_ABS(43),
    /** Keyword: acos */
    KEYWORD_ACOS(44),
    /** Keyword: add */
    KEYWORD_ADD(45),
    /** Keyword: asin */
    KEYWORD_ASIN(46),
    /** Keyword: atan */
    KEYWORD_ATAN(47),
    /** Keyword: atan2 */
    KEYWORD_ATAN2(48),
    /** Keyword: ceil */
    KEYWORD_CEIL(49),
    /** Keyword: cos */
    KEYWORD_COS(50),
    /** Keyword: div */
    KEYWORD_DIV(51),
    /** Keyword: exp */
    KEYWORD_EXP(52),
    /** Keyword: floor */
    KEYWORD_FLOOR(53),
    /** Keyword: log */
    KEYWORD_LOG(54),
    /** Keyword: max */
    KEYWORD_MAX(55),
    /** Keyword: min */
    KEYWORD_MIN(56),
    /** Keyword: mod */
    KEYWORD_MOD(57),
    /** Keyword: mul */
    KEYWORD_MUL(58),
    /** Keyword: pow */
    KEYWORD_POW(59),
    /** Keyword: round */
    KEYWORD_ROUND(60),
    /** Keyword: sin */
    KEYWORD_SIN(61),
    /** Keyword: sqrt */
    KEYWORD_SQRT(62),
    /** Keyword: sgn */
    KEYWORD_SGN(63),
    /** Keyword: sub */
    KEYWORD_SUB(64),
    /** Keyword: tan */
    KEYWORD_TAN(65),
    /** Keyword: trunc */
    KEYWORD_TRUNC(66),
    /** Keyword: not */
    KEYWORD_NOT(67),
    /** Keyword: move */
    KEYWORD_MOVE(68),
    /** Keyword: poke */
    KEYWORD_POKE(69),
    /** Keyword: beqz */
    KEYWORD_BEQZ(70),
    /** Keyword: beqzal */
    KEYWORD_BEQZAL(71),
    /** Keyword: bnez */
    KEYWORD_BNEZ(72),
    /** Keyword: bnezal */
    KEYWORD_BNEZAL(73),
    /** Keyword: bgez */
    KEYWORD_BGEZ(74),
    /** Keyword: bgezal */
    KEYWORD_BGEZAL(75),
    /** Keyword: bgtz */
    KEYWORD_BGTZ(76),
    /** Keyword: bgtzal */
    KEYWORD_BGTZAL(77),
    /** Keyword: blez */
    KEYWORD_BLEZ(78),
    /** Keyword: blezal */
    KEYWORD_BLEZAL(79),
    /** Keyword: bltz */
    KEYWORD_BLTZ(80),
    /** Keyword: bltzal */
    KEYWORD_BLTZAL(81),
    /** Keyword: bnan */
    KEYWORD_BNAN(82),
    /** Keyword: bdns */
    KEYWORD_BDNS(83),
    /** Keyword: bdnsal */
    KEYWORD_BDNSAL(84),
    /** Keyword: bdse */
    KEYWORD_BDSE(85),
    /** Keyword: bdseal */
    KEYWORD_BDSEAL(86),
    /** Keyword: breqz */
    KEYWORD_BREQZ(87),
    /** Keyword: brgez */
    KEYWORD_BRGEZ(88),
    /** Keyword: brgtz */
    KEYWORD_BRGTZ(89),
    /** Keyword: brlez */
    KEYWORD_BRLEZ(90),
    /** Keyword: brltz */
    KEYWORD_BRLTZ(91),
    /** Keyword: brnan */
    KEYWORD_BRNAN(92),
    /** Keyword: brnez */
    KEYWORD_BRNEZ(93),
    /** Keyword: brdns */
    KEYWORD_BRDNS(94),
    /** Keyword: brdse */
    KEYWORD_BRDSE(95),
    /** Keyword: seqz */
    KEYWORD_SEQZ(96),
    /** Keyword: snez */
    KEYWORD_SNEZ(97),
    /** Keyword: sgez */
    KEYWORD_SGEZ(98),
    /** Keyword: sgtz */
    KEYWORD_SGTZ(99),
    /** Keyword: slez */
    KEYWORD_SLEZ(100),
    /** Keyword: sltz */
    KEYWORD_SLTZ(101),
    /** Keyword: snan */
    KEYWORD_SNAN(102),
    /** Keyword: snanz */
    KEYWORD_SNANZ(103),
    /** Keyword: sdns */
    KEYWORD_SDNS(104),
    /** Keyword: sdse */
    KEYWORD_SDSE(105),

    // ---- Ternary instructions ----
    /** Keyword: and */
    KEYWORD_AND(106),
    /** Keyword: nor */
    KEYWORD_NOR(107),
    /** Keyword: or */
    KEYWORD_OR(108),
    /** Keyword: sla */
    KEYWORD_SLA(109),
    /** Keyword: sll */
    KEYWORD_SLL(110),
    /** Keyword: sra */
    KEYWORD_SRA(111),
    /** Keyword: srl */
    KEYWORD_SRL(112),
    /** Keyword: xor */
    KEYWORD_XOR(113),
    /** Keyword: get */
    KEYWORD_GET(114),
    /** Keyword: put */
    KEYWORD_PUT(115),
    /** Keyword: l */
    KEYWORD_L(116),
    /** Keyword: ls */
    KEYWORD_LS(117),
    /** Keyword: lr */
    KEYWORD_LR(118),
    /** Keyword: s */
    KEYWORD_S(119),
    /** Keyword: sb */
    KEYWORD_SB(120),
    /** Keyword: rol */
    KEYWORD_ROL(121),
    /** Keyword: ror */
    KEYWORD_ROR(122),
    /** Keyword: rmap */
    KEYWORD_RMAP(123),
    /** Keyword: beq */
    KEYWORD_BEQ(124),
    /** Keyword: beqal */
    KEYWORD_BEQAL(125),
    /** Keyword: bne */
    KEYWORD_BNE(126),
    /** Keyword: bneal */
    KEYWORD_BNEAL(127),
    /** Keyword: bge */
    KEYWORD_BGE(128),
    /** Keyword: bgeal */
    KEYWORD_BGEAL(129),
    /** Keyword: bgt */
    KEYWORD_BGT(130),
    /** Keyword: bgtal */
    KEYWORD_BGTAL(131),
    /** Keyword: ble */
    KEYWORD_BLE(132),
    /** Keyword: bleal */
    KEYWORD_BLEAL(133),
    /** Keyword: blt */
    KEYWORD_BLT(134),
    /** Keyword: bltal */
    KEYWORD_BLTAL(135),
    /** Keyword: bapz */
    KEYWORD_BAPZ(136),
    /** Keyword: bapzal */
    KEYWORD_BAPZAL(137),
    /** Keyword: bnaz */
    KEYWORD_BNAZ(138),
    /** Keyword: bnazal */
    KEYWORD_BNAZAL(139),
    /** Keyword: bdnvl */
    KEYWORD_BDNVL(140),
    /** Keyword: bdnvs */
    KEYWORD_BDNVS(141),
    /** Keyword: breq */
    KEYWORD_BREQ(142),
    /** Keyword: brne */
    KEYWORD_BRNE(143),
    /** Keyword: brge */
    KEYWORD_BRGE(144),
    /** Keyword: brgt */
    KEYWORD_BRGT(145),
    /** Keyword: brle */
    KEYWORD_BRLE(146),
    /** Keyword: brlt */
    KEYWORD_BRLT(147),
    /** Keyword: brapz */
    KEYWORD_BRAPZ(148),
    /** Keyword: brnaz */
    KEYWORD_BRNAZ(149),
    /** Keyword: sapz */
    KEYWORD_SAPZ(150),
    /** Keyword: snaz */
    KEYWORD_SNAZ(151),
    /** Keyword: seq */
    KEYWORD_SEQ(152),
    /** Keyword: sne */
    KEYWORD_SNE(153),
    /** Keyword: sge */
    KEYWORD_SGE(154),
    /** Keyword: sgt */
    KEYWORD_SGT(155),
    /** Keyword: sle */
    KEYWORD_SLE(156),
    /** Keyword: slt */
    KEYWORD_SLT(157),

    // ---- Quaternary instructions ----
    /** Keyword: lerp */
    KEYWORD_LERP(158),
    /** Keyword: clamp */
    KEYWORD_CLAMP(159),
    /** Keyword: ext */
    KEYWORD_EXT(160),
    /** Keyword: ins */
    KEYWORD_INS(161),
    /** Keyword: ss */
    KEYWORD_SS(162),
    /** Keyword: lb */
    KEYWORD_LB(163),
    /** Keyword: sbn */
    KEYWORD_SBN(164),
    /** Keyword: sbs */
    KEYWORD_SBS(165),
    /** Keyword: bap */
    KEYWORD_BAP(166),
    /** Keyword: bapal */
    KEYWORD_BAPAL(167),
    /** Keyword: bna */
    KEYWORD_BNA(168),
    /** Keyword: bnaal */
    KEYWORD_BNAAL(169),
    /** Keyword: brap */
    KEYWORD_BRAP(170),
    /** Keyword: brna */
    KEYWORD_BRNA(171),
    /** Keyword: sap */
    KEYWORD_SAP(172),
    /** Keyword: sna */
    KEYWORD_SNA(173),
    /** Keyword: select */
    KEYWORD_SELECT(174),

    // ---- Quinary instructions ----
    /** Keyword: lbn */
    KEYWORD_LBN(175),
    /** Keyword: lbs */
    KEYWORD_LBS(176),

    // ---- Senary instructions ----
    /** Keyword: lbns */
    KEYWORD_LBNS(177);

    private final int value;

    TokenType(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this token type
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves a {@link TokenType} from its integer value.
     *
     * @param value the integer value (0-177)
     * @return the matching token type
     * @throws IllegalArgumentException if the value is out of range
     */
    public static TokenType fromValue(int value) {
        TokenType[] all = values();
        if (value >= 0 && value < all.length) {
            return all[value];
        }
        throw new IllegalArgumentException("Invalid TokenType value: " + value);
    }
}
