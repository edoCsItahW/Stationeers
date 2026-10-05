#! /user/bin/python3

#  Copyright (c) 2026. All rights reserved.
#  This source code is licensed under the CC BY-NC-SA
#  (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
#  This software is protected by copyright law. Reproduction, distribution, or use for commercial
#  purposes is prohibited without the author's permission. If you have any questions or require
#  permission, please contact the author: edocsitahw@qq.com

# -------------------------<edocsitahw>----------------------------
# file: genStdLib.py
# author: edocsitahw
# data: 2026/7/15 18:46
# desc: 从 metadata/json 生成标准库（文件头日期标志 + 枚举块 + 内置常量块 + 设备块）
#
# 输入：metadata/json/std/{enums,devices,instructions}.json
#   enums.json        = 脚本枚举（LogicType、GasType…）
#   devices.json      = 设备型号（extractDevices.py 从游戏元数据提炼）
#   instructions.json = 指令与内置常量，`type == "Constant"` 的条目生成 `define … #: @builtin`
# 输出：标准库文本。`@desc` 一律写成 ./locals/... 链接，真正（含中文）的文案在
#   metadata/json/locals/*.json 里，由插件按 ic10.language 取用。
#
# 文件头第一行是日期标志（`# stdLib.ic YYYY-MM-DD`）。assets/ic/stdLib.ic 是标准库的**唯一来源**，
# 各语言发布副本（如 publish/node/src/stdLib.ic）由发布脚本在构建时复制，并比对这一行确认
# 没有忘记同步就发布；因此重新生成后日期标志会跟着变，那是预期行为。
#
# 设备块按注解语法带出两样编译器要用的信息：
#   `@logic <名> <号> <权限>`      权限取自游戏 LogicAccessTypes（Read/Write/Read Write → r/w/rw）
#   `@logic-slot <名> <号> (<槽位>)` 适用槽位取自游戏 LogicAccessTypes 的序号列表
#   两者缺省（空串/空列表）时不写该项，语义与"未声明"一致（编译器不做该项校验）。
#
# 用法：
#   python script/genStdLib.py                        # 打印到 stdout
#   python script/genStdLib.py --out ../ic/stdLib.ic  # 直接写文件（常规做法）
#   python script/genStdLib.py --devices-only         # 只生成设备块（排查用）
# -------------------------<edocsitahw>----------------------------
import argparse
import json
import re
import sys
from datetime import date
from pathlib import Path
from typing import Any, Dict

ROOT = Path(__file__).resolve().parent.parent
JSON_DIR = ROOT / "metadata" / "json"

# Windows 控制台默认 GBK，装不下输出里的 ✓
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")


def load(name: str) -> Any:
    return json.loads((JSON_DIR / name).read_text(encoding="utf-8"))


def genHeader(day: str) -> str:
    """文件头：日期标志。

    发布脚本按 `^# stdLib.ic (\\d{4}-\\d{2}-\\d{2})$` 读这一行，用来确认发布副本与 assets
    侧是同一份；因此格式改动要同步改 code/scripts 里的 stdlib_marker / Get-StdLibMarker。
    """
    return f"# stdLib.ic {day}\n"


def genBuiltin(instructions: Dict[str, Any]) -> str:
    """内置常量块：`type == "Constant"` 的条目 → `define <名> "<名>" #: @builtin @desc …`。

    取值只是常量名（真正的数值在编译器的内置表里，见 core/src/parser/parser.cpp），
    所以这里只需要名字、签名与描述链接；名字与签名各按最长者补空格对齐，与手写块一致。
    """
    entries = sorted(
        (name, value) for name, value in instructions.items() if value.get("type") == "Constant"
    )

    if not entries:
        return ""

    name_width = max(len(name) for name, _ in entries) + 1
    quoted_width = max(len(name) + 2 for name, _ in entries) + 1
    lines = []

    for name, value in entries:
        quoted = f'"{name}"'
        lines.append(
            f"define {name.ljust(name_width)}{quoted.ljust(quoted_width)}"
            f" #: @builtin @desc {value['desc']}"
        )

    return "\n" + "\n".join(lines) + "\n"


def genEnum(data: Dict[str, Any]) -> str:
    return " ".join(
        [
            f"""
#> @enum
#> @name {name}{f'\n#> @desc {value["desc"]}' if "desc" in value else ""}
{'\n'.join([f'#> @value {subName} {subValue["value"]} {subValue["desc"] if "desc" in subValue else ""}' for subName, subValue in value["enums"].items()])}
#> @end-enum
"""
            for name, value in data.items()
        ]
    )


def ident(raw: str, fallback: str = "") -> str:
    """把游戏里的名字压成合法标识符。

    游戏的 SlotName 会带空格（`Source Plant`、`Liquid Canister`），个别 PrefabName 还带括号
    （`StructureTransformerMedium(Reversed)`），注解语法只认标识符；这类名字在 IC10 里本来也写不出来
    （代码用 HASH(...) 定位，型号哈希不受影响），压平后至少可用于 `#: @type` 提示。
    """
    name = re.sub(r"[^0-9A-Za-z_]+", "_", (raw or "").strip()).strip("_")

    if not name or name[0].isdigit():
        name = f"_{name}" if name else fallback

    return name


