r"""Round 4 关键风险验证：删除 s9 `return cash && slot < 0` catch-all 后，
合法穿戴会不会被误拒（尤其现金/时装装备）。

逐字复刻改后 EquipSlot.java 的 isAllowed() 语义，穷举：
  A) v083 BP 表（护肩槽\槽位基址.txt）里所有 ACTIVE 槽位的 正常位/cash镜像位
  B) 本端 7 项新增槽位（-20/-51/-54..-61）的正常位与 cash 镜像位
  C) 对照组：确实应该被拒的畸形组合（跨部位 cash、宠物座位）
断言：A/B 全放行，C 全拒绝。
"""
import re
import sys
from pathlib import Path

JAVA = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java")
BPDOC = Path(r"E:\MXD\扩展改动\护肩槽\槽位基址.txt")

PET_BPS = {14, *range(21, 49)}


def parse_java(text):
    enum_body = text.split("public enum EquipSlot {", 1)[1].split(";", 1)[0]
    out = {}
    for m in re.finditer(r'(\w+)\("([^"]*)"((?:\s*,\s*-?\d+)*)\)', enum_body):
        nums = [int(x.strip()) for x in m.group(3).split(",") if x.strip()]
        if nums:
            out[m.group(2)] = nums          # islot -> allowed[]
    return out


def is_allowed(allowed, slot, cash):
    """Java isAllowed 的精确复刻（非 PET_EQUIP 分支）。"""
    if slot < 0 and allowed is not None:
        for allow in allowed:
            if slot == allow:
                return True
            if cash and allow > -100 and slot == allow - 100:
                return True
        return False
    return False


