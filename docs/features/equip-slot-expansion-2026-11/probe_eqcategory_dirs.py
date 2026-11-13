r"""探测高版本号段素材的实际落盘目录分布（决定 get_eqcategory 的正确写法）。

Round 6 实测：第三方批量导入的素材按 itemid 分散落在多个目录，
（120 在 Accessory 与 Totem、166 在 Android 与 Weapon、167 在 Heart/Android/Weapon），
所以「一个前缀=一个目录」的假设不成立。

策略：客户端 get_eqcategory 只能给出**唯一**目录名，因此必须
  1) 统计每个前缀的真实目录分布；
  2) 找出「主流目录」与散落项；
  3) 若散落项确实存在，客户端需要「多目录回退」而不是单一映射。
"""
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

LIVE = Path(r"E:\MXD\BeiDou-Client-xy\Data\Character")
PREFIXES = ("115", "116", "118", "119", "120", "166", "167")


def main():
    dist = defaultdict(Counter)
    examples = defaultdict(lambda: defaultdict(list))
    for p in LIVE.rglob("*.img"):
        if not re.fullmatch(r"0*\d{7}", p.stem):
            continue
        digits = p.stem.lstrip("0")
        if len(digits) != 7:
            continue
        prefix = digits[:3]
        if prefix not in PREFIXES:
            continue
        dist[prefix][p.parent.name] += 1
        if len(examples[prefix][p.parent.name]) < 3:
            examples[prefix][p.parent.name].append(p.stem)

    print("=== 各高版本前缀的真实目录分布 ===")
    for prefix in PREFIXES:
        c = dist.get(prefix)
        if not c:
            print(f"  {prefix}xxxx: (无素材)")
            continue
        total = sum(c.values())
        ranked = c.most_common()
        major, major_n = ranked[0]
        print(f"  {prefix}xxxx: total={total}  主流={major}({major_n})  "
              f"分布={dict(ranked)}  覆盖率={major_n/total:.0%}")
        for d, n in ranked:
            print(f"      {d}: {n}  例 {examples[prefix][d]}")

    # 判定：是否存在「同一前缀跨目录」的情况
    cross = {p: c for p, c in dist.items() if len(c) > 1}
    print(f"\n=== 跨目录前缀数 = {len(cross)} / {len(dist)} ===")
    if cross:
        print("结论：get_eqcategory 不能只返回单一目录名，必须支持多目录回退。")
    else:
        print("结论：每个前缀单一目录，get_eqcategory 单映射即可。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
