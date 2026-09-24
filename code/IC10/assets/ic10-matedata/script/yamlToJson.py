#! /user/bin/python3

#  Copyright (c) 2026. All rights reserved.
#  This source code is licensed under the CC BY-NC-SA
#  (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
#  This software is protected by copyright law. Reproduction, distribution, or use for commercial
#  purposes is prohibited without the author's permission. If you have any questions or require
#  permission, please contact the author: edocsitahw@qq.com

# -------------------------<edocsitahw>----------------------------
# file: script.py
# author: edocsitahw
# data: 2026/7/15 10:25
# desc:
# -------------------------<edocsitahw>----------------------------
"""
YAML to JSON Converter
-----------------------
Convert a YAML file to a JSON file.

Usage:
    python yaml_to_json.py input.yaml [output.json]

If output file is not specified, the script will create a JSON file with the
same base name as the input (e.g., input.yaml → input.json).
"""

import sys
import json
import argparse
from pathlib import Path

# 检查并导入 PyYAML
try:
    import yaml
except ImportError:
    print("ERROR: PyYAML is required but not installed.", file=sys.stderr)
    print("Install it with: pip install PyYAML", file=sys.stderr)
    sys.exit(1)


def convert_yaml_to_json(yaml_path: Path, json_path: Path) -> None:
    """读取 YAML 文件，解析后写入 JSON 文件。"""
    try:
        with open(yaml_path, 'r', encoding='utf-8') as f:
            data = yaml.safe_load(f)
    except FileNotFoundError:
        print(f"ERROR: YAML file '{yaml_path}' not found.", file=sys.stderr)
        sys.exit(1)
    except yaml.YAMLError as e:
        print(f"ERROR: Invalid YAML syntax – {e}", file=sys.stderr)
        sys.exit(1)

    try:
        with open(json_path, 'w', encoding='utf-8') as f:
            # 使用 indent=2 使输出易读，ensure_ascii=False 保留非 ASCII 字符
            json.dump(data, f, indent=4, ensure_ascii=False)
    except IOError as e:
        print(f"ERROR: Could not write JSON file – {e}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert a YAML file to JSON format."
    )
    parser.add_argument(
        'input',
        help="Path to the input YAML file."
    )
    parser.add_argument(
        'output', nargs='?',
        help="Path to the output JSON file (optional). "
             "If omitted, the output file will have the same base name with .json extension."
    )
    args = parser.parse_args()

    input_path = Path(args.input)
    if not input_path.exists():
        print(f"ERROR: Input file '{args.input}' does not exist.", file=sys.stderr)
        sys.exit(1)

    # 若未指定输出文件，则替换扩展名为 .json
    if args.output:
        output_path = Path(args.output)
    else:
        output_path = input_path.with_suffix('.json')

    convert_yaml_to_json(input_path, output_path)
    print(f"✅ Successfully converted '{input_path}' → '{output_path}'")


if __name__ == '__main__':
    main()
