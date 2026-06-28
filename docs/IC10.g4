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
KeywordBnanz  : 'bnanz';
KeywordBdns   : 'bdns';
KeywordBdnsal : 'bdnsal';
KeywordBdse   : 'bdse';
KeywordBdseal : 'bdseal';
KeywordBreqz  : 'breqz';
KeywordBrnz   : 'brnz';
KeywordBrgez  : 'brgez';
KeywordBrgtz  : 'brgtz';
KeywordBrlez  : 'brlez';
KeywordBrltz  : 'brltz';
KeywordBrnan  : 'brnan';
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
KeywordGasConstant : '19as';

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

// 数值
number : Integer
       | Float
       | HexNumber
       | BinaryNumber
       ;

// 字符串
string : String;

// 标识符
identifier : Identifier;

register : Register;

device : Device;

/* ==================== 语法规则 ==================== */

// 程序结构
program : statement (NEWLINE+ statement)* NEWLINE* EOF;

// 程序结构
/// 语句
statement : labelDef
          | preprocessorDirective
          | executableInstruction
          ;

// 语句
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
/// 别名指令
aliasDirective : KeywordAlias identifier registerOrDevice ;

/// 定义指令
defineDirective : KeywordDefine identifier operand ;

// 可执行指令
/// 空指令
nullaryInstruction : KeywordHcf
                   | KeywordYield
                   ;

/// 一元指令
unaryInstruction : KeywordSnanz registerOrIdentifier
                 | KeywordPeek registerOrIdentifier
                 | KeywordRand registerOrIdentifier
                 | KeywordSeqz registerOrIdentifier
                 | KeywordSnez registerOrIdentifier
                 | KeywordSgez registerOrIdentifier
                 | KeywordSgtz registerOrIdentifier
                 | KeywordSlez registerOrIdentifier
                 | KeywordSltz registerOrIdentifier
                 | KeywordSnan registerOrIdentifier
                 | KeywordPop registerOrIdentifier
                 | KeywordClr deviceReference
                 | KeywordSleep operand
                 | KeywordPush operand
                 | KeywordJal operand
                 | KeywordJr operand
                 | KeywordJ operand
                 ;

/// 二元指令
binaryInstruction : KeywordAbs registerOrIdentifier operand
                  | KeywordAcos registerOrIdentifier operand
                  | KeywordAsin registerOrIdentifier operand
                  | KeywordAtan registerOrIdentifier operand
                  | KeywordCeil registerOrIdentifier operand
                  | KeywordCos registerOrIdentifier operand
                  | KeywordExp registerOrIdentifier operand
                  | KeywordFloor registerOrIdentifier operand
                  | KeywordLog registerOrIdentifier operand
                  | KeywordRound registerOrIdentifier operand
                  | KeywordSin registerOrIdentifier operand
                  | KeywordSqrt registerOrIdentifier operand
                  | KeywordTan registerOrIdentifier operand
                  | KeywordTrunc registerOrIdentifier operand
                  | KeywordNot registerOrIdentifier operand
                  | KeywordMove registerOrIdentifier operand
                  | KeywordPoke operand operand
                  | KeywordBeqz operand operand
                  | KeywordBeqzal operand operand
                  | KeywordBnez operand operand
                  | KeywordBnezal operand operand
                  | KeywordBgez operand operand
                  | KeywordBgezal operand operand
                  | KeywordBgtz operand operand
                  | KeywordBgtzal operand operand
                  | KeywordBlez operand operand
                  | KeywordBlezal operand operand
                  | KeywordBltz operand operand
                  | KeywordBltzal operand operand
                  | KeywordBnan operand operand
                  | KeywordBnanz operand operand
                  | KeywordBdns deviceReference operand
                  | KeywordBdnsal deviceReference operand
                  | KeywordBdse deviceReference operand
                  | KeywordBdseal deviceReference operand
                  | KeywordBreqz operand operand
                  | KeywordBrnz operand operand
                  | KeywordBrgez operand operand
                  | KeywordBrgtz operand operand
                  | KeywordBrlez operand operand
                  | KeywordBrltz operand operand
                  | KeywordBrnan operand operand
                  | KeywordBrdns deviceReference operand
                  | KeywordBrdse deviceReference operand
                  | KeywordSub registerOrIdentifier operand  // 添加缺失的减法指令
                  | KeywordSdns registerOrIdentifier deviceReference
                  | KeywordSdse registerOrIdentifier deviceReference
                  ;

