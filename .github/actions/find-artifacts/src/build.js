// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com
const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const root = path.join(__dirname, '..');
const dist = path.join(root, 'dist');
const tmp = path.join(root, '.tmp-build');
const entries = ['main', 'post'];

// 清理旧产物
fs.rmSync(dist, { recursive: true, force: true });
fs.rmSync(tmp, { recursive: true, force: true });
fs.mkdirSync(dist, { recursive: true });

try {
    for (const name of entries) {
        const outDir = path.join(tmp, name);
        execSync(`ncc build src/${name}.js -o "${outDir}"`, {
            stdio: 'inherit',
            cwd: root,
        });
        fs.renameSync(
            path.join(outDir, 'index.js'),
            path.join(dist, `${name}.js`)
        );
    }
} finally {
    fs.rmSync(tmp, { recursive: true, force: true });
}

console.log('Build done: dist/main.js, dist/post.js');
