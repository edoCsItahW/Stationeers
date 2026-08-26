grammar IC10;

import semanticOperand;


// 三元指令
ternaryInstruction : KeywordAdd    regTarget numValue numValue
                    | KeywordAnd    regTarget numValue numValue
                    | KeywordAtan2  regTarget numValue numValue
                    | KeywordBapz   numValue numValue jumpLine
                    | KeywordBapzal numValue numValue jumpLine
                    | KeywordBdnvl  deviceRef logicProp jumpLine
                    | KeywordBdnvs  deviceRef logicProp jumpLine
                    | KeywordBeq    numValue numValue jumpLine
                    | KeywordBeqal  numValue numValue jumpLine
                    | KeywordBge    numValue numValue jumpLine
                    | KeywordBgeal  numValue numValue jumpLine
                    | KeywordBgt    numValue numValue jumpLine
                    | KeywordBgtal  numValue numValue jumpLine
                    | KeywordBle    numValue numValue jumpLine
                    | KeywordBleal  numValue numValue jumpLine
                    | KeywordBlt    numValue numValue jumpLine
                    | KeywordBltal  numValue numValue jumpLine
                    | KeywordBnaz   numValue numValue jumpLine
                    | KeywordBnazal numValue numValue jumpLine
                    | KeywordBne    numValue numValue jumpLine
                    | KeywordBneal  numValue numValue jumpLine
                    | KeywordBrapz  numValue numValue jumpLine
                    | KeywordBreq   numValue numValue jumpLine
                    | KeywordBrge   numValue numValue jumpLine
                    | KeywordBrgt   numValue numValue jumpLine
                    | KeywordBrle   numValue numValue jumpLine
                    | KeywordBrlt   numValue numValue jumpLine
                    | KeywordBrnaz  numValue numValue jumpLine
                    | KeywordBrne   numValue numValue jumpLine
                    | KeywordDiv    regTarget numValue numValue
                    | KeywordGet    regTarget deviceRef address
                    | KeywordL      regTarget deviceRef logicProp
                    | KeywordMax    regTarget numValue numValue
                    | KeywordMin    regTarget numValue numValue
                    | KeywordMod    regTarget numValue numValue
                    | KeywordMul    regTarget numValue numValue
                    | KeywordNor    regTarget numValue numValue
                    | KeywordOr     regTarget numValue numValue
                    | KeywordPow    regTarget numValue numValue
                    | KeywordPut    deviceRef address numValue
                    | KeywordRmap   regTarget deviceRefStrict reagentHash
                    | KeywordRol    regTarget numValue numValue
                    | KeywordRor    regTarget numValue numValue
                    | KeywordS      deviceRef logicProp regTarget
                    | KeywordSapz   regTarget numValue numValue
                    | KeywordSb     deviceHash logicProp regTarget
                    | KeywordSeq    regTarget numValue numValue
                    | KeywordSge    regTarget numValue numValue
                    | KeywordSgt    regTarget numValue numValue
                    | KeywordSla    regTarget numValue numValue
                    | KeywordSle    regTarget numValue numValue
                    | KeywordSll    regTarget numValue numValue
                    | KeywordSlt    regTarget numValue numValue
                    | KeywordSnaz   regTarget numValue numValue
                    | KeywordSne    regTarget numValue numValue
                    | KeywordSra    regTarget numValue numValue
                    | KeywordSrl    regTarget numValue numValue
                    | KeywordSub    regTarget numValue numValue
                    | KeywordXor    regTarget numValue numValue
                    ;
