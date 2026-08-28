grammar IC10;

/**
 * This is the syntax file of IC10 language, including lexical and syntactic parts, for implementation reference only.
 * It does not pursue syntactic correctness and cannot be run by antlr4
 * */

import lexer;

import parser;


// 专用于实现本项目的标准库而拓展于IC10语言的语法

HASH : '#' ;
GT : '>' ;
AT : '@' ;
DIV : '/' ;

link : DOT ( DIV Identifier ( DOT Identifier )* )+ ;

description : link
            | string
            ;

// 类型提示
typeHintPrefix : HASH COLON ;

typeTag : AT 'type' Identifier ;

descriptionTag : AT 'desc' description ;

builtinTag : AT 'builtin' ;

typeHint : typeHintPrefix typeTag? descriptionTag? builtinTag? ;

// 类型注解

nameTag : AT 'name' Identifier ;

// 枚举标签
enumStartTag : AT 'enum' ;

enumEndTag : AT 'end-enum' ;

enumValueTag : AT 'value' Identifier Integer description? ;

// 设备标签
deviceStartTag : AT 'device' ;

deviceEndTag : AT 'end-device' ;

deviceLogicTag : AT 'logic' Identifier ;

deviceLogicSlotTag : AT 'logic-slot' Identifier ;

deviceHashTag : AT 'device-hash' Integer ;

deviceNameHashTag : AT 'name-hash' Integer ;

deviceReagentHashTag : AT 'reagent-hash' Integer ;

// 类型注解
typeAnnotationPrefix : HASH GT ;

enumTypeAnnoStart : typeAnnotationPrefix enumStartTag NEWLINE ;

enumTypeAnnoEnd : typeAnnotationPrefix enumEndTag NEWLINE ;

enumTypeAnnoName : typeAnnotationPrefix nameTag NEWLINE ;

enumTypeAnnoDesc : typeAnnotationPrefix descriptionTag NEWLINE ;

enumTypeAnnoValue : typeAnnotationPrefix enumValueTag NEWLINE ;

enumTypeAnno : enumTypeAnnoStart
               enumTypeAnnoName
               enumTypeAnnoDesc?
               enumTypeAnnoValue*
               enumTypeAnnoEnd ;

deviceTypeAnnoStart : typeAnnotationPrefix deviceStartTag NEWLINE ;

deviceTypeAnnoEnd : typeAnnotationPrefix deviceEndTag NEWLINE ;

deviceTypeAnnoName : typeAnnotationPrefix nameTag NEWLINE ;

deviceTypeAnnoDesc : typeAnnotationPrefix descriptionTag NEWLINE ;

deviceTypeAnnoLogic : typeAnnotationPrefix deviceLogicTag NEWLINE ;

deviceTypeAnnoLogicSlot : typeAnnotationPrefix deviceLogicSlotTag NEWLINE ;

deviceTypeAnnoHash : typeAnnotationPrefix deviceHashTag NEWLINE ;

deviceTypeAnnoNameHash : typeAnnotationPrefix deviceNameHashTag NEWLINE ;

deviceTypeAnnoReagentHash : typeAnnotationPrefix deviceReagentHashTag NEWLINE ;

deviceTypeAnno : deviceTypeAnnoStart
                 deviceTypeAnnoName
                 deviceTypeAnnoDesc?
                 deviceTypeAnnoHash?
                 deviceTypeAnnoNameHash?
                 deviceTypeAnnoLogic*
                 deviceTypeAnnoLogicSlot*
                 deviceTypeAnnoReagentHash*
                 deviceTypeAnnoEnd ;

typeAnnotation : enumTypeAnno | deviceTypeAnno ;