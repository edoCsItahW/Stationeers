#! /user/bin/python3

#  Copyright (c) 2026. All rights reserved.
#  This source code is licensed under the CC BY-NC-SA
#  (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
#  This software is protected by copyright law. Reproduction, distribution, or use for commercial
#  purposes is prohibited without the author's permission. If you have any questions or require
#  permission, please contact the author: edocsitahw@qq.com

# -------------------------<edocsitahw>----------------------------
# file: extractDevices.py
# author: edocsitahw
# data: 2026/10/03
# desc: 从游戏原始元数据提炼设备型号数据（std 结构 + locals 文本）
#
# 输入（都来自游戏 StreamingAssets）：
#   Stationpedia.json  Language/<语言>/Stationpedia.json —— 每个 Prefab 一页
#   Enums.json         Data/Enums.json                   —— LogicType / LogicSlotType 的名字→编号与说明
#   <语言>.xml         Language/<语言>.xml                —— 解析 Title/Description 里的 <N:XX:Key> 占位符
#
# 输出：
#   metadata/json/std/devices.json    语言无关：hash、字段名、字段编号、读写、槽位、模式
#   metadata/json/locals/devices.json 语言相关：显示名与描述，按 locale 合并（可多次运行补齐多语言）
#
# 用法：
#   python script/extractDevices.py --stationpedia <Stationpedia.json> --enums <Enums.json> \
#       --xml <english.xml> --locale en-us
#   # 中文：同一命令换中文语言文件与 --locale zh-hans，locals 会自动合并
# -------------------------<edocsitahw>----------------------------
import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional

# Windows 控制台默认 GBK，装不下输出里的 ✓ 与中文
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

ROOT = Path(__file__).resolve().parent.parent
JSON_DIR = ROOT / "metadata" / "json"

# 富文本与占位符：游戏的 Stationpedia 文本里混着 <link=…>、<color=…> 与 <N:EN:Key>
TAG_RE = re.compile(r"<[^>]+>")
PLACEHOLDER_RE = re.compile(r"<([NATD]):([A-Za-z]{2}):([^>]+)>")
LINK_RE = re.compile(r"<link=([A-Za-z0-9_]+)>")

# 语言 XML 的正文里用 `{LINK:Page;可见文本}`、`{GAS:Oxygen}`、`{THING:ItemOxite}` 这类标记
BRACE_RE = re.compile(r"\{(\w+):(?:(?P<key>[^;}]+);)?(?P<text>[^}]*)\}")


def clean_text(text: Optional[str], strings: Dict[str, str]) -> str:
    """去掉游戏的富文本标签与标记、解析占位符，并把字面量 \\n 还原成换行。"""
    if not text:
        return ""

    def placeholder(match: "re.Match[str]") -> str:
        key = match.group(3)

        return strings.get(key, key)

    def brace(match: "re.Match[str]") -> str:
        key, visible = match.group("key"), match.group("text") or ""

        # `{LINK:IngotPage;矿石}` → 矿石；`{GAS:Oxygen}` → 查同一份语言表的"氧气"
        if key:
            return visible

        return strings.get(visible, visible)

    text = PLACEHOLDER_RE.sub(placeholder, text)
    text = BRACE_RE.sub(brace, text)
    text = TAG_RE.sub("", text)
    text = text.replace("\\n", "\n")

    return "\n".join(line.strip() for line in text.splitlines()).strip()


def load_strings(xml_path: Optional[Path]) -> Dict[str, str]:
    """读取语言 XML 的 Key→Value（Record 与 RecordReagent 一起收）。"""
    if not xml_path or not xml_path.exists():
        return {}

    import xml.etree.ElementTree as ElementTree

    root = ElementTree.parse(xml_path).getroot()
    strings: Dict[str, str] = {}

    for record in root.iter():
        key = record.find("Key")
        value = record.find("Value")

        if key is not None and value is not None and key.text and key.text not in strings:
            strings[key.text] = clean_text(value.text, {})

    return strings


def enum_values(enums: Dict[str, Any], name: str) -> Dict[str, Dict[str, Any]]:
    """取脚本枚举的一个枚举：成员 → {value, description, deprecated}（Enums.json 里成员在 `values` 下）。"""
    entry = (enums.get("scriptEnums") or {}).get(name) or {}

    return entry.get("values") or {}


