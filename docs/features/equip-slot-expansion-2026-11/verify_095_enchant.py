r"""⑦ 095 强化链路门禁（2026-11-12 Round 8）。

两条硬约束，任何一条破了都会在真机上以「玄学」形态炸：

1) **潜能 10 字节版本门控**：s9 的 equip 包尾有 5 个 int16 潜能行
   （V1.11.7__add_inventoryequipment_potential.sql + PacketCreator 的
   5×writeShort）。旧服务端不发这 10 字节，客户端若裸读就会把后续包
   当成潜能耗掉 → 登录/进图/开背包随机错位。ItemParser 必须先用
   available() >= 10 判定。

2) **潜能行真的被绘制**：EquipTooltip.h 的 potlines[5] 历史上只有声明，
   EquipTooltip.cpp 从未赋值、从未 draw → 「字段有了、UI 没画」的半成品。
   现在必须：set_equip 里赋值 + draw 里出现 potlines[i].draw。
"""
import re
import sys
from pathlib import Path

ITEM_PARSER = Path(r"E:\project\MapleStory-Client\Net\Handlers\Helpers\ItemParser.cpp")
TOOLTIP_CPP = Path(r"E:\project\MapleStory-Client\IO\Components\EquipTooltip.cpp")
TOOLTIP_HDR = Path(r"E:\project\MapleStory-Client\IO\Components\EquipTooltip.h")
LOCALE = Path(r"E:\project\MapleStory-Client\locale\zh-CN.ini")


