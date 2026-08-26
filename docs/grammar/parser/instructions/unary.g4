grammar IC10;

import semanticOperand;


// 一元指令
unaryInstruction : KeywordClr  deviceRefStrict
                 | KeywordClrd hardwareId
                 | KeywordJ    jumpLine
                 | KeywordJal  jumpLine
                 | KeywordJr   jumpLine
                 | KeywordPeek regTarget
                 | KeywordPop  regTarget
                 | KeywordPush numValue
                 | KeywordRand regTarget
                 | KeywordSleep numValue
                 ;