def field_of(logic_name: str, family: str) -> Optional[str]:
    """从 LogicInsert 的 LogicName 里取字段名：优先链接目标（<link=LogicTypePressure>），否则取可见文本。"""
    match = LINK_RE.search(logic_name or "")

    if match and match.group(1).startswith(family):
        return match.group(1)[len(family):]

    text = clean_text(logic_name, {}).strip()

    return text or None


def slot_indices(access_types: str) -> List[int]:
    """LogicSlotInsert 的 LogicAccessTypes 是槽位序号列表（`0, 1, 2`）。"""
    result: List[int] = []

    for piece in (access_types or "").split(","):
        piece = piece.strip()

        if piece.isdigit():
            result.append(int(piece))

    return sorted(set(result))


def page_score(page: Dict[str, Any]) -> int:
    """同一 Prefab 可能有多页：信息最多的一页胜出。"""
    return (
        len(page.get("LogicInsert") or [])
        + len(page.get("LogicSlotInsert") or [])
        + len(page.get("SlotInserts") or [])
        + len(page.get("ModeInsert") or [])
        + (1 if page.get("Description") else 0)
    )


def collect_pages(pages: Iterable[Dict[str, Any]]) -> Dict[str, Dict[str, Any]]:
    """按 PrefabName 归并页面，保留信息最多的那份。

    Stationpedia 里除了具体设备还有分类/模板页（`AirlockDevicePage` 之类），它们的
    ModeInsert 会罗列全游戏的模式，必须排除：具体设备的 Key 一律是 `Thing<PrefabName>`。
    """
    best: Dict[str, Dict[str, Any]] = {}

    for page in pages:
        prefab = page.get("PrefabName")
        key = page.get("Key")

        if not prefab or not isinstance(prefab, str):
            continue

        if isinstance(key, str) and key and not key.startswith("Thing"):
            continue

        current = best.get(prefab)

        if current is None or page_score(page) > page_score(current):
            best[prefab] = page

    return best


