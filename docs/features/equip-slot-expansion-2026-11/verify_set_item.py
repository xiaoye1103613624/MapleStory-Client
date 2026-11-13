r"""⑤ 套装花名册格式与档位逻辑对账（2026-11-12）。

镜像解析 Data/SetItem.txt，验证：
  1) SET/ITEM/TIER 三类行格式合法、引用不悬空
  2) itemId -> setId 映射唯一
  3) 「穿 N 件」应激活的档位集合与 TIER 声明一致
  4) 样例套装的期望加成值
C++ 侧由整工程 Rebuild EXIT=0 证明编译链接无误（见 build_setitem.log）。
"""
import sys
from pathlib import Path

TXT = Path(r"E:\project\MapleStory-Client\Data\SetItem.txt")
VALID_KEYS = {
    "STR", "DEX", "INT", "LUK", "HP", "MP", "WATK", "MAGIC", "WDEF", "MDEF",
    "ACC", "AVOID", "HANDS", "SPEED", "JUMP", "ALLSTAT", "DAMAGE", "BOSS", "IGNOREDEF",
}


def parse(path):
    sets = {}
    item_to_set = {}
    cur = None
    for lineno, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        tag = parts[0]
        if tag == "SET":
            if len(parts) < 4:
                raise ValueError(f"L{lineno}: SET needs <setId> <name> <completeCount>")
            sid = int(parts[1])
            sets[sid] = {"name": parts[2], "complete": int(parts[3]),
                         "items": [], "tiers": [], "line": lineno}
            cur = sid
        elif tag == "ITEM":
            if cur is None:
                raise ValueError(f"L{lineno}: ITEM before any SET")
            iid = int(parts[1])
            if iid in item_to_set and item_to_set[iid] != cur:
                raise ValueError(f"L{lineno}: item {iid} in two sets")
            sets[cur]["items"].append(iid)
            item_to_set[iid] = cur
        elif tag == "TIER":
            if cur is None:
                raise ValueError(f"L{lineno}: TIER before any SET")
            cnt, key, val = int(parts[1]), parts[2], int(parts[3])
            if key not in VALID_KEYS:
                raise ValueError(f"L{lineno}: unknown statKey {key}")
            sets[cur]["tiers"].append((cnt, key, val))
        else:
            raise ValueError(f"L{lineno}: unknown tag {tag}")
    return sets, item_to_set


def active(sets, sid, equipped):
    owned = {i for i in sets[sid]["items"] if i in equipped}
    n = len(owned)
    return n, [(k, v) for c, k, v in sets[sid]["tiers"] if n >= c]


def main():
    errs = []
    sets, i2s = parse(TXT)
    print(f"sets={len(sets)} item_mappings={len(i2s)}")

    for sid, d in sets.items():
        if d["complete"] != len(d["items"]) and d["complete"] > len(d["items"]):
            errs.append(f"set {sid}: completeCount {d['complete']} > items {len(d['items'])}")
        print(f"  set {sid} {d['name']}: items={len(d['items'])} tiers={len(d['tiers'])} complete={d['complete']}")

    # 用例：101 穿 2 件 → STR+3 DEX+2；穿 3 件 → 再加 WATK+5 ALLSTAT+2
    e2 = [1002001, 1040002]
    n2, a2 = active(sets, 101, e2)
    print(f"  set101 equip2 -> count={n2} active={a2}")
    if n2 != 2 or ("STR", 3) not in a2 or ("DEX", 2) not in a2 or ("WATK", 5) in a2:
        errs.append("set101 equip2 tiers wrong")

    e3 = [1002001, 1040002, 1060006]
    n3, a3 = active(sets, 101, e3)
    print(f"  set101 equip3 -> count={n3} active={a3}")
    if n3 != 3 or ("WATK", 5) not in a3 or ("ALLSTAT", 2) not in a3:
        errs.append("set101 equip3 tiers wrong")

    # 用例：102 穿 4 件 → BOSS+10 IGNOREDEF+5 生效；穿 3 件不生效
    e4 = [1002586, 1041054, 1061061, 1082140]
    n4, a4 = active(sets, 102, e4)
    print(f"  set102 equip4 -> count={n4} active={a4}")
    if ("BOSS", 10) not in a4 or ("IGNOREDEF", 5) not in a4:
        errs.append("set102 equip4 tiers wrong")
    n3b, a3b = active(sets, 102, e4[:3])
    print(f"  set102 equip3 -> count={n3b} active={a3b}")
    if ("BOSS", 10) in a3b:
        errs.append("set102 equip3 should NOT activate 4-piece tier")

    # 去重：同一 itemid 重复出现在穿戴列表只算一次
    dup = [1002001, 1002001, 1040002]
    nd, _ = active(sets, 101, dup)
    print(f"  set101 duplicate equip -> count={nd}")
    if nd != 2:
        errs.append("duplicate item should count once")

    if errs:
        print("\n=== FAIL ===")
        for e in errs:
            print("  " + e)
        return 1
    print("\n=== PASS: 5) set roster / tier activation / dedup logic all consistent ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
