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
                LIE1: "增量解析失败，回退至全量解析 {err}",
                LIE2: "全量解析失败 {err}"
            }
        },
        handler: {
            error: "处理器{name}遇到严重错误: {err}"
        }
    },
    hover: {
        common: {
            description: "描述"
        },
        labelDef: {
            type: "标签"
        },
        aliasDirective: {
            type: "别名"
        },
        defineDirective: {
            type: "常量"
        },
        instruction: {
            type: "指令",
            signature: "签名",
            details: "详情"
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
    },
    completion: {
        common: {
            noDesc: "暂无文档"
        },
        access: {
            r: "仅读",
            w: "仅写",
            rw: "可读/写"
        },
        direction: {
            input: "输入",
            output: "输出"
        }
    },
    formatting: {
        parseError: "配置文件 {filename} 解析失败: {err}。使用插件默认配置。"
    }
};
