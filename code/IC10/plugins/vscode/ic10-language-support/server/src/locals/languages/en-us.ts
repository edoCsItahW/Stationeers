/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file en-US.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 00:04
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export default {
    server: {
        info: {
            start: "Server is running..."
        },
        parser: {
            info: {
                LIE1: "parseInc failed, falling back to full parse {err}",
                LIE2: "full parse failed {err}"
            }
        },
        handler: {
            error: "processor {name} encountered a serious error: {err}"
        }
    },
    hover: {
        common: {
            description: "description",
            value: "value"
        },
        labelDef: {
            type: "Label"
        },
        aliasDirective: {
            type: "Alias"
        },
        defineDirective: {
            type: "Constant"
        },
        instruction: {
            type: "Instruction",
            signature: "signature",
            details: "details"
        },
        operandType: {
            register: "Register",
            device: "Device",
            number: "Number",
            constant: "Builtin Constant",
            error: "Unknown",
            batchMode: "BatchMode",
            logicSlotType: "LogicSlotType",
            logicType: "LogicType",
            reagentMode: "ReagentMode",
            slotIdx: "SlotIdx"
        }
    },
    completion: {
        common: {
            noDesc: "No docs available"
        },
        access: {
            r: "Read only",
            w: "Write only",
            rw: "Read/Write"
        },
        direction: {
            input: "Input",
            output: "Output"
        }
    },
    formatting: {
        parseError: "Configuration file {filename} parsing failed: {err}. Use the default configuration of the plugin."
    }
};
