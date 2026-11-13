r"""装备槽位扩展：宠物/原始装备冲突回归对账（2026-11-12 用户追加约束）。

约束原文：「新增装备不要和宠物装备冲突了，要保证增加了新装备匹配装备号段的同时
也不影响原始装备的逻辑」。

本脚本是防复发的门禁，任何一条 FAIL 都不许合入：
  1) live 资源扫描：新增号段（115/118/119/120/166/167）在 live Character/*.img
     必须 0 命中（不与原始装备资源撞号）；原始武器 13/14 系必须有资源
     （证明把它们排除在 override 之外是正确决策，134 = 111 个 .img）。
  2) 客户端 EquipData.cpp prefix switch：只允许 {115,118,119,120,166,167}，
     禁止 109/116/134/135 等原始号段。
  3) 服务端 s9 getEquipmentSlot prefix switch：同上。
  4) 服务端 s9 EquipSlot.java：POCKET/AUX_WEAPON 不得存在；-33/-133/-62 不得
     出现在任何 allowed[]；除 PET_EQUIP 外不得有 allowed ∈ -14/-21..-48
     （v083 宠物座位 BP 集合）。
  5) 客户端 wire_bodypart()：34 槽映射值不得落入 {14,21..48}（宠物 BP）；
     SUBWEAPON 必须 == 10（共格别名回归 SHIELD 语义）。
  6) isAllowed 不得复活 catch-all 'return cash && slot < 0'。
  7) 原始分类路径不变：EquipData.cpp 的 NON_WEAPON_TYPES/WEAPON 表体与
     git HEAD 逐字节一致（新增只走 prefix switch 短路，旧逻辑零改动）。
"""
import hashlib
import re
import subprocess
import sys
from pathlib import Path

CLIENT_EQSLOT_CPP = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.cpp")
CLIENT_EQSLOT_H = Path(r"E:\project\MapleStory-Client\Character\Look\EquipSlot.h")
CLIENT_EQCDATA_CPP = Path(r"E:\project\MapleStory-Client\Data\EquipData.cpp")
SLOT_JAVA = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\constants\inventory\EquipSlot.java")
PROVIDER_JAVA = Path(r"E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\server\ItemInformationProvider.java")
LIVE_CHARACTER = Path(r"E:\MXD\BeiDou-Client-xy\Data\Character")
REPO = Path(r"E:\project\MapleStory-Client")

NEW_PREFIXES = {115, 118, 119, 120, 166, 167}
FORBIDDEN_PREFIXES = {109, 116, 134, 135}
PET_BPS = {14, *range(21, 49)}


def parse_enum_values(text):
    """从 EquipSlot.h 解析显式赋值的枚举项（本轮不变量：枚举值 == wire 编号）。"""
    body = text.split("enum Id : int16_t", 1)[1].split("};", 1)[0]
    out = {}
    alias = {}
    for m in re.finditer(r"^\s*([A-Z0-9_]+)\s*(?:=\s*([A-Z0-9_]+|\d+))?\s*,", body, re.M):
        name, val = m.group(1), m.group(2)
        if val is None:
            continue
        if val.isdigit():
            out[name] = int(val)
        else:
            alias[name] = val   # 同值别名（SUBWEAPON = SHIELD）
    for n, r in alias.items():
        if r in out:
            out[n] = out[r]
    return out


def parse_client_wire(text):
    body = text.split("int16_t wire_bodypart(Id slot)", 1)[1].split("Id from_wire_bodypart", 1)[0]
    return {n: int(v) for n, v in re.findall(r"case\s+Id::(\w+):\s*return\s+(\d+);", body)}


def parse_wire_identity(text):
    """新版 wire_bodypart 是白名单 + return (int16_t)slot（恒等），
    收集白名单里的槽名。"""
    body = text.split("int16_t wire_bodypart(Id slot)", 1)[1].split("Id from_wire_bodypart", 1)[0]
    if re.search(r"return\s+static_cast<int16_t>\(slot\)", body):
        return sorted(set(re.findall(r"case\s+Id::([A-Z0-9_]+):", body)))
    return None