def main():
    errs = []

    # ---- 1) 潜能读取必须有可用性门控 ----
    ip = ITEM_PARSER.read_text(encoding="utf-8")
    # 只截取「潜能 10B 门控块」本身：从 available() >= 10 到该 if 块结束。
    # 不能一路截到 add_equip —— Round 15 在其间插了强化段（多 2 个 read_short），
    # 旧正则会把它们算进潜能，误报 7 != 5。
    m = re.search(r"if \(recv\.available\(\) >= 10\)\s*\{(.*?)\}", ip, re.S)
    if not m:
        if re.search(r"int16_t p1 = recv\.read_short\(\)", ip):
            errs.append("ItemParser 潜能仍是裸 read_short（无 available 门控）")
        else:
            errs.append("ItemParser 里找不到潜能门控块，判据失效需重查")
    else:
        seg = m.group(1)
        n = seg.count("read_short()")
        if n != 5:
            errs.append(f"ItemParser 潜能块 read_short 次数 = {n}，应为 5")
        print(f"  [1] 潜能段已门控，read_short x{n}")

    # ---- 2) 潜能行必须被赋值 + 绘制 ----
    cpp = TOOLTIP_CPP.read_text(encoding="utf-8")
    hdr = TOOLTIP_HDR.read_text(encoding="utf-8")

    if "potlines" not in hdr:
        errs.append("EquipTooltip.h 没有 potlines 声明")
    writes = re.findall(r"potlines\[i\]\s*=\s*Text\(", cpp)
    if not writes:
        errs.append("EquipTooltip.cpp 没有给 potlines 赋值（死声明）")
    if "haspotlines[i] = true" not in cpp:
        errs.append("EquipTooltip.cpp 没有置 haspotlines —— 绘制会被跳过")
    if not re.search(r"potlines\[i\]\.draw\(", cpp):
        errs.append("EquipTooltip.cpp 没有 potlines[i].draw —— 潜能行不会显示")
    # 必须用 get_potential / get_bonus_potential 取值
    if "get_potential(" not in cpp or "get_bonus_potential(" not in cpp:
        errs.append("EquipTooltip.cpp 未调用 get_potential/get_bonus_potential")
    print(f"  [2] potlines 赋值 {len(writes)} 处、haspotlines 已置、draw 存在")

    # ---- 3) 文案通道存在（无 WZ 源，必须走 locale） ----
    loc = LOCALE.read_text(encoding="utf-8")
    n = len(re.findall(r"^equip\.potline\.\d+=", loc, re.M))
    if n == 0:
        errs.append("locale/zh-CN.ini 没有 equip.potline.* 词条（潜能行会显示成编号）")
    else:
        print(f"  [3] locale 潜能词条 {n} 条")

    # ---- 4) ⑦ 三件套：火花 / 宝珠 / 星之力（Round 15） ----
    equip_h = Path(r"E:\project\MapleStory-Client\Character\Inventory\Equip.h")
    equip_cpp = Path(r"E:\project\MapleStory-Client\Character\Inventory\Equip.cpp")
    inv_h = Path(r"E:\project\MapleStory-Client\Character\Inventory\Inventory.h")
    player = Path(r"E:\project\MapleStory-Client\Character\Player.cpp")
    srv_pc = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\util\PacketCreator.java")
    srv_eq = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\client\inventory\Equip.java")

    eh = equip_h.read_text(encoding="utf-8")
    ec = equip_cpp.read_text(encoding="utf-8")
    for getter in ("get_spark", "get_soul", "get_starforce"):
        if getter not in eh:
            errs.append(f"Equip.h 缺 {getter} 声明")
        if f"Equip::{getter}" not in ec:
            errs.append(f"Equip.cpp 缺 {getter} 实现")

    # 包解析：潜能 10B 之后必须再门控 5B
    if "available() >= 5" not in ip:
        errs.append("ItemParser 缺强化段 available() >= 5 门控（旧服务端会错位）")
    for v in ("spark_id", "soul_id", "star_force"):
        if v not in ip:
            errs.append(f"ItemParser 未解析 {v}")

    # tooltip：三行必须既赋值又绘制
    for flag, txt in (("has_spark_line", "spark_line"),
                      ("has_soul_line", "soul_line"),
                      ("has_star_line", "star_line")):
        if flag not in cpp:
            errs.append(f"EquipTooltip.cpp 缺 {flag}")
        if f"{txt}.draw(" not in cpp:
            errs.append(f"EquipTooltip.cpp 缺 {txt}.draw —— 该行不会显示")
        if f"{txt} = Text(" not in cpp:
            errs.append(f"EquipTooltip.cpp 未给 {txt} 赋值")

    # 属性注入：星之力必须进 recalc_stats 链
    pl = player.read_text(encoding="utf-8")
    if "apply_enchant_bonuses" not in pl:
        errs.append("Player.cpp 缺 apply_enchant_bonuses（星之力无加成）")
    if "get_starforce()" not in pl:
        errs.append("Player.cpp 未读 get_starforce()")

    # 服务端：Equip 字段 + PacketCreator 下发 5B
    se = srv_eq.read_text(encoding="utf-8")
    if "private short spark, soul;" not in se:
        errs.append("服务端 Equip.java 缺 spark/soul 字段")
    if "private byte starForce;" not in se:
        errs.append("服务端 Equip.java 缺 starForce 字段")
    sp = srv_pc.read_text(encoding="utf-8")
    for call in ("getSpark()", "getSoul()", "getStarForce()"):
        if call not in sp:
            errs.append(f"PacketCreator 未下发 {call}")

    # locale 词条
    ns = len(re.findall(r"^equip\.spark\.\d+=", loc, re.M))
    nl_ = len(re.findall(r"^equip\.soul\.\d+=", loc, re.M))
    if ns == 0:
        errs.append("locale 缺 equip.spark.* 词条")
    if nl_ == 0:
        errs.append("locale 缺 equip.soul.* 词条")
    print(f"  [4] 三件套齐备：火花词条 {ns} / 宝珠词条 {nl_}；星之力按星数生成")

    if errs:
        print("\n=== FAIL ===")
        for e in errs:
            print("  " + e)
        return 1
    print("\n=== PASS: 095 潜能既能读对（门控）也能显示（赋值+绘制+文案） ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
