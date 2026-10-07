/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file deviceTypes.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 21:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { DeviceTypeInfo, Optional } from "@ic10/common";
import type { TypeTableMap } from "@ic10/compiler";
import { Linker } from "@ic10/compiler";
import { devices } from "@ic10/metadata/locals";

import { locale } from "../../locals";
import { AST } from "../../utils";
import { STAND_LIB } from "./parserPipline";


/**
 * @if zh
 * @summary 元数据里一台设备的本地化文案
 *
 * @else
 * @summary Localized text of one device in the metadata
 *
 * @endif
 * */
interface LocalizedDevice {
    title?: Record<string, string>;
    desc?: Record<string, string>;
}

/**
 * @if zh
 * @summary 第二显示名所用的语言
 *
 * @details 英语是游戏的原始语言，也是元数据里覆盖最全的一门（577/577，中文只有 476），所以
 *          "本地化名 + 英文名"里的第二名固定取英语：非英语界面下它给用户一个稳定的参照，
 *          英语界面下两者自然相同、只展示一次，将来增加语言也不会让候选越堆越杂。
 *
 * @else
 * @summary The language used for the second display name
 *
 * @details English is the game's original language and the best-covered one in the metadata (577/577
 *          against 476 for Chinese), so the second half of "localized name + English name" is always
 *          English: off-English locales get a stable reference, under an English locale the two names
 *          coincide and are shown once, and adding languages later cannot clutter the candidates.
 *
 * @endif
 * */
const ENGLISH_LANGUAGE = "en-us";

/**
 * @if zh
 * @summary 元数据的 `devices` 表
 *
 * @details 生成器对超过阈值的大文件只给"键名联合 + `unknown`"（见 `script/build.mjs` 的
 *          `INLINE_AS_CONST_LIMIT`），因此这里收窄成按型号名索引的可用形状。
 *
 * @else
 * @summary The metadata `devices` table
 *
 * @details For files above the threshold the generator only emits a key union with `unknown` values
 *          (see `INLINE_AS_CONST_LIMIT` in `script/build.mjs`), so it is narrowed here into a usable
 *          model-name index.
 *
 * @endif
 * */
const LOCALIZED_DEVICES = devices as unknown as Record<string, Optional<LocalizedDevice>>;

/**
 * @if zh
 * @summary 索引项（本地化之前的部分）
 *
 * @else
 * @summary An index entry (before localization)
 *
 * @endif
 * */
interface DeviceTypeEntry {
    typeName: string;
    hash: Optional<number>;
    localized: Optional<LocalizedDevice>;
}

let cache: Optional<DeviceTypeEntry[]>;

let localizedCache: Optional<Map<string, LocalizedDevice>>;

/**
 * @if zh
 * @brief 把型号名压成标识符
 *
 * @details 与 `script/genStdLib.py` 的 `ident()` 是同一套规则：非字母数字下划线压成 `_`、
 *          去掉首尾 `_`、数字开头补 `_`。标准库里的类型名正是这么来的，所以把元数据的型号名
 *          压平之后就能与类型名对上（`StructureTransformerMedium(Reversed)` →
 *          `StructureTransformerMedium_Reversed`）。个别设备因为压平后重名被生成器补了序号
 *          （如 `…_Reversed_1`），这类设备在元数据里找不到条目，只有类型名可用。
 *
 * @param modelName 游戏里的型号名
 * @return 压平后的标识符
 *
 * @else
 * @brief Collapse a model name into an identifier
 *
 * @details Same rule as `ident()` in `script/genStdLib.py`: collapse non-alphanumerics into `_`, drop
 *          leading/trailing `_`, prefix `_` when it starts with a digit. Standard-library type names
 *          are produced this way, so a collapsed model name lines up with a type name
 *          (`StructureTransformerMedium(Reversed)` → `StructureTransformerMedium_Reversed`). A few
 *          devices got a serial suffix after collapsing (e.g. `…_Reversed_1`); those have no metadata
 *          entry and only their type name is available.
 *
 * @param modelName Model name as used by the game
 * @return The collapsed identifier
 *
 * @endif
 * */