def unique(name: str, used: set) -> str:
    """同名会互相顶掉（一台设备可能有多个 `Entity` 槽位），重复时补序号。"""
    base = name
    serial = 1

    while name in used:
        name = f"{base}_{serial}"
        serial += 1

    used.add(name)

    return name


# 游戏的 LogicAccessTypes（LogicInsert 上）→ 注解语法的权限字面量。
# 编译器只认 r/w/rw（`wr` 等价），因此这一层映射必须完整；出现未知取值直接中断生成，
# 不要写出编译器解释不了的权限。
ACCESS_LITERALS = {"Read": "r", "Write": "w", "Read Write": "rw"}


def access_literal(raw: Any, context: str) -> str:
    """把游戏的读写声明转成权限字面量；未声明（空串/None）返回空串，表示该项不写。"""
    value = str(raw or "").strip()

    if not value:
        return ""

    if value not in ACCESS_LITERALS:
        raise ValueError(f"{context}: 未知的 LogicAccessTypes「{value}」")

    return ACCESS_LITERALS[value]


def slot_list(slots: Any) -> str:
    """把适用槽位序号写成注解的 `(0 1 2)`；空列表返回空串，表示该项不写。

    注意 `()` 在语法里是"哪个槽位都不适用"，与"不写"（跳过校验）语义不同：机器的
    LogicAccessTypes 为空时当作未声明处理，免得给合法代码报 IWA26_3。
    """
    indices = [int(index) for index in (slots or [])]

    if not indices:
        return ""

    return "(" + " ".join(str(index) for index in indices) + ")"


def genDevice(data: Dict[str, Any]) -> str:
    """每台设备一个 `#> @device` 块：名字 + 两个哈希（同值）+ 描述链接 + 逻辑属性 + 槽位逻辑属性 + 槽位。"""
    blocks = []
    used_names: set = set()
    renamed: list = []

    for name, device in data.items():
        type_name = unique(ident(name, f"Device{abs(device['hash'])}"), used_names)

        if type_name != name:
            renamed.append((name, type_name))

        lines = [
            "#> @device",
            f"#> @name {type_name}",
            f"#> @device-hash {device['hash']}",
            # 未改名的设备里，型号哈希与名字哈希是同一个值
            f"#> @name-hash {device['hash']}",
        ]

        if device.get("desc"):
            lines.append(f"#> @desc {device['desc']}")

        for logic in device.get("logics") or []:
            # 注解里的书写顺序：名字、编号、权限、默认值、描述
            parts = ["#>", "@logic", str(logic["name"]), str(logic["value"])]
            literal = access_literal(logic.get("access"), f"{type_name}.{logic['name']}")

            if literal:
                parts.append(literal)

            if logic.get("desc"):
                parts.append(logic["desc"])

            lines.append(" ".join(parts))

        for logic in device.get("logicSlots") or []:
            # 注解里的书写顺序：名字、编号、适用槽位、默认值、描述
            parts = ["#>", "@logic-slot", str(logic["name"]), str(logic["value"])]
            indices = slot_list(logic.get("slots"))

            if indices:
                parts.append(indices)

            if logic.get("desc"):
                parts.append(logic["desc"])

            lines.append(" ".join(parts))

        used_slots: set = set()

        for slot in device.get("slots") or []:
            if slot.get("index") is None:
                continue

            slot_id = unique(ident(slot.get("name", ""), f"Slot{slot['index']}"), used_slots)

            lines.append(f"#> @slot {slot_id} {slot['index']}")

        lines.append("#> @end-device")

        blocks.append("\n" + "\n".join(lines) + "\n")

    for original, replacement in renamed:
        print(f"  ! 型号名含非法字符，已改写：{original} → {replacement}", file=sys.stderr)

    return " ".join(blocks)


def main() -> None:
    global JSON_DIR

    parser = argparse.ArgumentParser(description="从 metadata/json 生成标准库")
    parser.add_argument("--out", help="写出到该文件（缺省打印到 stdout）")
    parser.add_argument("--json-dir", default=str(JSON_DIR), help="metadata/json 目录")
    parser.add_argument("--devices-only", action="store_true", help="只生成设备块，不含枚举块")
    args = parser.parse_args()

    JSON_DIR = Path(args.json_dir)

    devices = load("std/devices.json")
    day = date.today().isoformat()

    if args.devices_only:
        text = genDevice(devices)
    else:
        text = (
            genHeader(day)
            + genEnum(load("std/enums.json"))
            + genBuiltin(load("std/instructions.json"))
            + genDevice(devices)
        )

    if args.out:
        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text, encoding="utf-8")

        print(f"✓ 已写出 {out}（{len(text.splitlines())} 行，{len(devices)} 台设备，日期标志 {day}）")
    else:
        print(text)


if __name__ == "__main__":
    main()
