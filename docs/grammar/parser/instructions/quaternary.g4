grammar IC10;

import semanticOperand;


// 四元指令
quaternaryInstruction : KeywordBap    numValue numValue numValue jumpLine
                       | KeywordBapal  numValue numValue numValue jumpLine
                       | KeywordBna    numValue numValue numValue jumpLine
                       | KeywordBnaal  numValue numValue numValue jumpLine
                       | KeywordBrap   numValue numValue numValue jumpLine
                       | KeywordBrna   numValue numValue numValue jumpLine
                       | KeywordClamp  regTarget numValue numValue numValue
                       | KeywordExt    regTarget numValue numValue numValue
                       | KeywordIns    regTarget numValue numValue numValue
                       | KeywordLb     regTarget deviceHash logicProp aggMode
                       | KeywordLerp   regTarget numValue numValue numValue
                       | KeywordLr     regTarget deviceRef reagentMode numValue
                       | KeywordLs     regTarget deviceRef slotIdx logicSlotProp
                       | KeywordSap    regTarget numValue numValue numValue
                       | KeywordSbn    deviceHash nameHash logicProp regTarget
                       | KeywordSbs    deviceHash slotIdx logicSlotProp regTarget
                       | KeywordSelect regTarget numValue numValue numValue
                       | KeywordSna    regTarget numValue numValue numValue
                       | KeywordSs     deviceRef slotIdx logicSlotProp regTarget
                       ;
