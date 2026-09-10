grammar IC10;

import nullary;
import unary;
import binary;
import ternary;
import quaternary;
import quinary;
import senary;

program : statement (NEWLINE+ statement)* NEWLINE* EOF;

statement : labelDef
          | preprocessorDirective
          | executableInstruction
          ;

labelDef : identifier COLON ;

preprocessorDirective : aliasDirective
                      | defineDirective
                      ;

executableInstruction : nullaryInstruction
                      | unaryInstruction
                      | binaryInstruction
                      | ternaryInstruction
                      | quaternaryInstruction
                      | quinaryInstruction
                      | senaryInstruction
                      ;

aliasDirective : KeywordAlias identifier regOrDev ;

defineDirective : KeywordDefine identifier constNum ;
