grammar IC10;

import keyword;

// 换行
NEWLINE : [\r\n] ;

// 注释
COMMENT : '#' ~[\r\n]* ;

// 浮点数
Float : [0-9]+ '.' [0-9]+ ( [eE] [+-]? [0-9]+ )? ;

// 整数
Integer : [0-9]+ ;

// 二进制数
BinaryNumber : '%' [0-1_]+ ;

// 十六进制数
HexNumber : '$' [a-zA-Z0-9]+ ;

// 寄存器
//AddressRegister : 'ra' ;
RA : 'ra' ;

//StackPointerRegister : 'sp' ;
SP : 'sp' ;

R : 'r' ;

//SpecialRegister : AddressRegister | StackPointerRegister ;

//GeneralPurposeRegister : 'r' ( [0-9] | '1' [0-7] ) ;

//StaticRegister : SpecialRegister | GeneralPurposeRegister ;

//DynamicRegister : 'r'+ ( GeneralPurposeRegister | AddressRegister );

//Register : StaticRegister | DynamicRegister ;

// 设备引脚
//SelfReferenceDevice : 'db' ;
DB : 'db' ;

D : 'd' ;

//OrdinaryDevice : 'd' [0-5] ;

//StaticDevice : SelfReferenceDevice | OrdinaryDevice ;

//DynamicDevice : 'd' DynamicRegister ;

//Device : ( StaticDevice | DynamicDevice ) ( ':' [0-7] )? ;

// 标识符
Identifier : [a-zA-Z_] [a-zA-Z0-9_]* ;

String : '"' ~["]* '"' ;

// 符号
LPAREN : '(' ;
RPAREN : ')' ;
COLON  : ':' ;
DOT    : '.' ;
SUB    : '-' ;
