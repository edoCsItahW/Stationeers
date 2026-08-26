grammar IC10;

import semanticOperand;


// 五元指令
quinaryInstruction : KeywordLbn regTarget deviceHash nameHash    logicProp   aggMode
                    | KeywordLbs regTarget deviceHash slotIdx    logicSlotProp aggMode
                    ;
