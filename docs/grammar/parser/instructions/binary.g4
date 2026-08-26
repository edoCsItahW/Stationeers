grammar IC10;

import semanticOperand;

// 二元指令
binaryInstruction : KeywordAbs    regTarget numValue
                   | KeywordAcos   regTarget numValue
                   | KeywordAsin   regTarget numValue
                   | KeywordAtan   regTarget numValue
                   | KeywordBdns   deviceRef jumpLine
                   | KeywordBdnsal deviceRef jumpLine
                   | KeywordBdse   deviceRef jumpLine
                   | KeywordBdseal deviceRef jumpLine
                   | KeywordBeqz   numValue jumpLine
                   | KeywordBeqzal numValue jumpLine
                   | KeywordBgez   numValue jumpLine
                   | KeywordBgezal numValue jumpLine
                   | KeywordBgtz   numValue jumpLine
                   | KeywordBgtzal numValue jumpLine
                   | KeywordBlez   numValue jumpLine
                   | KeywordBlezal numValue jumpLine
                   | KeywordBltz   numValue jumpLine
                   | KeywordBltzal numValue jumpLine
                   | KeywordBnan   numValue jumpLine
                   | KeywordBnez   numValue jumpLine
                   | KeywordBnezal numValue jumpLine
                   | KeywordBrdns  deviceRef jumpLine
                   | KeywordBrdse  deviceRef jumpLine
                   | KeywordBreqz  numValue jumpLine
                   | KeywordBrgez  numValue jumpLine
                   | KeywordBrgtz  numValue jumpLine
                   | KeywordBrlez  numValue jumpLine
                   | KeywordBrltz  numValue jumpLine
                   | KeywordBrnan  numValue jumpLine
                   | KeywordBrnez  numValue jumpLine
                   | KeywordCeil   regTarget numValue
                   | KeywordCos    regTarget numValue
                   | KeywordExp    regTarget numValue
                   | KeywordFloor  regTarget numValue
                   | KeywordLog    regTarget numValue
                   | KeywordMove   regTarget numValue
                   | KeywordNot    regTarget numValue
                   | KeywordPoke   address numValue
                   | KeywordRound  regTarget numValue
                   | KeywordSdns   regTarget deviceRef
                   | KeywordSdse   regTarget deviceRef
                   | KeywordSeqz   regTarget numValue
                   | KeywordSgez   regTarget numValue
                   | KeywordSgn    regTarget numValue
                   | KeywordSgtz   regTarget numValue
                   | KeywordSin    regTarget numValue
                   | KeywordSlez   regTarget numValue
                   | KeywordSltz   regTarget numValue
                   | KeywordSnan   regTarget numValue
                   | KeywordSnanz  regTarget numValue
                   | KeywordSnez   regTarget numValue
                   | KeywordSqrt   regTarget numValue
                   | KeywordTan    regTarget numValue
                   | KeywordTrunc  regTarget numValue
                   ;
