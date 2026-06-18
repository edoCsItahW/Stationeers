/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */

// @ts-ignore
const { setLanguage, Token, Pos, Lexer } = require('ic10-node-api');
import { execSync } from 'child_process';

function setupUTF8Console() {
    // 1. 修改控制台活动代码页（对应 C++ 的 system 调用）
    if (process.platform === 'win32')
        try {
            execSync('chcp 65001', { stdio: 'ignore' });
        } catch (_) {}

    process.stdout.setDefaultEncoding('utf8');
    process.stderr.setDefaultEncoding('utf8');
}


try {
    setupUTF8Console();

    if (typeof setLanguage === 'function')
        setLanguage("zh-hans");

    if (typeof Token === 'function') {
        const token = new Token(1, new Pos(), "a", 3);

        console.log(token.toString());
    }

    if (typeof Lexer === 'function') {
        const result: any[] = Lexer.tokenize("alias ic d0");

        result.forEach(t => console.log(t.toString()));

        const result2: any[] = (new Lexer("alias ic d0")).scan();

        result2.forEach(t => console.log(t.toString()));
    }

} catch (err) {
    console.error('Failed to load or execute native module:', err);
}
