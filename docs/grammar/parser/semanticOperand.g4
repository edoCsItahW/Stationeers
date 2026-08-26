grammar IC10;

import grammarOperand;

// r? : 目标寄存器（写入结果），必须是纯寄存器
regTarget : register ;

// r?|d? : （alias 的目标）可作为别名目标的寄存器或设备
regOrDev : register | device ;

// r?|num : 通用数值操作数（读取值参与运算/比较/赋值）
numValue : register | number | identifier | enum ;

// int, r?|num : （跳转指令中）跳转目标行号（绝对或相对），需解析为整数行号。
jumpLine : register | number | identifier ;

// address(r?|num) : 堆栈地址索引（用于 get/put/poke）
address : register | number | identifier | enum ;

// slotIdx : 设备槽位索引（需为非负整数）
slotIdx : register | number | identifier | enum ;

// id(r?|num) | （clrd/getd 等）设备数字硬件 ID（非端口名）
hardwareId : register | number | identifier | enum ;

// reagentHash(r?|num) : 试剂材料哈希值（用于配方查询）
reagentHash : register | number | identifier | enum ;

// device(d?|r?|id), d? : 设备引用（可为静态/动态端口或设备别名）
deviceRef : device | identifier ;

// d? : （clr 指令独有）严格设备端口（不接受别名或动态设备）
deviceRefStrict : device ;

// logicType : 设备逻辑属性路径（仅单标识符，如 Pressure、Activate）
logicProp : identifier ;

// logicSlotType : 槽位物品逻辑属性路径（仅单标识符）
logicSlotProp : identifier ;

// batchMode : 聚合模式（如 Average、Sum 或枚举常量）
aggMode : identifier | number | enum ;

// reagentMode : 试剂模式（如 Contents、Recipe）
reagentMode : identifier | number | enum ;

// deviceHash : 设备类型哈希值（支持字面量、常量别名、枚举、HASH 宏）
deviceHash : number | identifier | hashMacro ;

// nameHash : 设备名称哈希值
nameHash : number | identifier | hashMacro ;

// str :（alias 指令）别名定义名称（不含点号）
aliasDef : identifier ;

// str :（define 指令）常量定义名称（不含点号）
constDef : identifier ;

// num : （define 的值部分）纯数值字面量（仅数字）
constNum : number ;