def main():
    java = parse_java(JAVA.read_text(encoding="utf-8"))
    print(f"parsed islot groups = {sorted(java)}")
    fails = []

    # ---------- A) v083 ACTIVE 槽位：正常 + cash 镜像都必须放行 ----------
    # BP 表列序：| BP | Index | Entry Address | X | Y | Normal Slot | Cash Slot | Main Hit-Test | Native Category Mapping |
    #            c[0]  c[1]        c[2]        c[3] c[4]     c[5]         c[6]         c[7]                c[8]
    bp_rows = []
    for line in BPDOC.read_text(encoding="utf-8").splitlines():
        c = [x.strip() for x in line.strip().strip("|").split("|")]
        if len(c) >= 9 and re.fullmatch(r"\d+", c[0]) and c[7] == "ACTIVE":
            bp_rows.append(c)
    print(f"BP doc ACTIVE rows = {len(bp_rows)}")

    # 号段 → islot（与 s9 getEquipmentSlot / ExtendedEquipRegistry 同口径）
    CAT_ISLOT = {
        100: "Cp", 101: "Af", 102: "Ay", 103: "Ae", 104: "Ma", 105: "Ma",
        106: "Pn", 107: "So", 108: "GlGw", 109: "Si", 110: "Sr", 111: "Ri",
        112: "Pe", 113: "Be", 114: "Me", 115: "Sh", 118: "Ba", 119: "Em",
        120: "To", 166: "Dr", 167: "Ht", 190: "Tm", 191: "Sd",
        # BP 表第 20 行的 Native Category Mapping 是 192（v083 原生装饰槽，
        # 原文 "vanilla; often repurposed as shoulder"）。本项目按参考文档
        # E:\MXD\扩展改动\护肩槽\README.txt 的 SHOULDER("Sh", -20) 复用它，
        # 所以该行的 cash -120 归 Sh 校验，而不是静默跳过。
        192: "Sh",
    }

    checked_a = 0
    for row in bp_rows:
        normal = int(row[5])
        cashslot = int(row[6])
        mapping = row[8]
        if abs(normal) in PET_BPS:          # BP14/21..48 = 宠物座位，走 PET_EQUIP 分支
            continue
        m = re.search(r"`?(\d{3})`?", mapping)
        if not m:                            # 武器行（"category tens 13/14/16/17"）走 Wp
            nums = java.get("Wp")
        else:
            nums = java.get(CAT_ISLOT.get(int(m.group(1)), ""))
        if nums is None:
            fails.append(f"[A] 无法解析号段 islot：row={row[0]} mapping={mapping!r}")
            continue
        if not is_allowed(nums, normal, False):
            fails.append(f"[A] 正常位 {normal}（{mapping} via {nums}）被拒")
        if not is_allowed(nums, cashslot, True):
            fails.append(f"[A] cash 镜像 {cashslot}（{mapping}）被拒 —— 时装会穿不上")
        checked_a += 1
    if checked_a < 15:
        fails.append(f"[A] 只校验到 {checked_a} 组，BP 表解析退化")
    print(f"[A] v083 槽位校验 {checked_a} 组：正常位 + cash 镜像位（按号段查指定 islot）")

    # ---------- B) 本轮新增槽位：正常位与 cash 位都要放行 ----------
    NEW = {
        "Sh": [-20], "Pe2": [-51], "Ba": [-54], "To": [-55, -56, -57, -58],
        "Em": [-59], "Dr": [-60], "Ht": [-61],
    }
    for label, slots in NEW.items():
        key = {"Pe2": "Pe"}.get(label, label)
        nums = java.get(key)
        if nums is None:
            fails.append(f"[B] islot '{key}' 在服务端不存在")
            continue
        for s in slots:
            if not is_allowed(nums, s, False):
                fails.append(f"[B] {key} 正常位 {s} 被拒")
            # cash：sidecar 位可走正常位（S10 口径）或 -bp-100 镜像，至少一条要通
            mirror_ok = is_allowed(nums, s, True) or is_allowed(nums, s - 100, True)
            if not mirror_ok:
                fails.append(f"[B] {key} cash 位 {s} / {s-100} 双双被拒")
    print("[B] 新增槽位（护肩/第二项链/徽章/图腾×4/纹章/机器人/心脏）校验完成")

    # ---------- C) 必须被拒的畸形组合（catch-all 原来会放行它们） ----------
    BAD = [
        ("Cp", -101 - 4, True, "时装帽穿到 cash 上衣位"),
        ("Si", -149, True, "盾牌时装穿到 cash 勋章位"),
        ("Ri", -121, True, "戒指穿到宠物 BP21 的 cash 位"),
        ("Ri", -122, True, "戒指穿到宠物 BP22 的 cash 位"),
        ("Ba", -33, False, "徽章穿到口袋/宠物位 -33"),
        ("Ba", -133, True, "徽章穿到 2 号宠拾取袋 -133"),
        ("Aw", -62, False, "辅助武器位 -62（本端不采纳）"),
    ]
    for islot, slot, cash, why in BAD:
        nums = java.get(islot)
        if nums is None:
            continue
        if is_allowed(nums, slot, cash):
            fails.append(f"[C] 应被拒却放行：{islot} slot={slot} cash={cash} ({why})")
    print(f"[C] 畸形组合拒绝检查 {len(BAD)} 组")

    # ---------- D) 与旧 catch-all 对比：被收紧的到底是什么 ----------
    def old_is_allowed(allowed, slot, cash):
        if slot < 0 and allowed:
            for allow in allowed:
                cond = allow - 100 if cash else allow
                if slot == cond:
                    return True
        return cash and slot < 0

    tightened = []
    for islot, nums in java.items():
        for slot in list(range(-1, -70, -1)) + list(range(-101, -170, -1)):
            o = old_is_allowed(nums, slot, True)
            n = is_allowed(nums, slot, True)
            if o and not n:
                tightened.append((islot, slot))
    # cash 镜像口径（s9 EquipSlot.java:120 逐字）：cash && allow > -100 && slot == allow - 100
    # 即允许位 -5 的 cash 位是 -105（不是 -95）；-95 只能由允许位 +5 推出，恒假。
    def cash_mirror(islot, slot):
        return any(a > -100 and slot == a - 100 for a in java[islot])

    legit = [t for t in tightened if cash_mirror(t[0], t[1])]
    print(f"[D] 删除 catch-all 后新增拒绝的 (islot, cash slot) 组合 = {len(tightened)}")
    print(f"    其中属于「合法 cash 镜像位」(slot == allow-100) 的 = {len(legit)}")
    if legit:
        for islot, slot in legit[:10]:
            print(f"      误拒：{islot} cash {slot}（allowed={java[islot]}）")
        fails.append(f"[D] catch-all 删除误拒 {len(legit)} 个合法 cash 镜像位")

    # 宠物位必须仍在拒绝集合内（这是删 catch-all 的主要收益）
    pet_cash = [s for s in range(-170, -100) if (-s) - 100 in PET_BPS]
    if len(pet_cash) < 20:
        fails.append(f"[D] 宠物 cash 位集合退化 = {len(pet_cash)}（应约 28 个：-121/-122..-148）")
    pet_leak = [(i, s) for i in java for s in pet_cash if is_allowed(java[i], s, True)]
    if pet_leak:
        fails.append(f"[D] 宠物座位 cash 位泄漏 {len(pet_leak)} 组: {pet_leak[:6]}")
    print(f"    宠物座位 cash 位（bp∈21..48 → -121..-148）共 {len(pet_cash)} 个，"
          f"全部 28 个 islot 均被拒 = {len(java) * len(pet_cash) - len(pet_leak)} 组")

    # 负对照：确认 tightened 不是空集算出来的 0（区间为空 = 门禁失效）
    if len(tightened) == 0:
        fails.append("[D] tightened 为空 —— 遍历区间可能退化，本项未真正检查")

    if fails:
        print("\n=== FAIL ===")
        for f in fails:
            print("  " + f)
        return 1
    print("\n=== PASS: 删 catch-all 未误拒任何合法正常位/cash 镜像位；畸形组合全部被拒 ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
