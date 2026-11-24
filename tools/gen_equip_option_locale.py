#!/usr/bin/env python3
"""Generate equip potline/spark/soul locale entries for the 1..50 custom id space.

Data sources (documented in docs/features/equip-tooltip-native-fields-2026-11-20/refs-high-version-layout.md):
- existing zh-CN 12/10/5 entries (project convention: 1-4 STR/DEX/INT/LUK, 5-6 HP/MP, 7-8 atk, 9-10 def, 11-12 acc/eva)
- BWIKI /maplestory/潜能 formula tables (attribute %, all-stat %, boss dmg, crit, ignore def, etc.)
- high-version reference screenshots (潜在属性/附加潜能 line formats)
This is the SERVER's simplified 1..50 id space (CashOperationHandler r.nextInt(50)+1),
NOT the 095 native 6-digit ItemOption id space — do not mix them.

Writes UTF-8 (no BOM) lines, prints them for review; does not modify files automatically.
"""
import json

# 潜能 1..50：按 BWIKI 潜能等级规则分段（2026-11-20 修订）
# 段位 = 装备潜能等级（第一条 id 定级）：1-12=B 固定值 / 13-24=A +3% /
# 25-36=S +6% / 37-50=SS +9%~+12%。数值随等级递增，与 wiki
# 「数值随潜能等级变化」及「第一条=当前等级」规则自洽。
POT = {}
fixed_b = [
    "力量 +3", "敏捷 +3", "智力 +3", "运气 +3",
    "最大HP +30", "最大MP +30",
    "物理攻击力 +1", "魔法攻击力 +1",
    "物理防御力 +10", "魔法防御力 +10",
    "命中率 +2", "回避率 +2",
]
names_pct = [
    "所有属性", "力量", "敏捷", "智力", "运气", "最大HP", "最大MP",
    "攻击力", "魔法攻击力", "伤害", "BOSS伤害", "暴击率", "无视防御力",
]
# B 段 1-12：固定值
for i in range(1, 13):
    POT[i] = fixed_b[i - 1]
# A/S/SS 段：属性名 × 等级数值（13-24=+3% / 25-36=+6% / 37-50=+9%）
# 13 个名字循环填充 3 段（12+12+12=36，末段 14 条补 2 个高值）。
pct_fills = []
for tier_pct in (3, 6, 9):
    for nm in names_pct:
        if nm in ("BOSS伤害", "暴击率", "无视防御力"):
            val = {"BOSS伤害": {3: 20, 6: 30, 9: 40},
                   "暴击率": {3: 3, 6: 6, 9: 10},
                   "无视防御力": {3: 15, 6: 30, 9: 40}}[nm][tier_pct]
            pct_fills.append(f"{nm} +{val}%")
        else:
            pct_fills.append(f"{nm} +{tier_pct}%")
# 补到 38 条（13..50 = 38 条），用 SS 高值
while len(pct_fills) < 38:
    pct_fills.append("所有属性 +12%")
for idx, i in enumerate(range(13, 51)):
    POT[i] = pct_fills[idx]

# 火花 1..20（现有 10 条 + 百分比扩展）
SPARK = {}
spark_base = [
    "力量 +5", "敏捷 +5", "智力 +5", "运气 +5",
    "最大HP +50", "最大MP +50", "物理攻击力 +2", "魔法攻击力 +2",
    "全属性 +3", "伤害 +3%",
    "所有属性 +6%", "攻击力 +6%", "魔法攻击力 +6%", "BOSS伤害 +30%",
    "暴击率 +8%", "暴击伤害 +8%", "无视防御力 +30%", "最大HP +6%",
    "最大MP +6%", "全属性 +6%",
]
for i in range(1, 21):
    SPARK[i] = spark_base[i - 1]

# 宝珠 1..10（灵魂技能词条）
SOUL = {
    1: "灵魂冲击", 2: "灵魂爆发", 3: "灵魂守护", 4: "灵魂疾风", 5: "灵魂治愈",
    6: "灵魂之刃", 7: "灵魂链接", 8: "灵魂共鸣", 9: "灵魂庇护", 10: "灵魂觉醒",
}

