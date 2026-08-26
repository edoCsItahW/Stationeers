#! /user/bin/python3

# -------------------------<edocsitahw>----------------------------
# file: getKeyword
# author: edocsitahw
# data: 2026/8/25 21:27
# desc:
# -------------------------<edocsitahw>----------------------------
from json import load
from typing import TypedDict, Literal


class Item(TypedDict):
    desc: str
    signature: str
    type: Literal["Instruction", "Macro", "Constant"]

# with KeyError check
SEMANTIC_MAP: dict[str, str] = {
    "str": "",
    "r?": "",
    "a(r?|num)": "",  # r代表通用寄存器
    "b(r?|num)": "",
    "c(r?|num)": "",
    "d(r?|num)": "",
    "r?|d?": "",
    "device(d?|r?|id)": "",  # r代表动态设备端口，id代表标识符
    "logicType": "",
    "min(r?|num)": "",
    "max(r?|num)": "",
    "d?": "",
    "id(r?|num)": "",  # id代表唯一标识符，源码中为uuid，游戏中通常是PrefabHash，值通常为一个数字
    "num": "",
    "address(r?|num)": "",
    "int": "",  # label或者整数或者寄存器或者标识符
    "deviceHash": "",
    "batchMode": "",
    "nameHash": "",
    "slotIndex": "",
    "logicSlotType": "",
    "reagentMode": "",
    "value(r?|num)": "",
    "reagentHash(r?|num)": "",
}

def genKeyword(data: dict[str, Item]) -> None:
    d = { k: len(v["signature"].split(" ")) for k, v in data.items() if v["type"] == "Instruction" }

    if "#" in d:
        del d["#"]

    d = sorted(d.items(), key=lambda item: item[1])
    l = max(map(lambda x: len(x[0]), d))

    m = 0

    for k, v in d:
        if not (m & (1 << v)):
            m |= (1 << v)
            print(f"// {['', '零', '一', '二', '三', '四', '五', '六'][v]}元指令")

        print(f"Keyword{k.capitalize()} {f': \'{k}\';':>{l+5}}")

def genConstant(data: dict[str, Item]) -> None:
    d = {k: len(v["signature"].split(" ")) for k, v in data.items() if v["type"] == "Constant"}

    d = sorted(d.items(), key=lambda item: item[1])
    l = max(map(lambda x: len(x[0]), d))

    print("// 常量")

    for k, v in d:
        print(f"Keyword{k.capitalize()} {f': \'{k}\';':>{l + 5}}")

def genInstruction(data: dict[str, Item]) -> None:
    d = sorted(map(lambda item: (item[0], [SEMANTIC_MAP[s] for s in item[1]["signature"].split(" ")[1:]]), filter(lambda item: item[1]["type"] == "Instruction", data.items())), key=lambda item: len(item[1]))

    for k, l in d:
        print(f"Keyword{k.capitalize()} {' '.join(l)}")

def parse(jsonPath: str) -> None:
    with open(jsonPath, 'r', encoding="utf-8") as f:
        data = load(f)

        genInstruction(data)


if __name__ == "__main__":
    from pathlib import Path
    jsonDir = Path(__file__).resolve().parent.parent / 'json'

    parse(str(jsonDir / "std" / "instructions.json"))