function collapse(modelName: string): string {
    const collapsed = modelName
        .trim()
        .replace(/[^0-9A-Za-z_]+/g, "_")
        .replace(/^_+|_+$/g, "");

    return /^[0-9]/.test(collapsed) ? `_${collapsed}` : collapsed;
}

/**
 * @if zh
 * @brief 按压平后的型号名索引本地化文案（进程内只建一次）
 *
 * @else
 * @brief Index the localized text by collapsed model name (built once per process)
 *
 * @endif
 * */
function localizedIndex(): Map<string, LocalizedDevice> {
    if (localizedCache) return localizedCache;

    const index = new Map<string, LocalizedDevice>();

    for (const [modelName, text] of Object.entries(LOCALIZED_DEVICES))
        if (text) index.set(collapse(modelName), text);

    localizedCache = index;

    return index;
}

/**
 * @if zh
 * @brief 取某台设备的两个显示名：当前语言的本地化名与英文名
 *
 * @details 型号名压平后与类型名同形，因此直接按类型名查即可。本地化名缺失（该设备没有条目，
 *          或该语言没有译文）时退回 `undefined`，由调用方决定显示类型名还是英文名；
 *          英文名用来做"本地化 + 英文"的第二显示名，界面语言是英语时两者相同。
 *
 * @param typeName 类型名（标准库里的 `#> @name`）
 * @param language 当前界面语言
 * @return `title` 为当前语言的显示名、`englishTitle` 为英文显示名，均可能为 `undefined`
 *
 * @else
 * @brief Get both display names of one device: the localized one and the English one
 *
 * @details A collapsed model name has the same shape as a type name, so a direct lookup by type name
 *          works. A missing localized name (no entry for the device, or no translation for that
 *          language) stays `undefined` so callers can fall back to the type name or the English name;
 *          the English name is the second half of "localized + English" and equals the first one under
 *          an English locale.
 *
 * @param typeName Type name (the `#> @name` of the standard library)
 * @param language Current interface language
 * @return `title` is the display name in that language and `englishTitle` the English one; either may
 *         be `undefined`
 *
 * @endif
 * */
export function getDeviceTitles(
    typeName: string,
    language: string
): { title: Optional<string>; englishTitle: Optional<string> } {
    const title = localizedIndex().get(typeName)?.title;

    return { title: title?.[language], englishTitle: title?.[ENGLISH_LANGUAGE] };
}

/**
 * @if zh
 * @brief 建立设备类型索引（首次调用时链接标准库，之后复用）
 *
 * @details 类型名与型号哈希来自标准库：把 `stdLib.ic` 单独喂给链接器，取类型表里的
 *          `DeviceAnnotation` 节点即可，不必等某个文档被解析。索引与语言无关，因此只建一次；
 *          本地化在 `getDeviceTypes()` 里按当前语言取。
 *
 * @return 索引项列表
 *
 * @else
 * @brief Build the device type index (links the standard library on first use, then reuses it)
 *
 * @details Type names and model hashes come from the standard library: feed `stdLib.ic` to the linker
 *          alone and read the `DeviceAnnotation` nodes of the type table — no parsed document is
 *          needed. The index is language independent, so it is built once; `getDeviceTypes()`
 *          localizes it for the current language.
 *
 * @return The index entries
 *
 * @endif
 * */
function buildIndex(): DeviceTypeEntry[] {
    if (cache) return cache;

    const linker = new Linker();

    linker.addUnit(STAND_LIB);
    linker.link();

    const types = JSON.parse(linker.typeTable.toJSON()) as TypeTableMap;
    const localized = localizedIndex();

    cache = Object.values(types)
        .filter(AST.isDeviceAnnotation)
        .map(device => ({
            typeName: device.name,
            hash: device.deviceHash ? Number(device.deviceHash.value) : undefined,
            localized: localized.get(device.name)
        }))
        .sort((a, b) => a.typeName.localeCompare(b.typeName));

    return cache;
}