# 通用潜能文案中→英：按属性名+数值模式翻译，覆盖所有等级档位
_POT_NAME_EN = {
    "所有属性": "All Stats", "力量": "STR", "敏捷": "DEX", "智力": "INT", "运气": "LUK",
    "最大HP": "Max HP", "最大MP": "Max MP", "攻击力": "Attack", "魔法攻击力": "Magic Attack",
    "物理攻击力": "Attack", "魔法攻击力": "Magic Attack", "物理防御力": "Defense",
    "魔法防御力": "Magic Defense", "防御力": "Defense", "命中率": "Accuracy", "回避率": "Avoid",
    "伤害": "Damage", "BOSS伤害": "Boss Damage", "暴击率": "Critical Rate",
    "暴击伤害": "Critical Damage", "无视防御力": "Ignore Defense", "移动速度": "Speed", "跳跃力": "Jump",
}


def pot_to_en(s):
    import re
    m = re.fullmatch(r"(.+?)\s*\+(\d+%?)", s)
    if not m:
        return s
    name, val = m.group(1), m.group(2)
    return f"{_POT_NAME_EN.get(name, name)} +{val}"


# 英文表（沿用项目风格：与中文一一对应）
POT_EN = {
    "力量 +3": "STR +3", "敏捷 +3": "DEX +3", "智力 +3": "INT +3", "运气 +3": "LUK +3",
    "最大HP +30": "Max HP +30", "最大MP +30": "Max MP +30",
    "物理攻击力 +1": "Attack +1", "魔法攻击力 +1": "Magic Attack +1",
    "物理防御力 +10": "Defense +10", "魔法防御力 +10": "Magic Defense +10",
    "命中率 +2": "Accuracy +2", "回避率 +2": "Avoid +2",
    "所有属性 +3%": "All Stats +3%", "力量 +3%": "STR +3%", "敏捷 +3%": "DEX +3%",
    "智力 +3%": "INT +3%", "运气 +3%": "LUK +3%", "最大HP +3%": "Max HP +3%",
    "最大MP +3%": "Max MP +3%", "攻击力 +3%": "Attack +3%", "魔法攻击力 +3%": "Magic Attack +3%",
    "伤害 +3%": "Damage +3%", "BOSS伤害 +20%": "Boss Damage +20%", "暴击率 +3%": "Critical Rate +3%",
    "暴击伤害 +5%": "Critical Damage +5%", "无视防御力 +15%": "Ignore Defense +15%",
    "移动速度 +2": "Speed +2", "跳跃力 +2": "Jump +2",
}
SPARK_EN = {
    "力量 +5": "STR +5", "敏捷 +5": "DEX +5", "智力 +5": "INT +5", "运气 +5": "LUK +5",
    "最大HP +50": "Max HP +50", "最大MP +50": "Max MP +50", "物理攻击力 +2": "Attack +2",
    "魔法攻击力 +2": "Magic Attack +2", "全属性 +3": "All Stats +3", "伤害 +3%": "Damage +3%",
    "所有属性 +6%": "All Stats +6%", "攻击力 +6%": "Attack +6%", "魔法攻击力 +6%": "Magic Attack +6%",
    "BOSS伤害 +30%": "Boss Damage +30%", "暴击率 +8%": "Critical Rate +8%",
    "暴击伤害 +8%": "Critical Damage +8%", "无视防御力 +30%": "Ignore Defense +30%",
    "最大HP +6%": "Max HP +6%", "最大MP +6%": "Max MP +6%", "全属性 +6%": "All Stats +6%",
}
SOUL_EN = {
    1: "Soul Strike", 2: "Soul Burst", 3: "Soul Guard", 4: "Soul Gale", 5: "Soul Heal",
    6: "Soul Blade", 7: "Soul Link", 8: "Soul Resonance", 9: "Soul Shelter", 10: "Soul Awakening",
}

zh_lines, en_lines = [], []
for i, v in POT.items():
    zh_lines.append(f"equip.potline.{i}={v}")
    en_lines.append(f"equip.potline.{i}={POT_EN.get(v, pot_to_en(v))}")
for i, v in SPARK.items():
    zh_lines.append(f"equip.spark.{i}={v}")
    en_lines.append(f"equip.spark.{i}={SPARK_EN.get(v, v)}")
for i, v in SOUL.items():
    zh_lines.append(f"equip.soul.{i}={v}")
    en_lines.append(f"equip.soul.{i}={SOUL_EN[i]}")

out = {"zh": zh_lines, "en": en_lines,
       "summary": {"potline": len(POT), "spark": len(SPARK), "soul": len(SOUL)}}
print(json.dumps(out, ensure_ascii=True, indent=1))
