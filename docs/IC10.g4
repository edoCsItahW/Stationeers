grammar IC10;

/* ==================== 词法规则 ==================== */

// 关键字

/// 指令

//// 别名指令
KeywordAlias : 'alias';

//// 定义指令
KeywordDefine : 'define';

//// 空指令
KeywordHcf   : 'hcf';
KeywordYield : 'yield';

//// 一元指令
KeywordPeek  : 'peek';
KeywordPop   : 'pop';
KeywordPush  : 'push';
KeywordClr   : 'clr';
KeywordClrd  : 'clrd';
KeywordJ     : 'j';
KeywordJal   : 'jal';
KeywordJr    : 'jr';
KeywordRand  : 'rand';
KeywordSleep : 'sleep';

//// 二元指令
KeywordAbs    : 'abs';
KeywordAcos   : 'acos';
KeywordAdd    : 'add';
KeywordAsin   : 'asin';
KeywordAtan   : 'atan';
KeywordAtan2  : 'atan2';
KeywordCeil   : 'ceil';
KeywordCos    : 'cos';
KeywordDiv    : 'div';
KeywordExp    : 'exp';
KeywordFloor  : 'floor';
KeywordLog    : 'log';
KeywordMax    : 'max';
KeywordMin    : 'min';
KeywordMod    : 'mod';
KeywordMul    : 'mul';
KeywordPow    : 'pow';
KeywordRound  : 'round';
KeywordSin    : 'sin';
KeywordSqrt   : 'sqrt';
KeywordSgn    : 'sgn';
KeywordSub    : 'sub';
KeywordTan    : 'tan';
KeywordTrunc  : 'trunc';
KeywordNot    : 'not';
KeywordMove   : 'move';
KeywordPoke   : 'poke';
KeywordBeqz   : 'beqz';
KeywordBeqzal : 'beqzal';
KeywordBnez   : 'bnez';
KeywordBnezal : 'bnezal';
KeywordBgez   : 'bgez';
KeywordBgezal : 'bgezal';
KeywordBgtz   : 'bgtz';
KeywordBgtzal : 'bgtzal';
KeywordBlez   : 'blez';
KeywordBlezal : 'blezal';
KeywordBltz   : 'bltz';
KeywordBltzal : 'bltzal';
KeywordBnan   : 'bnan';
KeywordBdns   : 'bdns';
KeywordBdnsal : 'bdnsal';
KeywordBdse   : 'bdse';
KeywordBdseal : 'bdseal';
KeywordBreqz  : 'breqz';
KeywordBrgez  : 'brgez';
KeywordBrgtz  : 'brgtz';
KeywordBrlez  : 'brlez';
KeywordBrltz  : 'brltz';
KeywordBrnan  : 'brnan';
KeywordBrnez  : 'brnez';
KeywordBrdns  : 'brdns';
KeywordBrdse  : 'brdse';
KeywordSeqz   : 'seqz';
KeywordSnez   : 'snez';
KeywordSgez   : 'sgez';
KeywordSgtz   : 'sgtz';
KeywordSlez   : 'slez';
KeywordSltz   : 'sltz';
KeywordSnan   : 'snan';
KeywordSnanz  : 'snanz';
KeywordSdns   : 'sdns';
KeywordSdse   : 'sdse';

//// 三元指令
KeywordAnd    : 'and';
KeywordNor    : 'nor';
KeywordOr     : 'or';
KeywordSla    : 'sla';
KeywordSll    : 'sll';
KeywordSra    : 'sra';
KeywordSrl    : 'srl';
KeywordXor    : 'xor';
KeywordGet    : 'get';
KeywordPut    : 'put';
KeywordL      : 'l';
KeywordLs     : 'ls';
KeywordLr     : 'lr';
KeywordS      : 's';
KeywordSb     : 'sb';
KeywordRol    : 'rol';
KeywordRor    : 'ror';
KeywordRmap   : 'rmap';
KeywordBeq    : 'beq';
KeywordBeqal  : 'beqal';
KeywordBne    : 'bne';
KeywordBneal  : 'bneal';
KeywordBge    : 'bge';
KeywordBgeal  : 'bgeal';
KeywordBgt    : 'bgt';
KeywordBgtal  : 'bgtal';
KeywordBle    : 'ble';
KeywordBleal  : 'bleal';
KeywordBlt    : 'blt';
KeywordBltal  : 'bltal';
KeywordBapz   : 'bapz';
KeywordBapzal : 'bapzal';
KeywordBnaz   : 'bnaz';
KeywordBnazal : 'bnazal';
KeywordBdnvl  : 'bdnvl';
KeywordBdnvs  : 'bdnvs';
KeywordBreq   : 'breq';
KeywordBrne   : 'brne';
KeywordBrge   : 'brge';
KeywordBrgt   : 'brgt';
KeywordBrle   : 'brle';
KeywordBrlt   : 'brlt';
KeywordBrapz  : 'brapz';
KeywordBrnaz  : 'brnaz';
KeywordSapz   : 'sapz';
KeywordSnaz   : 'snaz';
KeywordSeq    : 'seq';
KeywordSne    : 'sne';
KeywordSge    : 'sge';
KeywordSgt    : 'sgt';
KeywordSle    : 'sle';
KeywordSlt    : 'slt';

