// script/build.mjs
import { readFile, writeFile, mkdir, rm, readdir } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import path from 'node:path';


const ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const GEN  = path.join(ROOT, '.generated');

const MAPPINGS = [
    { from: path.join(ROOT, 'metadata/json'),         to: ''             },
    { from: path.join(ROOT, 'metadata/stationpedia'), to: 'stationpedia' },
];

/** 判定阈值：压缩后 JSON 超过这个就切到大文件模式 */
const INLINE_AS_CONST_LIMIT = 40_000;

function toIdent(name) {
    const camel = name.replace(/[-_.](\w)/g, (_, c) => c.toUpperCase());
    return /^[A-Za-z_$]/.test(camel) ? camel : `_${camel}`;
}

function isPlainObject(v) {
    return v !== null && typeof v === 'object' && !Array.isArray(v);
}

async function readJsonFile(p) {
    const raw = (await readFile(p, 'utf8')).replace(/^\uFEFF/, '');
    try {
        return { raw, json: JSON.parse(raw) };
    } catch (e) {
        throw new Error(`Failed to parse JSON: ${p}\n${e.message}`);
    }
}

function assertNoProto(obj, p = '') {
    if (obj === null || typeof obj !== 'object') return;
    for (const k of Object.keys(obj)) {
        if (k === '__proto__') throw new Error(`__proto__ key at ${p}`);
        assertNoProto(obj[k], `${p}.${k}`);
    }
}

/**
 * 生成一个 TS 模块。
 * - 小文件：as const，保留字面量类型
 * - 大对象：显式类型注解，避免 TS7056
 * - 大数组 / 其它：退化为 unknown
 */
function generateTsSource(json, relForComment) {
    const header = `// AUTO-GENERATED from ${relForComment}. DO NOT EDIT.\n`;
    const jsonStr = JSON.stringify(json, null, 2);
    const compactLen = JSON.stringify(json).length;

    if (compactLen <= INLINE_AS_CONST_LIMIT) {
        return (
            header +
            `const data = ${jsonStr} as const;\n` +
            `export default data;\n`
        );
    }

    if (isPlainObject(json)) {
        const keyUnion = Object.keys(json).map(k => JSON.stringify(k)).join(' | ');
        return (
            header +
            `export type Data = Record<${keyUnion}, unknown>;\n` +
            `const data: Data = ${jsonStr};\n` +
            `export default data;\n`
        );
    }

    return (
        header +
        `const data: unknown = ${jsonStr};\n` +
        `export default data;\n`
    );
}

async function walkJson(dir) {
    const out = [];
    for (const e of await readdir(dir, { withFileTypes: true })) {
        const p = path.join(dir, e.name);
        if (e.isDirectory())      out.push(...await walkJson(p));
        else if (e.name.endsWith('.json')) out.push(p);
    }
    return out;
}

await rm(GEN, { recursive: true, force: true });

const dirExports = new Map();

for (const { from, to } of MAPPINGS) {
    for (const file of await walkJson(from)) {
        const rel    = path.relative(from, file);
        const relDir = path.dirname(rel) === '.' ? '' : path.dirname(rel);
        const name   = path.basename(rel, '.json');

        const outDir = path.join(GEN, to, relDir);
        await mkdir(outDir, { recursive: true });

        const { json } = await readJsonFile(file);
        assertNoProto(json, path.relative(ROOT, file));

        const relForComment = path.relative(ROOT, file).replaceAll('\\', '/');
        const code = generateTsSource(json, relForComment);
        await writeFile(path.join(outDir, `${name}.ts`), code);

        if (!dirExports.has(outDir)) dirExports.set(outDir, []);
        dirExports.get(outDir).push(name);
    }
}

for (const [outDir, names] of dirExports) {
    await mkdir(outDir, { recursive: true });
    const lines = names.map(n =>
        `export { default as ${toIdent(n)} } from './${n}.js';`
    );
    await writeFile(path.join(outDir, 'index.ts'), lines.join('\n') + '\n');
}

console.log('✓ generated TS sources in .generated/');