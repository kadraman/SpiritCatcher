#!/usr/bin/env python3
"""
Generate a C initializer for a sin-like table used by SpriteSpirit.

Usage examples:
  python3 scripts/generate_spirit_table.py
  python3 scripts/generate_spirit_table.py --steps 128 --amp 111
  python3 scripts/generate_spirit_table.py --inplace include/SpriteSpirit.h

If --inplace is given the script will backup the target file to
`<file>.bak` and replace the `sin_table` initializer in-place.
"""
from __future__ import annotations
import argparse
import math
import time
import shutil
import re
from pathlib import Path


def gen_table(steps: int, amp: int) -> list[int]:
    vals = []
    for i in range(steps):
        # Use a sine wave; shift and scale to signed range
        v = math.sin(2.0 * math.pi * i / steps) * amp
        vals.append(int(round(v)))
    return vals


def format_c_array(name: str, vals: list[int], per_line: int = 8) -> str:
    lines = []
    for i in range(0, len(vals), per_line):
        chunk = vals[i:i+per_line]
        lines.append("    " + ", ".join(f"{v:+d}" if v < 0 else f" {v:d}" for v in chunk))
    body = ",\n".join(lines)
    return f"const INT8 {name}[] = {{\n{body}\n}};\n"


def inplace_patch(header_path: Path, name: str, array_text: str) -> None:
    if not header_path.exists():
        raise FileNotFoundError(header_path)
    bak = header_path.with_suffix(header_path.suffix + ".bak")
    # create a backup (overwrite if exists)
    shutil.copy2(header_path, bak)
    txt = header_path.read_text()
    # pattern to replace the whole declaration (const INT8 name ... { ... };)
    pattern = re.compile(r"const\s+INT8\s+" + re.escape(name) + r"\s*\[.*?\]\s*=\s*\{.*?\};", re.S)
    if not pattern.search(txt):
        raise RuntimeError(f"Could not find declaration for '{name}' in {header_path}")
    new_txt = pattern.sub(array_text.strip(), txt)
    header_path.write_text(new_txt)
    print(f"Backed up {header_path} -> {bak}")
    print(f"Patched {header_path}")


def main() -> None:
    p = argparse.ArgumentParser(description="Generate sin_table for SpriteSpirit")
    p.add_argument("--steps", "-s", type=int, default=128, help="number of entries (default 128)")
    p.add_argument("--amp", "-a", type=int, default=111, help="amplitude (max absolute value)")
    p.add_argument("--name", "-n", default="sin_table", help="C symbol name")
    p.add_argument("--out", "-o", type=Path, help="write output to file instead of stdout")
    p.add_argument("--inplace", "-i", type=Path, help="patch a header in-place (creates .bak)")
    args = p.parse_args()

    vals = gen_table(args.steps, args.amp)
    array_text = format_c_array(args.name, vals)

    if args.inplace:
        inplace_patch(args.inplace, args.name, array_text)
    elif args.out:
        args.out.write_text(array_text)
        print(f"Wrote {args.out}")
    else:
        print(array_text)


if __name__ == '__main__':
    main()
