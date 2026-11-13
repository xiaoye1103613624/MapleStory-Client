r"""④ 辅助装备子窗一致性对账（2026-11-12）。

核对三份字面量：
  1) UIAddonEquip.h 的 SLOTS（8 个 EquipSlot::Id）
  2) UIAddonEquip.h 的 CELLS（v5 manifest 的位图坐标）
  3) EquipSlot.cpp 的 wire_bodypart()（官方 BodyPart）
  4) 服务端 s9 EquipSlot.java 的 allowed[]
  5) live UIWindow.img/AddonEquip/backgrnd 节点（由 MCP 已验证，此处核文件尺寸）
"""
import re
import sys
from pathlib import Path

HDR = Path(r"E:\project\MapleStory-Client\IO\UITypes\UIAddonEquip.h")
CPP = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.cpp")
EQUIPSLOT_H = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.h")
SRV = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java")
PNG = Path(r"E:\project\MapleStory-Client\docs\features\addon-equipment\ui\window\v5\normal.png")
LIVE = Path(r"E:\MXD\BeiDou-Client-xy\Data\UI\UIWindow.img")

# 用户文案顺序：上排图腾×4；下排 纹章/机器人/新增(心脏)/徽章
EXPECTED = [
    ("TOTEM1", 55, (11, 32), "To"),
    ("TOTEM2", 56, (44, 32), "To"),
    ("TOTEM3", 57, (77, 32), "To"),
    ("TOTEM4", 58, (110, 32), "To"),
    ("EMBLEM", 59, (11, 65), "Em"),
    ("ANDROID_SLOT", 60, (44, 65), "Dr"),
    ("HEART", 61, (77, 65), "Ht"),
    ("BADGE", 54, (110, 65), "Ba"),
]


def parse_slots_cells(text):
    sb = text.split("SLOTS = {", 1)[1].split("};", 1)[0]
    slots = re.findall(r"EquipSlot::Id::(\w+)", sb)
    cb = text.split("CELLS = {", 1)[1].split("};", 1)[0]
    cells = [(int(a), int(b)) for a, b in re.findall(r"Point<int16_t>\((\d+),\s*(\d+)\)", cb)]
    return slots, cells


def parse_wire(text):
    """2026-11-12 回退后 wire_bodypart() 是「白名单 + return (int16_t)slot」恒等
    实现，编号来自 EquipSlot.h 的枚举值（不变量：枚举值 == wire BodyPart）。
    旧版解析 `case Id::X: return N;` 已命中 0 条 —— 那会让本脚本静默假绿。
    """
    hdr = EQUIPSLOT_H.read_text(encoding="utf-8")
    hbody = hdr.split("enum Id : int16_t", 1)[1].split("};", 1)[0]
    hval = {}
    for m in re.finditer(r"^\s*([A-Z0-9_]+)\s*=\s*(\d+)\s*,", hbody, re.M):
        hval[m.group(1)] = int(m.group(2))

    body = text.split("int16_t wire_bodypart(Id slot)", 1)[1].split("Id from_wire_bodypart", 1)[0]
    if not re.search(r"return\s+static_cast<int16_t>\(slot\)", body):
        raise RuntimeError("wire_bodypart 非恒等实现，本脚本口径失效")
    wl = set(re.findall(r"case\s+Id::([A-Z0-9_]+):", body))

    out = {n: hval[n] for n in wl if n in hval}
    if not out:
        raise RuntimeError("client wire parser 退化（0 条）")
    return out


def parse_server(text):
    enum_body = text.split("public enum EquipSlot {", 1)[1].split(";", 1)[0]
    out = {}
    for m in re.finditer(r'(\w+)\("([^"]*)"((?:\s*,\s*-?\d+)*)\)', enum_body):
        nums = [int(x.strip()) for x in m.group(3).split(",") if x.strip()]
        if nums:
            out[m.group(1)] = (m.group(2), nums)
    return out


def main():
    errs = []
    h = HDR.read_text(encoding="utf-8")
    slots, cells = parse_slots_cells(h)
    wire = parse_wire(CPP.read_text(encoding="utf-8"))
    srv = parse_server(SRV.read_text(encoding="utf-8"))

    print(f"SLOTS={len(slots)} CELLS={len(cells)} wire_entries={len(wire)}")

    if len(slots) != 8 or len(cells) != 8:
        errs.append(f"expected 8 slots/cells, got {len(slots)}/{len(cells)}")

    for i, (name, bp, cell, islot) in enumerate(EXPECTED):
        got_slot = slots[i] if i < len(slots) else None
        got_cell = cells[i] if i < len(cells) else None
        if got_slot != name:
            errs.append(f"[{i}] slot expected {name}, got {got_slot}")
        if got_cell != cell:
            errs.append(f"[{i}] cell expected {cell}, got {got_cell}")
        w = wire.get(name)
        if w != bp:
            errs.append(f"[{i}] wire {name} expected {bp}, got {w}")
        # 服务端：islot 对应的枚举 allowed[] 含 -bp（TOTEM 是 4 格合并枚举）
        found = False
        for ename, (wz, nums) in srv.items():
            if wz == islot and -bp in nums:
                found = True
                break
        if not found:
            errs.append(f"[{i}] server no islot '{islot}' with -{bp}")
        print(f"  [{i}] {name:<14} cell={cell} wire=-{bp} islot={islot} server=OK" if found
              else f"  [{i}] {name:<14} cell={cell} wire=-{bp} islot={islot} server=MISSING")

    # v5 底板尺寸与 live 资源
    if PNG.exists():
        import struct
        b = PNG.read_bytes()[:24]
        w, hh = struct.unpack(">II", b[16:24])
        print(f"v5 PNG = {w}x{hh}")
        if (w, hh) != (152, 105):
            errs.append(f"v5 PNG expected 152x105, got {w}x{hh}")
    else:
        errs.append("v5 PNG missing")

    print(f"live UIWindow.img size = {LIVE.stat().st_size if LIVE.exists() else 'MISSING'}")

    if errs:
        print("\n=== FAIL ===")
        for e in errs:
            print("  " + e)
        return 1
    print("\n=== PASS: 4) 8 slots / bitmap cells / wire bodypart / server islot all aligned ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
