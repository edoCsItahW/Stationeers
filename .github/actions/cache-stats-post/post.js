const { execSync } = require('child_process');
const os = require('os');

function runCommand(cmd) {
    try {
        const output = execSync(cmd, { stdio: 'pipe', encoding: 'utf-8' });
        console.log(output);
    } catch (error) {
        console.error(`Error running "${cmd}":`, error.message);
    }
}

function main() {
    const platform = os.platform();

    // 读取由 setup-cpp 设置的环境变量
    const cacheTool = process.env.ST_CACHE_TOOL;   // 'ccache' 或 'sccache' 或 undefined
    const isWindows = platform === 'win32';

    if (!cacheTool) {
        console.log('No cache tool detected (ST_CACHE_TOOL not set). Skipping stats.');
        return;
    }

    if (isWindows) {
        if (cacheTool === 'sccache') {
            runCommand('sccache --show-stats');
        } else if (cacheTool === 'ccache') {
            runCommand('ccache --show-stats');
        }
    } else {
        // Linux / macOS
        if (cacheTool === 'ccache') {
            runCommand('ccache --show-stats');
        } else if (cacheTool === 'sccache') {
            runCommand('sccache --show-stats');
        }
    }
}

main();