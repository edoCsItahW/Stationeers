const { execSync } = require('child_process');

function setupUTF8Console() {
    // 1. 修改控制台活动代码页（对应 C++ 的 system 调用）
    if (process.platform === 'win32') {
        try {
            execSync('chcp 65001', { stdio: 'ignore' });
        } catch (_) {}
    }

    // 2. 设置 Node.js 输出流的默认编码（处理 Node 内部 Buffer -> 字符串）
    // 这对应了 C++ 中 setlocale 或 wcout 的 UTF-8 设置
    process.stdout.setDefaultEncoding('utf8');
    process.stderr.setDefaultEncoding('utf8');
}


try {
    // 加载原生模块（路径相对于当前脚本）
    const addon = require('ic10-node-api');

    console.log(addon);

    const { setLanguage, Token, Pos, Lexer } = addon;

    setupUTF8Console();

    // 假设模块导出了一个名为 'add' 的函数
    if (typeof setLanguage === 'function')
        setLanguage("zh-hans");

    if (typeof Token === 'function') {
        const token = new Token(1, new Pos(), "a", 3);

        console.log(token.toString());
    }

    if (typeof Lexer === 'function') {
        const result = Lexer.tokenize("alias ic d0");

        result.forEach(t => console.log(t.toString()));

        const result2 = (new Lexer("alias ic d0")).scan();

        result2.forEach(t => console.log(t.toString()));
    }

} catch (err) {
    console.error('Failed to load or execute native module:', err);
}