//// 四元指令
KeywordLerp   : 'lerp';
KeywordClamp  : 'clamp';
KeywordExt    : 'ext';
KeywordIns    : 'ins';
KeywordSs     : 'ss';
KeywordLb     : 'lb';
KeywordSbn    : 'sbn';
KeywordSbs    : 'sbs';
KeywordBap    : 'bap';
KeywordBapal  : 'bapal';
KeywordBna    : 'bna';
KeywordBnaal  : 'bnaal';
KeywordBrap   : 'brap';
KeywordBrna   : 'brna';
KeywordSap    : 'sap';
KeywordSna    : 'sna';
KeywordSelect : 'select';

//// 五元指令
KeywordLbn : 'lbn';
KeywordLbs : 'lbs';

//// 六元指令
KeywordLbns : 'lbns';


/// 常量
KeywordNan          : 'nan';
KeywordPinf         : 'pinf';
KeywordNinf         : 'ninf';
KeywordPi           : 'pi';
KeywordTau          : 'tau';
KeywordDeg2rad      : 'deg2rad';
KeywordRad2deg      : 'rad2deg';
KeywordEpsilon      : 'epsilon';
KeywordRgas         : 'rgas';

/// 宏和函数
KeywordHash : 'HASH';
KeywordStr  : 'STR';

// 标识符
/// 变量名
Identifier : [a-zA-Z_] [a-zA-Z0-9_]*;

/// 寄存器设备
Register : 'r' ( [0-9] | '1' [0-5] );
Device   : 'd' [0-5];

// 数字
Integer      : [0-9]+;
Float        : [0-9]+ '.' [0-9]+ ( [eE] [+-]? [0-9]+ )?;
HexNumber    : '$' [0-9a-fA-F]+;
BinaryNumber : '%' [01_]+;

// 符号
LPAREN  : '(';
RPAREN  : ')';
COLON   : ':';

// 注释
COMMENT : ('#' | '//') ~[\r\n]*;

// 换行
NEWLINE : [\r\n];

// 字符串
String : '"' (~'"')* '"';

/* ==================== 词法对应语法 ==================== */

// 数字字面量
number : Integer
       | Float
       | HexNumber
       | BinaryNumber
       ;

// 字符串字面量
string : String;

// 标识符
identifier : Identifier;

// 寄存器字面量
register : Register;

// 设备字面量
device : Device;

// 常量
constant : KeywordNan
         | KeywordPinf
         | KeywordNinf
         | KeywordPi
         | KeywordTau
         | KeywordDeg2rad
         | KeywordRad2deg
         | KeywordEpsilon
         | KeywordRgas
         ;

// 宏调用
macroCall : hashCall
          | strCall
          ;

hashCall : KeywordHash LPAREN string RPAREN;

strCall : KeywordStr LPAREN string RPAREN;

// 寄存器引用 (r?)
// 语法层面：register + identifier（alias 可推导为寄存器）
registerRef : register
            | identifier
            ;

// 数字值 (num)
// 语法层面：number + constant + macroCall + identifier（define 可推导为数字）
numberValue : number
            | constant
            | macroCall
            | identifier
            ;

// 寄存器或数字 (r?|num)
// 语法层面：registerRef + number + constant + macroCall
// 注：identifier 已在 registerRef 和 numberValue 中
registerOrNumber : register
                 | identifier
                 | number
                 | constant
                 | macroCall
                 ;

// 设备引用 (device(d?|r?|id))
// 语法层面：device + register + identifier
deviceRef : device
          | register
          | identifier
          ;

// 设备别名引用 (d?)
// 语法层面：device + identifier（alias 可推导为设备）
deviceAliasRef : device
               | identifier
               ;

// 跳转目标 (int)
// 语法层面：与 numberValue 相同（行号/标签都是数字值）
jumpTarget : numberValue;

