// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file zh-hans.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 18:00
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export default {
    completion: {
        scope: {
            /** 状态栏提示：{scope} 为下面某个类别的名称 */
            message: "IC10 补全范围：{scope}",
            device: "设备",
            register: "寄存器",
            number: "数字与常量",
            enum: "枚举成员",
            identifier: "标识符与标签",
            keyword: "指令关键字",
            all: "全部"
        }
    },
    deviceSearch: {
        /** 快速选择面板标题 */
        title: "搜索设备类型",
        /** 快速选择面板占位提示 */
        placeholder: "输入设备名称（中英文皆可）以查找类型名",
        /** 没有任何匹配时的占位提示 */
        empty: "没有匹配的设备类型",
        /** 插入后的状态栏提示：{name} 为类型名 */
        inserted: "已插入设备类型：{name}"
    }
};