def extract(
    pages: Dict[str, Dict[str, Any]],
    logics: Dict[str, Dict[str, Any]],
    slots: Dict[str, Dict[str, Any]],
    strings: Dict[str, str],
    locale: str,
    std_devices: Dict[str, Any],
    locals_devices: Dict[str, Any],
) -> Dict[str, int]:
    """提炼一台设备；std 部分语言无关、locals 部分按 locale 写入。"""
    stats = {
        "devices": 0,
        "logics": 0,
        "logicSlots": 0,
        "slots": 0,
        "modes": 0,
        "modeRanges": 0,
        "unknown": 0,
    }

    for prefab, page in pages.items():
        logic_entries: List[Dict[str, Any]] = []
        logic_slot_entries: List[Dict[str, Any]] = []
        slot_entries: List[Dict[str, Any]] = []
        mode_entries: List[Dict[str, Any]] = []
        seen_logic: set = set()
        seen_slot_logic: set = set()

        for item in page.get("LogicInsert") or []:
            name = field_of(item.get("LogicName", ""), "LogicType")

            if not name or name in seen_logic:
                continue

            seen_logic.add(name)

            member = logics.get(name)
            value = member.get("value") if member else None

            if value is None:
                stats["unknown"] += 1

                continue

            logic_entries.append({"name": name, "value": value, "access": item.get("LogicAccessTypes", "")})

        for item in page.get("LogicSlotInsert") or []:
            name = field_of(item.get("LogicName", ""), "LogicSlotType")

            if not name or name in seen_slot_logic:
                continue

            seen_slot_logic.add(name)

            member = slots.get(name)
            value = member.get("value") if member else None

            if value is None:
                stats["unknown"] += 1

                continue

            logic_slot_entries.append(
                {"name": name, "value": value, "slots": slot_indices(item.get("LogicAccessTypes", ""))}
            )

        for item in page.get("SlotInserts") or []:
            name = item.get("SlotName")

            if not name:
                continue

            index = item.get("SlotIndex")
            index = int(index) if str(index).isdigit() else None

            slot_entries.append({"name": name, "index": index})

        # ModeInsert 只留在 JSON 里，不生成注解（用户决定）。
        # 有些设备的"模式"其实是 0..N 的纯数字区间（如 ModulalDeviceMeter3x3 的 10 万个数字），
        # 那不是枚举，收进去只会把 JSON 撑到十几 MB，这里丢弃并计入 stats["modeRanges"]。
        for item in page.get("ModeInsert") or []:
            name = clean_text(item.get("LogicName", ""), strings)

            if not name:
                continue

            raw = item.get("LogicAccessTypes", "")
            value = int(raw) if str(raw).strip().lstrip("-").isdigit() else None

            mode_entries.append({"name": name, "value": value})

        if mode_entries and all(str(entry["name"]).isdigit() for entry in mode_entries):
            stats["modeRanges"] += 1
            mode_entries = []

        # 有逻辑属性 / 槽位逻辑 / 槽位 / 模式的才算 IC10 相关设备
        if not (logic_entries or logic_slot_entries or slot_entries or mode_entries):
            continue

        title = clean_text(page.get("Title", ""), strings)
        description = clean_text(page.get("Description", ""), strings)

        device: Dict[str, Any] = {"hash": page.get("PrefabHash", 0)}

        if description:
            device["desc"] = f"./locals/devices.{prefab}.desc"

        device["logics"] = [
            {
                **entry,
                # 字段说明复用枚举成员已有的双语文案（locals/enums.json），不必按设备重复存一份
                "desc": f"./locals/enums.LogicType.enums.{entry['name']}.desc",
            }
            for entry in logic_entries
        ]
        device["logicSlots"] = [
            {
                **entry,
                "desc": f"./locals/enums.LogicSlotType.enums.{entry['name']}.desc",
            }
            for entry in logic_slot_entries
        ]
        device["slots"] = slot_entries
        device["modes"] = mode_entries

        std_devices[prefab] = device

        # locals 按 locale 合并：同一设备跑多遍语言即可凑齐多语言
        existing = locals_devices.setdefault(prefab, {})

        if title:
            existing.setdefault("title", {}).update({locale: title})

        if description:
            existing.setdefault("desc", {}).update({locale: description})

        stats["devices"] += 1
        stats["logics"] += len(logic_entries)
        stats["logicSlots"] += len(logic_slot_entries)
        stats["slots"] += len(slot_entries)
        stats["modes"] += len(mode_entries)

    return stats


def load_thing_records(xml_path: Path) -> Dict[str, Dict[str, str]]:
    """读取语言 XML 的 RecordThing：prefab → {title, desc}。

    当前游戏版本已不再导出 Stationpedia.json，但 `Language/<语言>.xml` 里每个 Thing 都有
    `Key`（prefab 名）、`Value`（显示名）与 `Description`（描述），因此"只补某语言的文案"
    可以直接读它。
    """
    if not xml_path.exists():
        raise FileNotFoundError(f"语言 XML 不存在：{xml_path}")

    import xml.etree.ElementTree as ElementTree

    root = ElementTree.parse(xml_path).getroot()
    strings = load_strings(xml_path)
    records: Dict[str, Dict[str, str]] = {}

    for record in root.iter("RecordThing"):
        key = record.find("Key")

        if key is None or not key.text:
            continue

        value = record.find("Value")
        description = record.find("Description")

        records[key.text] = {
            "title": clean_text(value.text if value is not None else "", strings),
            "desc": clean_text(description.text if description is not None else "", strings),
        }

    return records


def extract_from_locale_xml(
    records: Dict[str, Dict[str, str]],
    device_keys: Iterable[str],
    locale: str,
    locals_devices: Dict[str, Any],
) -> "tuple[Dict[str, int], List[str]]":
    """只从语言 XML 取文案：给已有设备补上该语言的显示名与描述，返回统计与没找到的设备。"""
    stats = {"matched": 0, "titled": 0, "described": 0}
    missing: List[str] = []

    for prefab in device_keys:
        record = records.get(prefab)

        if not record:
            missing.append(prefab)

            continue

        stats["matched"] += 1
        existing = locals_devices.setdefault(prefab, {})

        if record["title"]:
            existing.setdefault("title", {}).update({locale: record["title"]})
            stats["titled"] += 1

        if record["desc"]:
            existing.setdefault("desc", {}).update({locale: record["desc"]})
            stats["described"] += 1

    return stats, missing


