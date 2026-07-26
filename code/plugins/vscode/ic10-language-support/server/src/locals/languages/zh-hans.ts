/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file zh-CN.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 00:04
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export default {
    server: {
        info: {
            start: "服务器正在运行..."
        },
        parser: {
            info: {
                LIE1: "增量解析失败，回退至全量解析 {}",
                LIE2: "全量解析失败 {}"
            }
        }
    },
    hover: {
        common: {
            description: '描述'
        },
        labelDef: {
            type: '标签'
        },
        aliasDirective: {
            type: '别名'
        },
        defineDirective: {
            type: '常量'
        },
        instruction: {
            type: '指令',
            signature: '签名',
            details: '详情'
        },
        operandType: {
            register: "寄存器",
            device: "设备",
            number: "数值",
            constant: "内置常量",
            error: "未知",
            batchMode: "批模式",
            logicSlotType: "逻辑槽类型",
            logicType: "逻辑类型",
            reagentMode: "试剂模式",
            slotIdx: "槽索引"
        }
    }
};