/// 三元指令
ternaryInstruction : KeywordAdd registerOrIdentifier operand operand
                   | KeywordAtan2 registerOrIdentifier operand operand
                   | KeywordDiv registerOrIdentifier operand operand
                   | KeywordMax registerOrIdentifier operand operand
                   | KeywordMin registerOrIdentifier operand operand
                   | KeywordMod registerOrIdentifier operand operand
                   | KeywordMul registerOrIdentifier operand operand
                   | KeywordPow registerOrIdentifier operand operand
                   | KeywordAnd registerOrIdentifier operand operand
                   | KeywordNor registerOrIdentifier operand operand
                   | KeywordOr registerOrIdentifier operand operand
                   | KeywordSla registerOrIdentifier operand operand
                   | KeywordSll registerOrIdentifier operand operand
                   | KeywordSra registerOrIdentifier operand operand
                   | KeywordSrl registerOrIdentifier operand operand
                   | KeywordXor registerOrIdentifier operand operand
                   | KeywordGet registerOrIdentifier deviceReference operand
                   | KeywordPut deviceReference operand operand
                   | KeywordL registerOrIdentifier deviceReference logicType
                   | KeywordLr registerOrIdentifier deviceReference reagentMode
                   | KeywordS deviceReference logicType registerOrIdentifier
                   | KeywordSb operand logicType registerOrIdentifier
                   | KeywordRmap registerOrIdentifier deviceReference operand
                   | KeywordBeq operand operand operand
                   | KeywordBeqal operand operand operand
                   | KeywordBne operand operand operand
                   | KeywordBneal operand operand operand
                   | KeywordBge operand operand operand
                   | KeywordBgeal operand operand operand
                   | KeywordBgt operand operand operand
                   | KeywordBgtal operand operand operand
                   | KeywordBle operand operand operand
                   | KeywordBleal operand operand operand
                   | KeywordBlt operand operand operand
                   | KeywordBltal operand operand operand
                   | KeywordBapz operand operand operand
                   | KeywordBapzal operand operand operand
                   | KeywordBnaz operand operand operand
                   | KeywordBnazal operand operand operand
                   | KeywordBdnvl deviceReference logicType operand
                   | KeywordBdnvs deviceReference logicType operand
                   | KeywordBreq operand operand operand
                   | KeywordBrne operand operand operand
                   | KeywordBrge operand operand operand
                   | KeywordBrgt operand operand operand
                   | KeywordBrle operand operand operand
                   | KeywordBrlt operand operand operand
                   | KeywordBrapz operand operand operand
                   | KeywordBrnaz operand operand operand
                   | KeywordSapz registerOrIdentifier operand operand
                   | KeywordSnaz registerOrIdentifier operand operand
                   | KeywordSeq registerOrIdentifier operand operand
                   | KeywordSne registerOrIdentifier operand operand
                   | KeywordSge registerOrIdentifier operand operand
                   | KeywordSgt registerOrIdentifier operand operand
                   | KeywordSle registerOrIdentifier operand operand
                   | KeywordSlt registerOrIdentifier operand operand
                   ;

/// 四元指令
quaternaryInstruction : KeywordLerp registerOrIdentifier operand operand operand
                      | KeywordExt registerOrIdentifier operand operand operand
                      | KeywordIns registerOrIdentifier operand operand operand
                      | KeywordSs deviceReference slotIndex logicSlotType registerOrIdentifier
                      | KeywordLb registerOrIdentifier operand logicType batchMode
                      | KeywordSbn operand operand logicType registerOrIdentifier
                      | KeywordSbs operand slotIndex logicSlotType registerOrIdentifier
                      | KeywordBap operand operand operand operand
                      | KeywordBapal operand operand operand operand
                      | KeywordBna operand operand operand operand
                      | KeywordBnaal operand operand operand operand
                      | KeywordBrap operand operand operand operand
                      | KeywordBrna operand operand operand operand
                      | KeywordSap registerOrIdentifier operand operand operand
                      | KeywordSna registerOrIdentifier operand operand operand
                      | KeywordSelect registerOrIdentifier operand operand operand
                      | KeywordLs registerOrIdentifier deviceReference slotIndex logicSlotType
                      ;

/// 五元指令
quinaryInstruction : KeywordLbn registerOrIdentifier operand operand logicType batchMode
                   | KeywordLbs registerOrIdentifier operand slotIndex logicSlotType batchMode
                   ;

/// 六元指令
senaryInstruction : KeywordLbns registerOrIdentifier operand operand slotIndex logicSlotType batchMode
                   ;

// 宏和函数
/// 宏调用
macroCall : hashCall
          | strCall
          ;

/// 哈希函数
hashCall : KeywordHash LPAREN string RPAREN;

/// 字符串函数
strCall : KeywordStr LPAREN string RPAREN;

// 数符定义
/// 操作数
operand : registerOrDevice
        | number
        | identifier
        | constant
        | macroCall
        ;

/// 寄存器或设备引用
registerOrDevice : registerOrIdentifier
                | device
                ;

/// 寄存器或标识符
registerOrIdentifier : register
                    | identifier
                    ;

/// 设备引用
deviceReference : device
                | registerOrIdentifier
                | number
                ;

// 常量
constant : KeywordNan
        | KeywordPinf
        | KeywordNinf
        | KeywordPi
        | KeywordTau
        | KeywordDeg2rad
        | KeywordRad2deg
        | KeywordEpsilon
        | KeywordGasConstant
        ;

// 特殊类型
/// 逻辑类型
logicType : identifier | number;

/// 槽位索引
slotIndex : number;

/// 逻辑槽类型
logicSlotType : identifier | number;

/// 批处理模式
batchMode : identifier | number;

/// 配方模式
reagentMode : identifier | number;
