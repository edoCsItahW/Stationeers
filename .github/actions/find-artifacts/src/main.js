const core = require('@actions/core');
const fg = require('fast-glob');
const path = require('path');

function parseList(input) {
    return input.split(/\r?\n|,/).map(item => item.trim()).filter(Boolean);
}

function normalizePattern(pattern) {
    let p = pattern.replace(/\\/g, '/');

    // 如果模式没有路径分隔符，并且不是以 **/ 开头，则自动递归匹配
    if (!p.includes('/') && !p.startsWith('**/')) {
        p = `**/${p}`;
    }

    return p;
}

async function main() {
    try {
        const inputs = {
            searchDir: path.relative(core.getInput('search-dir', {required: true}), to),
            patterns: parseList(core.getInput('patterns', { required: true })),
            excludeDirs: parseList(core.getInput('exclude-dirs') || "node_modules"),
            ifNoFiles: core.getInput('if-no-files-found') || "warn",
            artifactName: core.getInput('artifact-name') || ''
        }

        const ignore = inputs.excludeDirs.map(dir => {
            let p = dir.replace(/\\/g, '/').replace(/^\.\//, '').replace(/\/$/, '');

            if (!p || p === '.') return;

            return `**/${dir}/**`;
        }).filter(Boolean);

        core.info(`Searching in: ${inputs.searchDir}`);
        core.info(`Patterns: ${inputs.patterns.join(', ')}`);
        core.info(`Excluding: ${inputs.excludeDirs.join(', ')}`);

        let results = [];
        let matchedPattern = null;

        for (const rawPattern of inputs.patterns) {
            const pattern = normalizePattern(rawPattern);

            const files = await fg(pattern, {
                cwd: inputs.searchDir,
                absolute: true,
                onlyFiles: true,
                unique: true,
                followSymbolicLinks: false,
                dot: true,
                ignore
            });

            if (files.length > 0) {
                results = files;
                matchedPattern = rawPattern;
                core.info(`Pattern '${rawPattern}' matched ${files.length} file(s).`);
                break;
            }
        }

        if (results.length === 0) {
            const msg = `No files found in ${inputs.searchDir} for patterns: ${inputs.patterns.join(', ')}`;
            if (inputs.ifNoFiles === 'warn') {
                core.warning(msg);
            } else {
                core.notice(msg);
            }
        } else {
            core.info(`Found ${results.length} file(s):`);
            results.forEach((file) => core.info(`  ${file}`));
        }


    } catch (err) {
        core.setFailed(err.message);
    }
}


main()