// 特殊类型
logicType : identifier | number;
slotIndex : number;
logicSlotType : identifier | number;
batchMode : identifier | number;
reagentMode : identifier | number;

/* ==================== 语法规则 ==================== */

// 程序结构
program : statement (NEWLINE+ statement)* NEWLINE* EOF;

/// 语句
statement : labelDef
          | preprocessorDirective
          | executableInstruction
          ;

/// 标签定义
labelDef : identifier COLON ;

/// 预处理指令
preprocessorDirective : aliasDirective
                      | defineDirective
                      ;

/// 可执行指令
executableInstruction : nullaryInstruction
                      | unaryInstruction
                      | binaryInstruction
                      | ternaryInstruction
                      | quaternaryInstruction
                      | quinaryInstruction
                      | senaryInstruction
                      ;

// 预处理指令
/// 别名指令 (alias str r?|d?)
aliasDirective : KeywordAlias identifier deviceRef ;

/// 定义指令
defineDirective : KeywordDefine identifier numberValue ;

/// 空指令
nullaryInstruction : KeywordHcf
                   | KeywordYield
                   ;

/// 一元指令
unaryInstruction : KeywordPeek registerRef
                 | KeywordRand registerRef
                 | KeywordPop registerRef
                 | KeywordClr deviceAliasRef
                 | KeywordClrd registerOrNumber
                 | KeywordSleep registerOrNumber
                 | KeywordPush registerOrNumber
                 | KeywordJal jumpTarget
                 | KeywordJr jumpTarget
                 | KeywordJ jumpTarget
                 ;

/// 二元指令
binaryInstruction : KeywordAbs registerRef registerOrNumber
                  | KeywordAcos registerRef registerOrNumber
                  | KeywordAsin registerRef registerOrNumber
                  | KeywordAtan registerRef registerOrNumber
                  | KeywordCeil registerRef registerOrNumber
                  | KeywordCos registerRef registerOrNumber
                  | KeywordExp registerRef registerOrNumber
                  | KeywordFloor registerRef registerOrNumber
                  | KeywordLog registerRef registerOrNumber
                  | KeywordRound registerRef registerOrNumber
                  | KeywordSin registerRef registerOrNumber
                  | KeywordSqrt registerRef registerOrNumber
                  | KeywordSgn registerRef registerOrNumber
                  | KeywordTan registerRef registerOrNumber
                  | KeywordTrunc registerRef registerOrNumber
                  | KeywordNot registerRef registerOrNumber
                  | KeywordMove registerRef registerOrNumber
                  | KeywordSeqz registerRef registerOrNumber
                  | KeywordSnez registerRef registerOrNumber
                  | KeywordSgez registerRef registerOrNumber
                  | KeywordSgtz registerRef registerOrNumber
                  | KeywordSlez registerRef registerOrNumber
                  | KeywordSltz registerRef registerOrNumber
                  | KeywordSnan registerRef registerOrNumber
                  | KeywordSnanz registerRef registerOrNumber
                  | KeywordPoke registerOrNumber registerOrNumber
                  | KeywordBeqz registerOrNumber registerOrNumber
                  | KeywordBeqzal registerOrNumber registerOrNumber
                  | KeywordBnez registerOrNumber registerOrNumber
                  | KeywordBnezal registerOrNumber registerOrNumber
                  | KeywordBgez registerOrNumber registerOrNumber
                  | KeywordBgezal registerOrNumber registerOrNumber
                  | KeywordBgtz registerOrNumber registerOrNumber
                  | KeywordBgtzal registerOrNumber registerOrNumber
                  | KeywordBlez registerOrNumber registerOrNumber
                  | KeywordBlezal registerOrNumber registerOrNumber
                  | KeywordBltz registerOrNumber registerOrNumber
                  | KeywordBltzal registerOrNumber registerOrNumber
                  | KeywordBnan registerOrNumber registerOrNumber
                  | KeywordBdns deviceRef registerOrNumber
                  | KeywordBdnsal deviceRef registerOrNumber
                  | KeywordBdse deviceRef registerOrNumber
                  | KeywordBdseal deviceRef registerOrNumber
                  | KeywordBreqz registerOrNumber registerOrNumber
                  | KeywordBrnez registerOrNumber registerOrNumber
                  | KeywordBrgez registerOrNumber registerOrNumber
                  | KeywordBrgtz registerOrNumber registerOrNumber
                  | KeywordBrlez registerOrNumber registerOrNumber
                  | KeywordBrltz registerOrNumber registerOrNumber
                  | KeywordBrnan registerOrNumber registerOrNumber
                  | KeywordBrdns deviceRef registerOrNumber
                  | KeywordBrdse deviceRef registerOrNumber
                  | KeywordSdns registerRef deviceRef
                  | KeywordSdse registerRef deviceRef
                  ;