def write_json(path: Path, data: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=4, ensure_ascii=False) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description="提炼游戏设备元数据为 @ic10/metadata 的简洁 JSON")
    parser.add_argument("--stationpedia", help="游戏的 Stationpedia.json（按语言）；完整提炼时必填")
    parser.add_argument("--enums", help="游戏的 Enums.json（结构，语言无关）；完整提炼时必填")
    parser.add_argument("--xml", help="该语言的 <语言>.xml（解析 <N:XX:Key> 占位符，可缺省）")
    parser.add_argument(
        "--from-locale-xml",
        help="只从该语言 XML 的 RecordThing 取设备显示名与描述，补进已有的 std/devices.json 对应文案",
    )
    parser.add_argument("--locale", required=True, help="写进 locals 的语言键，如 en-us / zh-hans")
    parser.add_argument("--std-out", default=str(JSON_DIR / "std" / "devices.json"))
    parser.add_argument("--locals-out", default=str(JSON_DIR / "locals" / "devices.json"))
    args = parser.parse_args()

    std_path = Path(args.std_out)
    locals_path = Path(args.locals_out)

    locals_devices: Dict[str, Any] = (
        json.loads(locals_path.read_text(encoding="utf-8")) if locals_path.exists() else {}
    )

    # 只补文案：不读 Stationpedia/Enums，设备清单以已有的 std/devices.json 为准
    if args.from_locale_xml:
        if not std_path.exists():
            parser.error(f"缺少设备清单 {std_path}；先用 --stationpedia/--enums 完整提炼一次")

        records = load_thing_records(Path(args.from_locale_xml))
        std_devices = json.loads(std_path.read_text(encoding="utf-8"))
        stats, missing = extract_from_locale_xml(records, std_devices.keys(), args.locale, locals_devices)

        write_json(locals_path, dict(sorted(locals_devices.items())))

        print(
            f"✓ {args.locale}: {stats['matched']}/{len(std_devices)} 台设备在该语言里有条目"
            f"（显示名 {stats['titled']} 条，描述 {stats['described']} 条），"
            f"XML 的 RecordThing 共 {len(records)} 条"
        )
        print(f"  locals → {locals_path}")

        if missing:
            print(f"  ! 该语言 XML 里没有这 {len(missing)} 台设备：{', '.join(missing[:10])}"
                  f"{' …' if len(missing) > 10 else ''}")

        return

    if not args.stationpedia or not args.enums:
        parser.error("完整提炼需要 --stationpedia 与 --enums；只补文案请用 --from-locale-xml")

    pages_raw = json.loads(Path(args.stationpedia).read_text(encoding="utf-8"))

    # 游戏的 Stationpedia.json 是 `{version, pages: [...]}`；也容忍直接给数组
    if isinstance(pages_raw, dict):
        pages_raw = pages_raw.get("pages") or []

    pages = collect_pages(pages_raw)

    enums = json.loads(Path(args.enums).read_text(encoding="utf-8"))
    strings = load_strings(Path(args.xml) if args.xml else None)

    std_devices: Dict[str, Any] = {}

    stats = extract(
        pages,
        enum_values(enums, "LogicType"),
        enum_values(enums, "LogicSlotType"),
        strings,
        args.locale,
        std_devices,
        locals_devices,
    )

    # 稳定输出：设备按 PrefabName 排序（字段顺序保持游戏给的顺序）
    write_json(std_path, dict(sorted(std_devices.items())))
    write_json(locals_path, dict(sorted(locals_devices.items())))

    print(
        f"✓ {args.locale}: {stats['devices']} 台设备，{stats['logics']} 条逻辑属性，"
        f"{stats['logicSlots']} 条槽位逻辑属性，{stats['slots']} 个槽位，{stats['modes']} 个模式"
        f"（纯数字区间模式丢弃 {stats['modeRanges']} 台，枚举里查不到的字段跳过 {stats['unknown']} 条）"
    )
    print(f"  std    → {std_path}")
    print(f"  locals → {locals_path}")


if __name__ == "__main__":
    main()
