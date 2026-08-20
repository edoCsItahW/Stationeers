const core = require('@actions/core');
const fs = require('fs/promises');
const path = require('path');
const os = require('os');
const { DefaultArtifactClient } = require('@actions/artifact');

async function run() {
    try {
        // 读取 main 阶段保存的状态
        const filesJson = core.getState('files');
        const artifactName = core.getState('artifactName');
        const searchDir = core.getState('searchDir');
        const retentionDaysRaw = core.getState('retentionDays') || '0';

        if (!filesJson) {
            core.info('No files state found. Skipping post upload.');
            return;
        }

        const files = JSON.parse(filesJson);

        if (!artifactName || files.length === 0) {
            core.info('Artifact name is empty or no files found. Skipping upload.');
            return;
        }

        const retentionDays = parseInt(retentionDaysRaw, 10);

        // 创建临时 staging 目录
        const staging = path.join(
            os.tmpdir(),
            `upload-${Date.now()}-${Math.random().toString(36).slice(2)}`
        );
        await fs.mkdir(staging, { recursive: true });

        const uploadFiles = [];
        const usedDests = new Set();

        for (const file of files) {
            const abs = path.resolve(file);

            // 计算相对于搜索目录的路径，用于在 artifact 中保留目录结构
            let rel = path.relative(searchDir, abs);

            // 如果文件在搜索目录之外，或相对路径不安全，则退化为 basename
            if (!rel || rel.startsWith('..') || path.isAbsolute(rel)) {
                rel = path.basename(abs);
            }

            // 处理可能的文件名冲突
            let destRel = rel;
            if (usedDests.has(destRel)) {
                const ext = path.extname(destRel);
                const base = path.basename(destRel, ext);
                const dir = path.dirname(destRel);
                destRel = path.join(dir, `${base}-${Math.random().toString(36).slice(2, 8)}${ext}`);
            }
            usedDests.add(destRel);

            const dest = path.join(staging, destRel);
            await fs.mkdir(path.dirname(dest), { recursive: true });
            await fs.copyFile(abs, dest);

            uploadFiles.push(destRel);
        }

        core.info(`Uploading ${uploadFiles.length} file(s) as artifact '${artifactName}'...`);

        const artifactClient = new DefaultArtifactClient();
        const options = {};
        if (retentionDays > 0) {
            options.retentionDays = retentionDays;
        }

        const result = await artifactClient.uploadArtifact(
            artifactName,
            uploadFiles,
            staging,
            options
        );

        core.info(`Artifact '${artifactName}' uploaded successfully.`);

        // 清理临时目录
        await fs.rm(staging, { recursive: true, force: true });
    } catch (error) {
        // post 阶段可以选择 warning 或 setFailed，这里用 warning 避免影响已完成的 job
        core.warning(`Post artifact upload failed: ${error.message}`);
    }
}

run();