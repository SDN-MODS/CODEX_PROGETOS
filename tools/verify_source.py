#!/usr/bin/env python3
"""Static package checks, NOT an Enforce compiler or a DayZ runtime test."""
import argparse
import re
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def check(root=ROOT):
    errors = []
    mod = root / "SDN_Leaderboard"
    scripts = sorted(mod.glob("scripts/*/*.c"))
    if not scripts:
        return ["No Enforce scripts found"]
    all_code = "\n".join(p.read_text() for p in scripts)
    # Strip quoted strings and comments before balancing delimiters.
    tokens = re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*.*?\*/', re.S)
    for path in [*scripts, mod / "config.cpp", mod / "mod.cpp", mod / "gui/layouts/SDN_Leaderboard.layout"]:
        if not path.exists():
            errors.append(f"Missing {path.relative_to(root)}")
            continue
        text = tokens.sub("", path.read_text())
        stack = []
        pairs = {"}": "{", ")": "(", "]": "["}
        for char in text:
            if char in "{([":
                stack.append(char)
            elif char in pairs:
                if not stack or stack.pop() != pairs[char]:
                    errors.append(f"Unbalanced delimiters: {path.name}")
                    break
        if stack:
            errors.append(f"Unclosed delimiters: {path.name}")
        if path.suffix == ".c" and not path.name.startswith("SDN_"):
            errors.append(f"Missing SDN prefix: {path.name}")
    if (mod / "$PBOPREFIX$").read_text().strip() != "SDN_Leaderboard":
        errors.append("PBO prefix mismatch")
    config = (mod / "config.cpp").read_text()
    for resource in re.findall(r'"(SDN_Leaderboard/[^"\n]+)"', config + all_code):
        if not (root / resource).exists():
            errors.append(f"Missing resource: {resource}")
    tree = ET.parse(mod / "data/SDN_LeaderboardInputs.xml")
    if tree.getroot().tag != "modded_inputs":
        errors.append("Invalid input XML root")
    if tree.find("./inputs/actions/input[@name='UASDNLeaderboard']") is None:
        errors.append("Missing remappable input")
    if tree.find("./preset/input[@name='UASDNLeaderboard']/btn[@name='kF7']") is None:
        errors.append("Missing default F7 binding")
    layout = (mod / "gui/layouts/SDN_Leaderboard.layout").read_text()
    widget_names = re.findall(r'\b\w+WidgetClass\s+(SDN_\w+)', layout)
    if len(widget_names) != len(set(widget_names)):
        errors.append("Duplicate widget names")
    for name in re.findall(r'FindAnyWidget\("(SDN_\w+)"\)', all_code):
        if name not in widget_names:
            errors.append(f"Missing UI widget: {name}")
    lower = "\n".join(p.read_text() for p in scripts if p.parent.name != "5_Mission")
    for upper_type in ("SDN_LeaderboardServer", "SDN_LeaderboardPersistence", "SDN_LeaderboardMenu"):
        if re.search(r'\b' + upper_type + r'\b', lower):
            errors.append(f"Lower module depends on {upper_type}")
    bridge = (mod / "scripts/4_World/SDN_LeaderboardBridge.c").read_text()
    for hook in ("EEHitBy", "EEKilled", "OnRPC"):
        if f"super.{hook}(" not in bridge:
            errors.append(f"Original {hook} callback is not preserved")
    ids = re.findall(r'static const int (?:RPC_\w+|MENU_ID) = (\d+);', all_code)
    if len(ids) != 3 or len(set(ids)) != len(ids):
        errors.append("Missing/duplicate menu or RPC IDs")
    # Verify read/write order of response fields, catching protocol drift.
    server = (mod / "scripts/5_Mission/SDN_LeaderboardServer.c").read_text()
    writes = re.findall(r'rpc.Write\(page\.(\w+)\)', server)
    reads = re.findall(r'ctx.Read\(page\.(\w+)\)', bridge)
    if writes != reads:
        errors.append("Response field order differs between writer and reader")
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args()
    try:
        errors = check(args.root)
    except (OSError, ET.ParseError) as exc:
        parser.exit(1, f"Package check failed: {exc}\n")
    if errors:
        parser.exit(1, "\n".join(errors) + "\n")
    print("PASS: package paths, delimiters, XML, UI names, module boundaries, hooks and RPC field order.")
    print("Enforce compilation, native layout parsing and gameplay were NOT tested.")


if __name__ == "__main__":
    main()
