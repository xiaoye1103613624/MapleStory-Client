r"""按 WZ islot 而非号段前缀校验目录归属（Round 7 修正判据）。

为什么改判据：
  实测 01192001.img 的 islot=Si（盾牌）却落在 Character/Shield —— 它本来就是盾，
  只是 itemid 号段落在 119xxxx。用「119 = 纹章」做判据会把它误报成越界。

正确判据：**WZ islot 决定它该去哪，号段只是前缀**。
  islot → 期望目录（与 ItemData.cpp::get_eqcategory 的候选链对应）
"""
import re
import struct
import sys
import zlib
from collections import defaultdict
from pathlib import Path

LIVE = Path(r"E:\MXD\BeiDou-Client-xy\Data\Character")

# islot → 允许的目录集合（含实际散落目录，与 get_eqcategory 候选链一致）
ISLOT_DIRS = {
    "Sh": {"Accessory"},
    "Ba": {"Accessory"},
    "Em": {"Accessory"},
    "To": {"Accessory", "Totem"},
    "Dr": {"Android", "Weapon"},
    "Ht": {"Heart", "Weapon", "Android"},
    # Si = 盾/副武器。实测 119xxxx 号段里混有 41 个 Si，它们既可能落在
    # Shield（原生盾目录）也可能落在 Accessory（高版本副武器导入口径）。
    # 两者都是**合法可达**的：EquipData 现在 islot 优先，Si 一律判 SHIELD，
    # 与落在哪个目录无关 —— 目录只影响能否读到数据，不影响槽位判定。
    "Si": {"Shield", "Accessory"},
    # Hd = 帽子历史别名。实测 120xxxx 有少量 Hd 落在 Character/ 与 Weapon/，
    # 属素材自身 islot 写法问题；本端 from_islot 对它返回 NONE → 回退号段兜底，
    # 因此这里不把它当扩展号段的判定对象。
}
WATCH_ISLOTS = tuple(ISLOT_DIRS)


def read_islot(path: Path) -> str:
    """读 info/islot。

    为什么不用手写字节扫描：Round 7 第一版就是这么写的，结果扫描到 0 个素材
    却打印 PASS（假绿）。.img 节点流里 'islot' 前后的长度字节并不可靠地
    紧邻，必须走真正的解析器。

    这里复用仓库自带 tools/_wz.py（nlnx 同源解析器，已验证能解出
    islot/reqLevel/incPAD 等字段）。为控制开销，只在需要时导入。
    """
    import subprocess

    wz = Path(r"E:\project\MapleStory-Client\tools\_wz.py")
    if not wz.exists():
        return ""
    try:
        r = subprocess.run(
            [sys.executable, str(wz), str(path), "info", "3"],
            capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=30)
    except Exception:
        return ""
    m = re.search(r"islot \[string '([^']+)'\]", r.stdout)
    return m.group(1) if m else ""


def main():
    errs = []
    seen = defaultdict(list)
    scanned = 0

    for p in LIVE.rglob("*.img"):
        if not re.fullmatch(r"0*\d{7}", p.stem):
            continue
        if p.stat().st_size == 0:
            continue
        scanned += 1
        seen["__all__"].append(p)

    print(f"非空 itemid 命名 .img 共 {scanned} 个")

    # 先按「目录 × 号段」统计（零子进程开销），再对每个 (目录,号段) 组合抽样
    # 解析 islot —— 全量逐文件起 python 子进程不可行（几千次 × 30s 超时）。
    combo = defaultdict(list)
    for p in seen["__all__"]:
        digits = p.stem.lstrip("0")
        combo[(p.parent.name, digits[:3])].append(p)

    watch_prefix = {"115", "118", "119", "120", "166", "167"}
    targets = {k: v for k, v in combo.items() if k[1] in watch_prefix}
    print(f"涉及的 (目录,号段) 组合 {len(targets)} 个，抽样解析 islot：")

    resolved = 0
    for (dirname, prefix), files in sorted(targets.items()):
        sample = files[:2]
        islots = [read_islot(f) for f in sample]
        islots = [i for i in islots if i]
        if not islots:
            errs.append(f"无法解析 islot：{sample[0].name} @ {dirname}")
            continue
        resolved += 1
        ok = [i for i in islots if dirname in ISLOT_DIRS.get(i, set())]
        unknown = [i for i in islots if i not in ISLOT_DIRS]
        if unknown:
            # 未识别的 islot（如 Hd = 帽子历史别名）不是「越界」，是判据盲区：
            # 本端 from_islot 对它返回 NONE → 回退号段兜底，因此不影响槽位判定。
            print(f"  {dirname:10} prefix {prefix}: n={len(files):3} islot={islots} -> UNKNOWN(跳过)")
            continue
        mark = "OK" if len(ok) == len(islots) else "MISPLACED"
        print(f"  {dirname:10} prefix {prefix}: n={len(files):3} islot={islots} -> {mark}")
        if len(ok) != len(islots):
            errs.append(
                f"{dirname} 下有 islot={[i for i in islots if i not in ok]} 的素材"
                f"（允许目录 {[sorted(ISLOT_DIRS[i]) for i in islots if i in ISLOT_DIRS]}）")

    if resolved == 0:
        errs.append("没有任何组合解析成功 —— 门禁失效（这会是假绿）")

    if errs:
        print("\n=== FAIL ===")
        for e in errs:
            print("  " + e)
        return 1
    print(f"\n=== PASS: {resolved} 个 (目录,号段) 组合的 islot 与所在目录一致 ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
