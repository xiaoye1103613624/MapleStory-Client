r"""装备槽位 wire 编号跨端对账（2026-11-12）。

客户端：Character/Look/EquipSlot.cpp 的 wire_bodypart() switch
服务端：E:\project\BeiDou-Server_s9\...\EquipSlot.java 的 allowed[] 首值
权威表：本目录 official-slot-mapping.md §1

不依赖编译器：直接解析两份源码里的字面量，逐项比对。
"""
import re
import sys
from pathlib import Path

CLIENT = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.cpp")
CLIENT_HDR = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.h")
SERVER = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java")

# 官方期望（来自 S10 EquipSlot.java + official-slot-mapping.md）
EXPECTED = {
    "HAT": 1, "FACE": 2, "EYEACC": 3, "EARACC": 4, "TOP": 5,
    "BOTTOM": 6, "SHOES": 7, "GLOVES": 8, "CAPE": 9, "SHIELD": 10,
    "WEAPON": 11, "RING1": 12, "RING2": 13, "RING3": 15, "RING4": 16,
    "PENDANT1": 17, "TAMEDMOB": 18, "SADDLE": 19, "SHOULDER": 20,
    # POCKET 已删：-33/-133 = BP33 宠物座位，用户要求不与宠物装备冲突。
    # RING5/RING6 已删：用户 ① 只要四戒（-12/-13/-15/-16），不开 sidecar 六戒位。
    "MEDAL": 49, "BELT": 50, "PENDANT2": 51,
    "BADGE": 54,
    "TOTEM1": 55, "TOTEM2": 56, "TOTEM3": 57, "TOTEM4": 58,
    "EMBLEM": 59, "ANDROID_SLOT": 60, "HEART": 61,
}

# SUBWEAPON 与 SHIELD 是同一个 wire 位（双手武器共格），所以它**不单独**出现在
# wire_bodypart 的白名单 case 里（同值 case 会 duplicate case 编译失败）。
# 单独校验它的别名关系，而不是塞进 EXPECTED 让门禁误报。
ALIASES = {"SUBWEAPON": "SHIELD"}

# Java 枚举名 → 客户端 Id 名（同一槽位的不同命名）
# 用户 2026-11-12 约束收敛后：POCKET/AUX_WEAPON 不在表内（宠物/原始武器冲突）。
JAVA_TO_CLIENT = {
    "HAT": "HAT", "FACE_ACCESSORY": "FACE", "EYE_ACCESSORY": "EYEACC",
    "EARRINGS": "EARACC", "TOP": "TOP", "PANTS": "BOTTOM", "SHOES": "SHOES",
    "GLOVES": "GLOVES", "CAPE": "CAPE", "SHIELD": "SHIELD", "WEAPON": "WEAPON",
    "RING": "RING1", "PENDANT": "PENDANT1", "TAMED_MOB": "TAMEDMOB",
    "SADDLE": "SADDLE", "SHOULDER": "SHOULDER",
    "MEDAL": "MEDAL", "BELT": "BELT", "BADGE": "BADGE", "TOTEM": "TOTEM1",
    "EMBLEM": "EMBLEM", "ANDROID": "ANDROID_SLOT", "HEART": "HEART",
}


def parse_client(path):
    """客户端 wire 编号 = EquipSlot.h 的枚举值（不变量：枚举值 == wire BodyPart），
    再与 EquipSlot.cpp 里 wire_bodypart() 的白名单取交集。

    历史坑（2026-11-12 负对照坐实）：旧版解析 `case Id::X: return N;`，
    而回退后的实现是「白名单 + return static_cast<int16_t>(slot)」恒等写法，
    旧解析器命中 0 条 → 本脚本客户端侧全部检查空转、假绿。
    """
    hdr = CLIENT_HDR.read_text(encoding="utf-8")
    body = hdr.split("enum Id : int16_t", 1)[1].split("};", 1)[0]

    enumv = {}
    alias = {}
    for m in re.finditer(r"^\s*([A-Z0-9_]+)\s*(?:=\s*([A-Z0-9_]+|\d+))?\s*,", body, re.M):
        name, val = m.group(1), m.group(2)
        if val is None:
            continue
        if val.isdigit():
            enumv[name] = int(val)
        else:
            alias[name] = val
    for n, r in alias.items():
        if r in enumv:
            enumv[n] = enumv[r]

    src = path.read_text(encoding="utf-8")
    wl_body = src.split("int16_t wire_bodypart(Id slot)", 1)[1].split("Id from_wire_bodypart", 1)[0]
    if not re.search(r"return\s+static_cast<int16_t>\(slot\)", wl_body):
        raise RuntimeError("wire_bodypart 不是恒等实现，本脚本口径失效，需同步修改")
    wl = set(re.findall(r"case\s+Id::([A-Z0-9_]+):", wl_body))

    out = {}
    for name in wl:
        if name not in enumv:
            raise RuntimeError(f"wire 白名单 {name} 在 EquipSlot.h 无显式枚举值")
        out[name] = enumv[name]

    # 解析数为 0 一律视为门禁失效，绝不静默通过
    if not out or not enumv:
        raise RuntimeError(f"client parser 退化：enum={len(enumv)} wire={len(out)}")
    return out


def parse_server(path):
    text = path.read_text(encoding="utf-8")
    # 只取枚举常量声明段（到第一个分号 ; 结束）
    enum_body = text.split("public enum EquipSlot {", 1)[1].split(";", 1)[0]
    out = {}
    for m in re.finditer(r'(\w+)\("([^"]*)"((?:\s*,\s*-?\d+)*)\)', enum_body):
        name = m.group(1)
        nums = [int(x.strip()) for x in m.group(3).split(",") if x.strip()]
        if nums:
            out[name] = nums
    return out


