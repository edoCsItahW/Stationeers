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
# desc:
# -------------------------<edocsitahw>----------------------------
from json import load, dump
from warnings import warn
from typing import TypedDict, List, Dict

class SubEnumValue(TypedDict):
    desc: str
    value: int
    type: str

class EnumValue(TypedDict):
    desc: str
    enums: Dict[str, SubEnumValue]

type EnumData = Dict[str, EnumValue]

def genEnum(data: EnumData) -> str:
    return " ".join([f"""
#> @enum
#> @name {name}{f'\n#> @desc {value["desc"]}' if "desc" in value else ""}
{'\n'.join([f'#> @value {subName} {subValue["value"]} {subValue["desc"] if "desc" in subValue else ""}' for subName, subValue in value["enums"].items()])}
#> @end-enum
""" for name, value in data.items()])

def genDevice() -> str:
    return """
#> @device
#> @name TestDevice
#> @desc 测试设备
#> @end-device
    """

def parseEnum(jsonPath: str) -> None:
    with open(jsonPath, 'r', encoding="utf-8") as f:
        data = load(f)

        print(genEnum(data))


if __name__ == '__main__':
    from pathlib import Path
    jsonDir = Path(__file__).resolve().parent.parent / 'json'

    parseEnum(str(jsonDir / "std" / "enums.json"))

    print(genDevice())