/**
 * @if zh
 * @brief 取设备类型索引（已按当前语言本地化）
 *
 * @details 排序用当前语言的显示名（缺失时退回类型名），让"搜索设备类型"的初始列表就是
 *          用户能读的顺序。
 *
 * @return 索引项列表，`title` / `englishTitle` / `desc` 为当前语言的文案与英文名
 *
 * @else
 * @brief Get the device type index, localized for the current language
 *
 * @details Entries are sorted by the display name of the current language (falling back to the type
 *          name), so the initial list of "search device type" reads naturally.
 *
 * @return The entries, with `title` / `englishTitle` / `desc` for the current language
 *
 * @endif
 * */
export function getDeviceTypes(): DeviceTypeInfo[] {
    const language = locale.getLocale();

    return buildIndex()
        .map(({ typeName, hash, localized }) => ({
            typeName,
            hash,
            title: localized?.title?.[language],
            englishTitle: localized?.title?.[ENGLISH_LANGUAGE],
            desc: localized?.desc?.[language]
        }))
        .sort((a, b) => (a.title ?? a.typeName).localeCompare(b.title ?? b.typeName, language));
}

/**
 * @if zh
 * @summary 一个可以写进 `HASH("…")` 的型号名及其显示名
 *
 * @else
 * @summary A model name writable inside `HASH("…")` with its display names
 *
 * @endif
 * */
export interface DeviceModelName {
    /** @if zh @brief 原始型号名（含括号等，`HASH("…")` 里要写的就是它） @else @brief Raw model name (parentheses included; this is what `HASH("…")` takes) @endif */
    modelName: string;

    /** @if zh @brief 当前语言下的显示名 @else @brief Display name in the current language @endif */
    title: Optional<string>;

    /** @if zh @brief 英文显示名 @else @brief English display name @endif */
    englishTitle: Optional<string>;
}

/**
 * @if zh
 * @brief 取全部设备型号名（`HASH("…")` 的候选）
 *
 * @details 与 `getDeviceTypes()` 的区别在于**名字取哪一种**：类型名是型号名压平后的标识符
 *          （`StructureTransformerMedium(Reversed)` → `StructureTransformerMedium_Reversed`），
 *          写进 `#: @type` 的是它；而 `HASH("…")` 要的是游戏里的**原始型号名**（带括号），
 *          两者对大多数设备相同、对个别设备不同。因此这里直接用元数据的键（原始型号名），
 *          不经过压平，也不与类型表求交集。
 *
 * @param language 当前界面语言
 * @return 型号名候选，按当前语言的显示名排序
 *
 * @else
 * @brief Get every device model name (the candidates of `HASH("…")`)
 *
 * @details The difference from `getDeviceTypes()` is **which name**: a type name is the collapsed
 *          identifier of a model name (`StructureTransformerMedium(Reversed)` →
 *          `StructureTransformerMedium_Reversed`) and is what `#: @type` takes, whereas `HASH("…")`
 *          wants the game's **raw model name** (parentheses included); the two coincide for most
 *          devices and differ for a few. This therefore uses the metadata keys (raw model names)
 *          directly, without collapsing and without intersecting the type table.
 *
 * @param language Current interface language
 * @return Model name candidates sorted by the display name of the current language
 *
 * @endif
 * */
export function getDeviceModelNames(language: string): DeviceModelName[] {
    return Object.entries(LOCALIZED_DEVICES)
        .map(([modelName, localized]) => ({
            modelName,
            title: localized?.title?.[language],
            englishTitle: localized?.title?.[ENGLISH_LANGUAGE]
        }))
        .sort((a, b) => (a.title ?? a.modelName).localeCompare(b.title ?? b.modelName, language));
}