def main():
    errors = []
    c = parse_client(CLIENT)
    s = parse_server(SERVER)

    print(f"client wire entries: {len(c)}")
    print(f"server enum entries: {len(s)}")

    # 1) 客户端每个官方槽位编号正确
    for name, expect in EXPECTED.items():
        got = c.get(name)
        if got != expect:
            errors.append(f"[client] {name}: expected -{expect}, got {got}")
    extra_c = set(c) - set(EXPECTED)
    if extra_c:
        errors.append(f"[client] unexpected cases: {sorted(extra_c)}")

    # 1.5) 同值别名：SUBWEAPON 必须与 SHIELD 同一个 wire 位（双手武器共格），
    #      且不落在宠物座位 BP(21..48)。
    hdr = CLIENT_HDR.read_text(encoding="utf-8")
    hbody = hdr.split("enum Id : int16_t", 1)[1].split("};", 1)[0]
    hval = {}
    for m in re.finditer(r"^\s*([A-Z0-9_]+)\s*=\s*(\d+)\s*,", hbody, re.M):
        hval[m.group(1)] = int(m.group(2))
    for m in re.finditer(r"^\s*([A-Z0-9_]+)\s*=\s*([A-Z][A-Z0-9_]*)\s*,", hbody, re.M):
        if m.group(2) in hval:
            hval[m.group(1)] = hval[m.group(2)]
    for name, target in ALIASES.items():
        if hval.get(name) != hval.get(target):
            errors.append(f"[client] alias {name}={hval.get(name)} != {target}={hval.get(target)}")
    for name, v in hval.items():
        if name in ("NONE", "BOOK", "TOP_DEFAULT", "BOTTOM_DEFAULT", "LENGTH"):
            continue
        if 21 <= v <= 48:
            errors.append(f"[client] {name} = {v} 落在 v083 宠物座位 BP(21..48)")

    # 2) 服务端每个对应枚举的首个 allowed 值与客户端一致（绝对值）
    for jname, cname in JAVA_TO_CLIENT.items():
        arr = s.get(jname)
        if not arr:
            errors.append(f"[server] missing enum {jname}")
            continue
        want_bp = EXPECTED[cname]
        # 首个 allowed 应是 -bp（或 dual-band 首值）
        if -want_bp not in arr:
            errors.append(f"[server] {jname} allowed={arr} does not contain -{want_bp} (client {cname})")

    # 3) 服务端扩展槽位（用户 7 项收敛版：无 POCKET/AUX_WEAPON）
    for jname in ("SHOULDER", "BADGE", "TOTEM", "EMBLEM", "ANDROID", "HEART"):
        if jname not in s:
            errors.append(f"[server] extension enum missing: {jname}")
    for banned in ("POCKET", "AUX_WEAPON"):
        if banned in s:
            errors.append(f"[server] banned enum present (宠物/原始武器冲突): {banned}")

    # 4) 扩展 7 格的完整编号集合（徽章/纹章/机器人/心脏 + 图腾×4 = 7 槽 8 位含镜像）
    addon = set()
    for jname, bp in (("BADGE", 54), ("EMBLEM", 59), ("ANDROID", 60), ("HEART", 61)):
        if -bp in s.get(jname, []):
            addon.add(-bp)
    for n in s.get("TOTEM", []):
        if -58 <= n <= -55:
            addon.add(n)
    if len(addon) != 8:
        errors.append(f"[server] addon seats incomplete: {sorted(addon)} (need -54..-61)")

    # 4.5) 原始武器号段 13/14 系不得出现在任何非 WEAPON 枚举的 allowed[]
    for jname, nums in s.items():
        if jname in ("WEAPON", "WEAPON_2", "LOW_WEAPON"):
            continue
        if -11 in nums:
            errors.append(f"[server] {jname} allowed 含 -11（武器槽）——不该抢武器位")
    if any(-62 in nums or -33 in nums for nums in s.values()):
        errors.append("[server] -62/-33 残留（辅助武器/口袋宠物冲突位）")

    # 4.6) 宠物防串槽：isAllowed catch-all 必须仍是精确匹配。
    # Javadoc 里引用了被禁止的写法作反面教材，必须先剥注释再判违规。
    java_src = SERVER.read_text(encoding="utf-8")
    java_src = re.sub(r"/\*.*?\*/", "", java_src, flags=re.S)
    java_src = re.sub(r"//[^\n]*", "", java_src)
    if "return cash && slot < 0" in java_src:
        errors.append("[server] catch-all 'cash && slot<0' 复活（帽子时装会穿进宠物位）")

    # 5) 四戒与第二项链（用户 ① 的右上 2 格 = -15/-16）
    ring = s.get("RING", [])
    for bp in (12, 13, 15, 16):
        if -bp not in ring:
            errors.append(f"[server] RING missing -{bp}")
    for bp in (52, 53):
        if -bp in ring:
            errors.append(f"[server] RING 仍含 -{bp}（六戒 sidecar 超出用户 ① 范围）")
    if -51 not in s.get("PENDANT", []):
        errors.append("[server] PENDANT missing -51 (second pendant)")

    print("\n=== client wire_bodypart ===")
    for k in sorted(c, key=lambda x: c[x]):
        print(f"  {k:<16} -> -{c[k]}")
    print("\n=== server allowed (first values) ===")
    for k in sorted(s):
        print(f"  {k:<16} -> {s[k]}")

    if errors:
        print("\n=== FAIL ===")
        for e in errors:
            print("  " + e)
        return 1
    print("\n=== PASS: client wire_bodypart() 与 s9 EquipSlot.java 官方编号逐项一致 ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