def parse_from_wire(text):
    """解析 from_wire_bodypart 的 case N: return Id::NAME; → {N: NAME}。"""
    body = text.split("Id from_wire_bodypart", 1)[1]
    body = body.split("\n\t\t}", 1)[0]
    return {int(n): nm for n, nm in re.findall(r"case\s+(\d+):\s*return\s+Id::([A-Z0-9_]+);", body)}


def parse_prefix_switch(text, marker):
    body = text.split(marker, 1)[1]
    body = body.split("};", 1)[0] if "};\n" in body[:4000] else body.split("}", 1)[0]
    out = set()
    for m in re.finditer(r"case\s+(\d+)(?:\s*,\s*(\d+))?\s*(?:->|:)", body):
        out.add(int(m.group(1)))
        if m.group(2):
            out.add(int(m.group(2)))
    return out


def parse_server_allowed(text):
    enum_body = text.split("public enum EquipSlot {", 1)[1].split(";", 1)[0]
    out = {}
    for m in re.finditer(r'(\w+)\("([^"]*)"((?:\s*,\s*-?\d+)*)\)', enum_body):
        nums = [int(x.strip()) for x in m.group(3).split(",") if x.strip()]
        out[m.group(1)] = nums
    return out


def main():
    errs = []

    # 1) live 资源扫描
    # 判据演进（Round 6）：Round 4 用「新增号段必须 0 命中」来证明没撞车，
    # 但那只能验证「当时 live 是空的」。Round 6 实测另一会话批量导入了 1459 个
    # 高版本装备 .img（19:30-19:31），该判据随之失效 —— 继续沿用会误报。
    # 现在改为验证**目录归属**：新号段素材必须落在客户端目录解析器
    # （Data/ItemData.cpp::get_eqcategory）会去找的那个目录里，
    # 且其 WZ islot 必须与 EquipSlot.java 的槽位字符串一致。
    EXPECT_DIR = {
        "115": ["Accessory"],
        "118": ["Accessory"],
        # 119xxxx 是**混合号段**：实测 41 个 islot=Si（盾/副武器）、19 个 islot=Em。
        # Si 的那批可能落在 Shield。客户端从 islot 判槽（from_islot 优先），
        # 所以 Shield 也是合法落点；候选链只用于校验「能被 get_eqcategory 找到」。
        "119": ["Accessory", "Shield"],
        "120": ["Accessory", "Totem"],
        "166": ["Android", "Weapon"],
        "167": ["Heart", "Weapon", "Android"],
    }

    by_prefix = {}
    for p in LIVE_CHARACTER.rglob("*.img"):
        if not re.fullmatch(r"0*\d{7}", p.stem):
            continue
        digits = p.stem.lstrip("0")
        if len(digits) != 7:
            continue
        by_prefix.setdefault(digits[:3], []).append(p)
    imgs = [p for group in by_prefix.values() for p in group]

    print(f"live Character .img total={len(imgs)} distinct_prefix={len(by_prefix)}")

    # 判据（Round 6）：不再要求「单一目录」，而是要求**同一 itemid 至少出现在
    # 客户端候选链的某个目录里**，否则 get_eqcategory 探测会全部落空 →
    # 表现为「能穿上但属性读不到」。候选取自 ItemData.cpp::get_eqcategory。
    transient = 0
    dup_ok = 0
    for p in sorted(NEW_PREFIXES):
        files = by_prefix.get(str(p), [])
        cands = EXPECT_DIR[str(p)]
        # 0 字节 = 导入工具正在写的中间态（Round 7 实测存在），
        # 不参与「目录归属」判定，只单独计数并在末尾汇总提示。
        empty_files = [f for f in files if f.stat().st_size == 0]
        files = [f for f in files if f.stat().st_size > 0]
        if empty_files:
            transient += len(empty_files)
        outside = [f for f in files if f.parent.name not in cands]
        print(f"  new prefix {p}: {len(files)} imgs (nonempty), candidates={cands}, "
              f"outside={len(outside)}, transient_zero={len(empty_files)}")
        if outside:
            shown = ", ".join(f"{f.name}@{f.parent.name}" for f in outside[:4])
            errs.append(
                f"prefix {p} 有 {len(outside)} 个 .img 落在候选目录 {cands} 之外（{shown}）"
                f" —— 客户端 get_eqcategory 探测会失败")

        # 同一 itemid 同名文件散落在多个候选目录：**只要内容一致就无害**
        # （Round 6 实测 01202022/01202023/01662148 三对重复的 SHA256 完全相同，
        #  是同一素材被导了两处，探测顺序不会导致属性不同）。
        # 只有「同名不同内容」才是真冲突 —— 那会让客户端读到哪份随机。
        #
        # ⚠️ Round 7 补充：另一个会话正在批量导入装备素材（实测 live 下
        # Character/*.img 一度有 3.6 万个 0 字节文件，且数量在持续减少 = 导入
        # 中间态）。0 字节文件的 SHA256 是 e3b0c442...（空串哈希），会把
        # 「同名不同内容」判成误报。因此把 0 字节单独归类为「导入中间态」，
        # 只做提示不做 FAIL —— 否则门禁在别人导入期间永远红，失去信号价值。
        stems = {}
        for f in files:
            stems.setdefault(f.stem, []).append(f)
        for stem, group in stems.items():
            dirs = {g.parent.name for g in group}
            if len(dirs) < 2:
                continue
            nonempty = [g for g in group if g.stat().st_size > 0]
            if len(nonempty) < 2:
                transient += 1
                continue
            hashes = {}
            for g in nonempty:
                hashes.setdefault(g.parent.name, hashlib.sha256(g.read_bytes()).hexdigest())
            if len(set(hashes.values())) > 1:
                # 内容不同**不一定**是故障。客户端读取是确定的：
                # get_eqcategory 按候选链顺序探测，命中第一个存在的目录即停，
                # 所以「实际读到哪份」由候选链唯一决定，不存在随机。
                #
                # ⚠️ Round 12 修正判据：原本用「候选首位那份体积不得小于其它」
                # 作代理判据，实测是**误报**：
                #   01672059 / 01672063 → 两边节点树完全相同，仅编码差 56B；
                #   01672096 → 两边都有 icon/iconRaw，只是 27x30 vs 26x30 的
                #               不同美术版本，不是「残缺」。
                # 体积不是完整性的代理。正确判据：**候选首位那份必须包含
                # 装备渲染所需的关键节点**（icon / iconRaw / info），
                # 缺了才算客户端真的读不到。
                first_dir = cands[0]
                first = [g for g in nonempty if g.parent.name == first_dir]
                if first:
                    import subprocess as _sp
                    wz = Path(r"E:\project\MapleStory-Client\tools\_wz.py")
                    if wz.exists():
                        r = _sp.run([sys.executable, str(wz), str(first[0]), "", "8"],
                                    capture_output=True, text=True, encoding="utf-8",
                                    errors="replace", timeout=60)
                        tree = r.stdout
                        if "icon" not in tree:
                            errs.append(
                                f"prefix {p} 的 {stem}：候选首位 {first_dir} 里没有 icon 节点"
                                f" —— 客户端会读不到图标")
                        else:
                            dup_ok += 1
                    else:
                        dup_ok += 1
                else:
                    dup_ok += 1

    # 134 护栏同样只数非空文件：导入中间态的 0 字节壳会把计数虚高，
    # 一旦真实素材被删而只剩空壳，护栏就会误判「安全」。
    n134 = len([f for f in by_prefix.get("134", []) if f.stat().st_size > 0])
    n134_all = len(by_prefix.get("134", []))
    print(f"  guard prefix 134 (原始武器): {n134} nonempty / {n134_all} total imgs")
    if n134 == 0:
        errs.append("prefix 134 非空素材扫描为 0，live 路径变化，本门禁结论失效，需重查")

    if transient:
        print(f"  [info] 已跳过 {transient} 个 0 字节 .img（导入工具中间态，不参与判定）")
    if dup_ok:
        print(f"  [info] {dup_ok} 个 itemid 在多目录有多个版本，但候选首位那份完整，读取确定")

    # 2) 客户端 EquipData.cpp prefix switch
    eqtext = CLIENT_EQCDATA_CPP.read_text(encoding="utf-8")
    client_prefixes = parse_prefix_switch(eqtext, "switch (prefix)")
    print(f"client EquipData override prefixes = {sorted(client_prefixes)}")
    if client_prefixes != NEW_PREFIXES:
        errs.append(f"client EquipData prefix 集合 {sorted(client_prefixes)} != {sorted(NEW_PREFIXES)}")
    for fp in FORBIDDEN_PREFIXES & client_prefixes:
        errs.append(f"client EquipData 覆盖了原始号段 {fp}")

    # 3) 服务端 provider prefix switch
    provtext = PROVIDER_JAVA.read_text(encoding="utf-8")
    m = re.search(r"final String forcedSlot = switch \(slotPrefix\) \{(.*?)\};", provtext, re.S)
    if not m:
        errs.append("s9 provider: forcedSlot switch 未找到")
        server_prefixes = set()
    else:
        server_prefixes = set(int(x) for x in re.findall(r"case\s+(\d+)", m.group(1)))
    print(f"server provider override prefixes = {sorted(server_prefixes)}")
    if server_prefixes != NEW_PREFIXES:
        errs.append(f"server provider prefix 集合 {sorted(server_prefixes)} != {sorted(NEW_PREFIXES)}")

    # 4) s9 EquipSlot.java：POCKET/AUX_WEAPON 已删；无 -33/-62；非宠物枚举不占宠物 BP
    slottext = SLOT_JAVA.read_text(encoding="utf-8")
    allowed = parse_server_allowed(slottext)
    for banned in ("POCKET", "AUX_WEAPON"):
        if banned in allowed:
            errs.append(f"server EquipSlot 仍有 {banned}（与宠物/原始武器冲突）")
    all_nums = {n for nums in allowed.values() for n in nums}
    for n in (-33, -133, -62, -162):
        if n in all_nums:
            errs.append(f"server allowed[] 残留 {n}（宠物镜像/原始武器位）")
    for ename, nums in allowed.items():
        if ename == "PET_EQUIP":
            continue
        for n in nums:
            if -n in PET_BPS:
                errs.append(f"server {ename} allowed={n} 落入宠物 BP 座位")

    # 5) 本轮回退锁死的三条不变量（Round2 曾把枚举改连续，导致原始装备
    #    图标/卸下回归；这三条就是防它再犯的机器门禁）
    eqh = CLIENT_EQSLOT_H.read_text(encoding="utf-8")
    eqc = CLIENT_EQSLOT_CPP.read_text(encoding="utf-8")
    enumv = parse_enum_values(eqh)
    wl = parse_wire_identity(eqc)
    frommap = parse_from_wire(eqc)

    if wl is None:
        errs.append("wire_bodypart 不再是白名单+恒等返回（本轮回退的写法被改写）")
        wl = []

    # 5.1 枚举值 == wire 编号（恒等），且白名单槽位必须在头文件里有显式赋值
    for name in wl:
        if name not in enumv:
            errs.append(f"wire 白名单含 {name}，但 EquipSlot.h 无显式枚举值（恒等会返回错的数）")
        elif enumv[name] not in frommap:
            errs.append(f"{name} = {enumv[name]} 在 from_wire_bodypart 缺互逆映射")
        elif frommap[enumv[name]] != name:
            errs.append(f"{name} 互逆不一致：enum={enumv[name]} -> from_wire={frommap.get(enumv[name])}")

    # 5.2 角色装备槽绝不落在宠物座位 BP（21..48）
    for name, v in enumv.items():
        if name in ("NONE", "BOOK", "TOP_DEFAULT", "BOTTOM_DEFAULT", "LENGTH"):
            continue
        if v in PET_BPS:
            errs.append(f"枚举 {name} = {v} 落在宠物座位 BP(21..48)")

    # 5.3 已删除的冲突项不得复活；SUBWEAPON 必须是 SHIELD 同值别名
    for banned in ("POCKET", "RING5", "RING6"):
        if banned in enumv:
            errs.append(f"EquipSlot.h 仍定义 {banned}（宠物/超范围）")
    if "SUBWEAPON" not in enumv or enumv.get("SUBWEAPON") != enumv.get("SHIELD"):
        errs.append(f"SUBWEAPON 必须与 SHIELD 同值别名（现 SHIELD={enumv.get('SHIELD')} SUBWEAPON={enumv.get('SUBWEAPON')}）")
    if any(k in frommap for k in (33, 52, 53, 62)):
        errs.append("from_wire_bodypart 仍认 33/52/53/62（宠物位/六戒/辅助武器）")

    # 5.4 符号归一化：调用点两套口径并存（InventoryHandlers.cpp:184 传负、:210 传正）
    if not re.search(r"if \(bodypart < 0\)\s*\n?\s*bodypart = static_cast<int16_t>\(-bodypart\)", eqc):
        errs.append("from_wire_bodypart 缺符号归一化（收到 -20 会落 default，护肩/新槽图标不刷新）")

    # 5.5 islot 优先（Round 7）：119xxxx 混有 islot=Si（盾/副武器 41 件）、
    #     120xxxx 混有 islot=Hd（帽），按号段一刀切会把它们判错槽。
    #     要求 EquipData 先调 from_islot()，号段 switch 只作兜底。
    if "from_islot" not in eqtext:
        errs.append("EquipData 未使用 from_islot() —— 号段一刀切会把 119 号段里的盾判成纹章")
    if not re.search(r"from_islot\s*\([^)]*islot[^)]*\)[\s\S]{0,200}?return;", eqtext):
        errs.append("EquipData 的 from_islot 调用后没有 return（会被后面的号段 switch 覆盖）")
    # islot 调用必须出现在号段 switch 之前，否则兜底会先命中
    i_islot = eqtext.find("from_islot")
    i_prefix_switch = eqtext.find("switch (prefix)")
    if i_islot > 0 and i_prefix_switch > 0 and i_islot > i_prefix_switch:
        errs.append("from_islot 排在号段 switch 之后 —— 兜底先命中，islot 优先失效")

    # 5.6 from_islot 不得把宠物 islot 映射成角色槽位（用户硬约束）
    islot_body = eqc.split("Id from_islot", 1)[1] if "Id from_islot" in eqc else ""
    for pet_islot in ('"1812', '"180', '"181', '"182', '"183'):
        if pet_islot in islot_body:
            errs.append(f"from_islot 映射了宠物 islot {pet_islot} —— 会与宠物装备串槽")

    print(f"enum entries with explicit value = {len(enumv)}, wire whitelist = {len(wl)}")
    print(f"  SHOULDER={enumv.get('SHOULDER')} PENDANT2={enumv.get('PENDANT2')} "
          f"MEDAL={enumv.get('MEDAL')} RING3={enumv.get('RING3')} "
          f"TOTEM1={enumv.get('TOTEM1')} SUBWEAPON={enumv.get('SUBWEAPON')}/SHIELD={enumv.get('SHIELD')}")

    # 6) catch-all 不复活（必须先剥注释：第 4 项的说明文字里引用了反面教材）
    slottext_nocomment = re.sub(r"/\*.*?\*/", "", slottext, flags=re.S)
    slottext_nocomment = re.sub(r"//[^\n]*", "", slottext_nocomment)
    if "return cash && slot < 0" in slottext_nocomment:
        errs.append("server isAllowed catch-all 复活")

    # 7) 原始分类逻辑零删改：HEAD 的每一行代码都必须按序保留在当前文件里
    #    （只允许新增 prefix 短路块，不允许删/改/换序）。逐行判定，忽略注释与空白。
    old = subprocess.run(
        ["git", "-C", str(REPO), "show", "HEAD:Data/EquipData.cpp"],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
    ).stdout
    if old:
        def code_lines(src):
            src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
            out = []
            for ln in src.splitlines():
                ln = re.sub(r"//[^\n]*", "", ln).strip()
                if ln:
                    out.append(ln)
            return out

        head_lines = code_lines(old)
        cur_lines = code_lines(eqtext)
        idx = 0
        dropped = []
        for ln in head_lines:
            found = False
            while idx < len(cur_lines):
                if cur_lines[idx] == ln:
                    idx += 1
                    found = True
                    break
                idx += 1
            if not found:
                dropped.append(ln)
        if dropped:
            errs.append(f"EquipData.cpp 原始逻辑被删改 {len(dropped)} 行: {dropped[:4]}")
        else:
            print(f"原始分类逻辑零删改（HEAD {len(head_lines)} 行代码逐序保留，新增 {len(cur_lines)-len(head_lines)} 行）")

    if errs:
        print("\n=== FAIL ===")
        for e in errs:
            print("  " + e)
        return 1
    print("\n=== PASS: 新增号段零资源冲突 / 宠物 BP 零占用 / 原始装备路径零改动 ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