/// 三元指令
ternaryInstruction : KeywordAdd registerRef registerOrNumber registerOrNumber
                   | KeywordSub registerRef registerOrNumber registerOrNumber
                   | KeywordAtan2 registerRef registerOrNumber registerOrNumber
                   | KeywordDiv registerRef registerOrNumber registerOrNumber
                   | KeywordMax registerRef registerOrNumber registerOrNumber
                   | KeywordMin registerRef registerOrNumber registerOrNumber
                   | KeywordMod registerRef registerOrNumber registerOrNumber
                   | KeywordMul registerRef registerOrNumber registerOrNumber
                   | KeywordPow registerRef registerOrNumber registerOrNumber
                   | KeywordAnd registerRef registerOrNumber registerOrNumber
                   | KeywordNor registerRef registerOrNumber registerOrNumber
                   | KeywordOr registerRef registerOrNumber registerOrNumber
                   | KeywordSla registerRef registerOrNumber registerOrNumber
                   | KeywordSll registerRef registerOrNumber registerOrNumber
                   | KeywordSra registerRef registerOrNumber registerOrNumber
                   | KeywordSrl registerRef registerOrNumber registerOrNumber
                   | KeywordXor registerRef registerOrNumber registerOrNumber
                   | KeywordRol registerRef registerOrNumber registerOrNumber
                   | KeywordRor registerRef registerOrNumber registerOrNumber
                   | KeywordGet registerRef deviceRef registerOrNumber
                   | KeywordPut deviceRef registerOrNumber registerOrNumber
                   | KeywordL registerRef deviceRef logicType
                   | KeywordS deviceRef logicType registerRef
                   | KeywordSb registerOrNumber logicType registerRef
                   | KeywordRmap registerRef deviceAliasRef registerOrNumber
                   | KeywordBeq registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBeqal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBne registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBneal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBge registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBgeal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBgt registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBgtal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBle registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBleal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBlt registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBltal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBapz registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBapzal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBnaz registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBnazal registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBdnvl deviceRef logicType registerOrNumber
                   | KeywordBdnvs deviceRef logicType registerOrNumber
                   | KeywordBreq registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrne registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrge registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrgt registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrle registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrlt registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrapz registerOrNumber registerOrNumber registerOrNumber
                   | KeywordBrnaz registerOrNumber registerOrNumber registerOrNumber
                   | KeywordSapz registerRef registerOrNumber registerOrNumber
                   | KeywordSnaz registerRef registerOrNumber registerOrNumber
                   | KeywordSeq registerRef registerOrNumber registerOrNumber
                   | KeywordSne registerRef registerOrNumber registerOrNumber
                   | KeywordSge registerRef registerOrNumber registerOrNumber
                   | KeywordSgt registerRef registerOrNumber registerOrNumber
                   | KeywordSle registerRef registerOrNumber registerOrNumber
                   | KeywordSlt registerRef registerOrNumber registerOrNumber
                   ;

/// 四元指令
quaternaryInstruction : KeywordLerp registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordClamp registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordExt registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordIns registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordSs deviceRef slotIndex logicSlotType registerRef
                      | KeywordLr registerRef deviceRef reagentMode jumpTarget
                      | KeywordLb registerRef registerOrNumber logicType batchMode
                      | KeywordSbn registerOrNumber registerOrNumber logicType registerRef
                      | KeywordSbs registerOrNumber slotIndex logicSlotType registerRef
                      | KeywordBap registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordBapal registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordBna registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordBnaal registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordBrap registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordBrna registerOrNumber registerOrNumber registerOrNumber registerOrNumber
                      | KeywordSap registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordSna registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordSelect registerRef registerOrNumber registerOrNumber registerOrNumber
                      | KeywordLs registerRef deviceRef slotIndex logicSlotType
                      ;

/// 五元指令
quinaryInstruction : KeywordLbn registerRef registerOrNumber registerOrNumber logicType batchMode
                   | KeywordLbs registerRef registerOrNumber slotIndex logicSlotType batchMode
                   ;

/// 六元指令
senaryInstruction : KeywordLbns registerRef registerOrNumber registerOrNumber slotIndex logicSlotType batchMode
                   ;
