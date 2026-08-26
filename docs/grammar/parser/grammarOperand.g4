grammar IC10;

import lexer;

// 数值
interger : SUB? Integer ;

float : SUB? Float ;

binaryNumber : BinaryNumber ;

hexNumber : HexNumber ;

number : interger | float | binaryNumber | hexNumber ;

// 标识符
identifier : Identifier ;

// 寄存器
generalPurposeRegister : R Integer ;

addressRegister : RA ;

stackPointerRegister : SP ;

specialRegister : addressRegister | stackPointerRegister ;

staticRegister : specialRegister
               | generalPurposeRegister
               ;

// 用于产生嵌套ast结构
dynamicRegister : R ( dynamicRegister | generalPurposeRegister ) ;

register : staticRegister
         | dynamicRegister
         ;

// 设备
selfReferenceDevice : DB ;

ordinaryDevice : D Integer ;

staticDevice : ( selfReferenceDevice | ordinaryDevice ) ( COLON Integer )? ;

// 用于产生嵌套ast结构
zeroOrMoreRSuffix : R zeroOrMoreRSuffix | generalPurposeRegister ;

dynamicDevice : D zeroOrMoreRSuffix ;

device : staticDevice
       | dynamicDevice
       ;

// 枚举
enum : Identifier ( DOT Identifier )? ;

// 宏
hashMacro : KeywordHash LPAREN String RPAREN ;

strMacro : KeywordStr LPAREN String RPAREN ;

macro : hashMacro | strMacro ;
