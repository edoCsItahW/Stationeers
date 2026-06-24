/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
import {execSync} from 'child_process';

export function setupUTF8Console() {
    if (process.platform === 'win32')
        try {
            execSync('chcp 65001', {stdio: 'ignore'});
        } catch (_) {
        }

    process.stdout.setDefaultEncoding('utf8');
    process.stderr.setDefaultEncoding('utf8');
}