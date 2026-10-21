"""Compose MapleStory Character Stat + Detail UI (EN/CN).

Default (v1): existing preview/ paths — do not change behavior.
--v2: extended detail fields, 2–3 col layout, write to preview/v2/ (keeps v1).
--v3: unified label_w, min/range value tiers, left panel AP/Job/Damage, preview/v3/.
--v4: colorful 2-col detail (ref style); label 80 left-align; value 80 / phys ATK 240;
      height matches left panel; clear msyh 11; preview/v4/.
"""
from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

OUT = Path(r"E:\project\MapleStory-Client\docs\features\char-stats-ui-ps")
LAYERS = OUT / "layers"
PREVIEW = OUT / "preview"

# Colors
PINK = (214, 98, 147, 255)
GREEN = (181, 207, 123, 255)
SECTION_BLUE = (89, 116, 155, 255)
LABEL_BLUE = (110, 147, 181, 255)
RESIST_GREY = (85, 85, 85, 255)
PANEL_BG = (225, 225, 225, 255)
PANEL_EDGE = (90, 105, 130, 255)
TITLE_BG = (255, 255, 255, 255)
VALUE_BG = (255, 255, 255, 255)
VALUE_EDGE = (120, 130, 145, 255)
TEXT_DARK = (40, 40, 40, 255)
TEXT_WHITE = (255, 255, 255, 255)
BEVEL_HI = (210, 218, 230, 255)
BEVEL_LO = (110, 125, 150, 255)

# v4 semantic label colors (closer to ref mockup)
V4_COMBAT = (94, 143, 186, 255)       # general combat blue
V4_COMBAT_DK = (70, 110, 160, 255)    # damage
V4_CRIT_RATE = (196, 72, 72, 255)
V4_CRIT_DMG = (90, 168, 90, 255)
V4_FINAL = (210, 90, 150, 255)
V4_BOSS = (140, 90, 180, 255)
V4_IGNORE = (60, 170, 190, 255)
V4_STATUS = (140, 60, 70, 255)
V4_BUFF = (50, 140, 120, 255)
V4_STANCE = (150, 110, 70, 255)
V4_DROP = (200, 130, 60, 255)
V4_MESO = (190, 170, 60, 255)
V4_HONOR = (100, 130, 160, 255)
V4_AILMENT = (90, 115, 140, 255)      # status-avoid rows (muted)

# Shared metrics
PAD = 6
ROW_H = 18
TITLE_H = 20
FOOTER_H = 3
SECTION_H = 16
LABEL_W_L = 70
RESIST_COLS = 3
RESIST_LABEL_H = 14
RESIST_VALUE_H = 16
RESIST_CELL_GAP_Y = 3
CONTENT_BOTTOM_PAD = 2

LEFT_W = 300
RIGHT_W_V1 = 420
RIGHT_W_V2 = 380
# v3: wide enough for 2× range ATK row: 2×(LABEL_W+2+VW_RANGE_V3)+gap+pads
RIGHT_W_V3 = 440

# Value box widths by kind (v2) — sized from UIStatsInfo display formats
# (msyh 9pt measure + ~8px chrome). Caps: StatCaps.h / BeiDou CombatStatCaps.
VW_RANGE = 68   # "99999 ~ 99999" (client: min + " ~ " + max)
VW_PAREN = 66   # "999 (800+199)" buffed/basevs total
VW_NUM = 40     # plain int up to 5 digits (ACC/Honor)
VW_PCT = 34     # "100%" / "140%" integer percent
VW_PCT2 = 44    # "999.99%" two-decimal percent (Final/Crit Dmg)

# v3: most attrs use min tier; ATK/Magic range = prior native interval width ×2
VW_MIN_V3 = VW_PCT  # 34 — baseline for majority of detail value boxes
VW_RANGE_V3 = 144   # 72×2 (was native "99999 ~ 99999" chrome width)
# Unified detail/left label width ≈ 5–6 Chinese chars (msyh9: 5han=45, 6han=54 + pad)
LABEL_W_V3 = 66
LABEL_W_L_V3 = 66

# v4 geometry
LABEL_W_V4 = 80
LABEL_W_L_V4 = 80
VW_NORMAL_V4 = 80
VW_ATK_V4 = 240  # physical attack only
COL_GAP_V4 = 4
# PAD*2 + max(80+2+240, 2*(80+2+80)+COL_GAP) = 12 + 332 = 344 → pad to 352
RIGHT_W_V4 = 352
ROW_H_V4 = 18
FONT_LABEL_V4 = 11
FONT_TITLE_V4 = 12

# v1/v2 left identity (unchanged)
LEFT_IDENTITY = {
    "en": ["Name", "Level", "Guild", "HP", "MP", "EP", "EXP", "Fame"],
    "cn": ["名称", "等级", "公会", "HP", "MP", "EP", "经验值", "名声"],
}
# v3: restore classic UIStatsInfo fields (Job/Damage/AP); keep Level+EXP+EP (useful)
# Classic order: Name, Job, Guild, Fame, Damage, HP, MP, AP, STR…
# Level coexists with Job (not a replacement).
LEFT_IDENTITY_V3 = {
    "en": [
        "Name", "Job", "Level", "Guild", "Fame",
        "Damage", "HP", "MP", "AP", "EXP", "EP",
    ],
    "cn": [
        "名称", "职业", "等级", "公会", "名声",
        "伤害", "HP", "MP", "AP", "经验值", "EP",
    ],
}
LEFT_CORE = {
    "en": ["STR", "DEX", "INT", "LUK"],
    "cn": ["力量", "敏捷", "智力", "幸运"],
}
LEFT_RESIST = {
    "en": [
        "Phys DR", "Fire Res", "Ice Res",
        "Lightning", "Poison Res", "Holy Res",
    ],
    "cn": [
        "物理减伤", "火焰抗性", "冰冷抗性",
        "闪电抗性", "毒素抗性", "神圣抗性",
    ],
}

# v1 right: ("full"|"dual", ...)
RIGHT_SECTIONS_V1 = {
    "en": [
        (
            "Attack Attributes",
            [
                ("full", "ATK Power"),
                ("full", "Magic ATK"),
                ("full", "Accuracy"),
                ("dual", "Crit Damage", "Crit Rate"),
                ("dual", "ATK Speed", "Cast Speed"),
            ],
        ),
        (
            "Defense Attributes",
            [
                ("full", "Defense"),
                ("dual", "HP Regen/s", "MP Regen/s"),
                ("dual", "Move Speed", "Jump"),
                ("dual", "Evasion", "Block Rate"),
            ],
        ),
        (
            "Status Effects",
            [
                ("dual", "Bleed Avoid", "Ignite Avoid"),
                ("dual", "Chill Avoid", "Shock Avoid"),
                ("dual", "Poison Avoid", "Blind Avoid"),
                ("dual", "Stun Avoid", "Curse Reduce"),
            ],
        ),
    ],
    "cn": [
        (
            "攻击属性",
            [
                ("full", "攻击力"),
                ("full", "魔法攻击力"),
                ("full", "命中值"),
                ("dual", "暴击伤害", "暴击率"),
                ("dual", "攻击速度", "施法速度"),
            ],
        ),
        (
            "防御属性",
            [
                ("full", "防御力"),
                ("dual", "每秒HP恢复", "每秒MP恢复"),
                ("dual", "移动速度", "跳跃力"),
                ("dual", "回避率", "格挡率"),
            ],
        ),
        (
            "异常状态",
            [
                ("dual", "避免流血", "避免点燃"),
                ("dual", "避免冰缓", "避免感电"),
                ("dual", "避免中毒", "避免致盲"),
                ("dual", "避免晕眩", "诅咒减免"),
            ],
        ),
    ],
}

# v2/v3: each section is list of rows; each row is list of (label, kind) length 1–3
# kind: range | paren | num | pct | pct2  (see VW_* / UIStatsInfo formats)
RIGHT_SECTIONS_V2 = {
    "en": [
        (
            "Attack",
            [
                [("ATK Power", "range"), ("Magic ATK", "range")],
                [("Accuracy", "num"), ("Crit Damage", "pct2"), ("Crit Rate", "pct")],
                [("ATK Speed", "pct"), ("Cast Speed", "pct"), ("Boss Damage", "pct")],
                [("Dmg Bonus", "pct"), ("Final Damage", "pct2"), ("Ignore DEF", "pct")],
                [("Buff Duration", "pct")],
            ],
        ),
        (
            "Defense",
            [
                [("Defense", "paren"), ("HP Regen", "num"), ("MP Regen", "num")],
                [("Move Speed", "pct"), ("Jump", "pct"), ("Evasion", "pct")],
                [("Block Rate", "pct")],
            ],
        ),
        (
            "Drops / Misc",
            [
                [("Item Drop", "pct"), ("Meso Drop", "pct"), ("Honor", "num")],
            ],
        ),
        (
            "Status",
            [
                [("Bleed Avoid", "pct"), ("Ignite Avoid", "pct"), ("Chill Avoid", "pct")],
                [("Shock Avoid", "pct"), ("Poison Avoid", "pct"), ("Blind Avoid", "pct")],
                [("Stun Avoid", "pct"), ("Curse Reduce", "pct")],
            ],
        ),
    ],
    "cn": [
        (
            "攻击属性",
            [
                [("攻击力", "range"), ("魔法攻击", "range")],
                [("命中值", "num"), ("暴击伤害", "pct2"), ("暴击率", "pct")],
                [("攻击速度", "pct"), ("施法速度", "pct"), ("Boss伤害", "pct")],
                [("额外伤害", "pct"), ("最终伤害", "pct2"), ("无视防御", "pct")],
                [("Buff持续", "pct")],
            ],
        ),
        (
            "防御属性",
            [
                [("防御力", "paren"), ("HP恢复", "num"), ("MP恢复", "num")],
                [("移动速度", "pct"), ("跳跃力", "pct"), ("回避率", "pct")],
                [("格挡率", "pct")],
            ],
        ),
        (
            "掉落/其它",
            [
                [("道具爆率", "pct"), ("金币爆率", "pct"), ("荣誉", "num")],
            ],
        ),
        (
            "异常状态",
            [
                [("避免流血", "pct"), ("避免点燃", "pct"), ("避免冰缓", "pct")],
                [("避免感电", "pct"), ("避免中毒", "pct"), ("避免致盲", "pct")],
                [("避免晕眩", "pct"), ("诅咒减免", "pct")],
            ],
        ),
    ],
}

TITLES = {
    "en": ("Character Data", "Detailed Data"),
    "cn": ("角色资料", "详细资料"),
}

# v4 detail: full-width phys ATK + two columns (label, color_key)
V4_ATK_FULL = {
    "en": ("Phys ATK", "combat"),
    "cn": ("物理攻击力", "combat"),
}
V4_LEFT_COL = {
    "en": [
        ("Magic ATK", "combat"),
        ("Phys DEF", "combat"),
        ("Magic DEF", "combat"),
        ("Accuracy", "combat"),
        ("Avoid", "combat"),
        ("Diligence", "combat"),
        ("ATK Speed", "combat"),
        ("Cast Speed", "combat"),
        ("Move Speed", "combat"),
        ("Jump", "combat"),
        ("Crit Rate", "crit_rate"),
        ("Crit Damage", "crit_dmg"),
        ("Block Rate", "combat"),
        ("HP Regen", "combat"),
        ("MP Regen", "combat"),
        ("Bleed Avoid", "ailment"),
        ("Ignite Avoid", "ailment"),
        ("Chill Avoid", "ailment"),
        ("Shock Avoid", "ailment"),
    ],
    "cn": [
        ("魔法攻击力", "combat"),
        ("物理防御力", "combat"),
        ("魔法防御力", "combat"),
        ("命中率", "combat"),
        ("回避率", "combat"),
        ("手技", "combat"),
        ("攻击速度", "combat"),
        ("施法速度", "combat"),
        ("移动速度", "combat"),
        ("跳跃力", "combat"),
        ("暴击几率", "crit_rate"),
        ("暴击伤害", "crit_dmg"),
        ("格挡率", "combat"),
        ("HP恢复", "combat"),
        ("MP恢复", "combat"),
        ("避免流血", "ailment"),
        ("避免点燃", "ailment"),
        ("避免冰缓", "ailment"),
        ("避免感电", "ailment"),
    ],
}
V4_RIGHT_COL = {
    "en": [
        ("Damage", "dmg"),
        ("Dmg Bonus", "combat"),
        ("Final Damage", "final"),
        ("Boss Damage", "boss"),
        ("Ignore DEF", "ignore"),
        ("Status Resist", "status"),
        ("Buff Duration", "buff"),
        ("Stance", "stance"),
        ("Item Drop", "drop"),
        ("Meso Drop", "meso"),
        ("Honor", "honor"),
        ("Poison Avoid", "ailment"),
        ("Blind Avoid", "ailment"),
        ("Stun Avoid", "ailment"),
        ("Curse Reduce", "ailment"),
        ("Fire Res", "ailment"),
        ("Ice Res", "ailment"),
        ("Lightning Res", "ailment"),
        ("Holy Res", "ailment"),
    ],
    "cn": [
        ("伤害", "dmg"),
        ("额外伤害", "combat"),
        ("最终伤害", "final"),
        ("首领伤害", "boss"),
        ("无视防御率", "ignore"),
        ("状态异常抗性", "status"),
        ("增益持续时间", "buff"),
        ("稳如泰山", "stance"),
        ("道具掉落率", "drop"),
        ("金币获得量", "meso"),
        ("荣誉", "honor"),
        ("避免中毒", "ailment"),
        ("避免致盲", "ailment"),
        ("避免晕眩", "ailment"),
        ("诅咒减免", "ailment"),
        ("火焰抗性", "ailment"),
        ("冰冷抗性", "ailment"),
        ("闪电抗性", "ailment"),
        ("神圣抗性", "ailment"),
    ],
}
V4_COLOR_MAP = {
    "combat": V4_COMBAT,
    "dmg": V4_COMBAT_DK,
    "crit_rate": V4_CRIT_RATE,
    "crit_dmg": V4_CRIT_DMG,
    "final": V4_FINAL,
    "boss": V4_BOSS,
    "ignore": V4_IGNORE,
    "status": V4_STATUS,
    "buff": V4_BUFF,
    "stance": V4_STANCE,
    "drop": V4_DROP,
    "meso": V4_MESO,
    "honor": V4_HONOR,
    "ailment": V4_AILMENT,
}


def font(size: int) -> ImageFont.ImageFont:
    for p in (
        Path(r"C:\Windows\Fonts\msyh.ttc"),
        Path(r"C:\Windows\Fonts\simhei.ttf"),
        Path(r"E:\project\MapleStory-Client\fonts\Arial\Arial.ttf"),
        Path(r"C:\Windows\Fonts\arial.ttf"),
    ):
        if p.exists():
            try:
                return ImageFont.truetype(str(p), size=size, index=0)
            except Exception:
                continue
    return ImageFont.load_default()


def text_size(draw: ImageDraw.ImageDraw, text: str, fnt) -> tuple[int, int]:
    b = draw.textbbox((0, 0), text, font=fnt)
    return b[2] - b[0], b[3] - b[1]


def value_w_for(kind: str) -> int:
    return {
        "range": VW_RANGE,
        "paren": VW_PAREN,
        "num": VW_NUM,
        "pct": VW_PCT,
        "pct2": VW_PCT2,
    }.get(kind, VW_NUM)


def value_w_for_v3(kind: str) -> int:
    """v3: range (ATK/Magic) uses native interval width; everything else uses min tier."""
    if kind == "range":
        return VW_RANGE_V3
    return VW_MIN_V3


def measure_left_content_h(identity_count: int = 8) -> int:
    y = TITLE_H + 4
    y += identity_count * (ROW_H + 1)
    y += 3
    y += 4 * (ROW_H + 1)
    y += 6
    resist_rows = 2
    cell_h = RESIST_LABEL_H + 2 + RESIST_VALUE_H
    y += resist_rows * cell_h + (resist_rows - 1) * RESIST_CELL_GAP_Y
    y += CONTENT_BOTTOM_PAD + FOOTER_H + 2
    return y


def measure_right_v1_h(lang: str) -> int:
    y = TITLE_H + 4
    for _t, rows in RIGHT_SECTIONS_V1[lang]:
        y += SECTION_H + 3
        y += len(rows) * (ROW_H + 1)
        y += 3
    y += CONTENT_BOTTOM_PAD + FOOTER_H + 2
    return y


def measure_right_v2_h(lang: str) -> int:
    y = TITLE_H + 4
    for _t, rows in RIGHT_SECTIONS_V2[lang]:
        y += SECTION_H + 3
        y += len(rows) * (ROW_H + 1)
        y += 3
    y += CONTENT_BOTTOM_PAD + FOOTER_H + 2
    return y


def draw_panel_chrome(img: Image.Image, title: str, *, title_font_size: int = 11) -> None:
    draw = ImageDraw.Draw(img)
    w, h = img.size
    draw.rounded_rectangle([0, 0, w - 1, h - 1], radius=3, fill=PANEL_BG, outline=PANEL_EDGE, width=2)
    draw.rectangle([2, 2, w - 3, TITLE_H], fill=TITLE_BG)
    draw.line([2, TITLE_H, w - 3, TITLE_H], fill=PANEL_EDGE, width=1)
    fnt = font(title_font_size)
    tw, th = text_size(draw, title, fnt)
    tx = int((w - tw) // 2)
    ty = int(max(2, (TITLE_H - th) // 2))
    draw.text((tx, ty), title, font=fnt, fill=TEXT_DARK)
    draw.line([3, h - 4, w - 4, h - 4], fill=BEVEL_HI, width=1)
    draw.line([3, h - 3, w - 4, h - 3], fill=BEVEL_LO, width=1)


def draw_label_value(
    img: Image.Image,
    x: int,
    y: int,
    label: str,
    label_w: int,
    vw: int,
    label_fill,
    fnt,
    *,
    label_left: bool = False,
    row_h: int = ROW_H,
) -> int:
    """Draw one label+value pair; returns total width used. Integer pixel coords."""
    draw = ImageDraw.Draw(img)
    x, y = int(x), int(y)
    label_w, vw = int(label_w), int(vw)
    lh = int(row_h - 2)
    draw.rounded_rectangle(
        [x, y, x + label_w, y + lh], radius=2, fill=label_fill, outline=(60, 60, 60, 255), width=1
    )
    tw, th = text_size(draw, label, fnt)
    tx = x + 3 if label_left else x + max(1, (label_w - tw) // 2)
    ty = y + max(0, (lh - th) // 2) - 1
    draw.text((int(tx), int(ty)), label, font=fnt, fill=TEXT_WHITE)
    vx0 = x + label_w + 2
    draw.rounded_rectangle(
        [vx0, y, vx0 + vw, y + lh], radius=2, fill=VALUE_BG, outline=VALUE_EDGE, width=1
    )
    return label_w + 2 + vw


def draw_section_bar(img: Image.Image, x: int, y: int, w: int, title: str) -> int:
    draw = ImageDraw.Draw(img)
    h = SECTION_H
    draw.rectangle([x, y, x + w, y + h], fill=SECTION_BLUE)
    mid = y + h // 2
    draw.polygon([(x + 3, mid), (x + 8, mid - 3), (x + 8, mid + 3)], fill=TEXT_WHITE)
    draw.polygon([(x + w - 3, mid), (x + w - 8, mid - 3), (x + w - 8, mid + 3)], fill=TEXT_WHITE)
    fnt = font(10)
    tw, th = text_size(draw, title, fnt)
    draw.text((x + (w - tw) // 2, y + max(0, (h - th) // 2) - 1), title, font=fnt, fill=TEXT_WHITE)
    return h


def build_left(
    lang: str,
    panel_h: int,
    left_w: int = LEFT_W,
    *,
    identity: dict | None = None,
    label_w: int = LABEL_W_L,
    label_font_size: int = 10,
    title_font_size: int = 11,
) -> Image.Image:
    title_l, _ = TITLES[lang]
    img = Image.new("RGBA", (left_w, panel_h), (0, 0, 0, 0))
    draw_panel_chrome(img, title_l, title_font_size=title_font_size)
    fnt = font(label_font_size)
    x0 = PAD
    row_w = left_w - PAD * 2
    value_w = row_w - label_w - 3
    y = TITLE_H + 4

    id_map = identity if identity is not None else LEFT_IDENTITY
    for label in id_map[lang]:
        draw_label_value(img, x0, y, label, label_w, value_w, PINK, fnt, label_left=True)
        y += ROW_H + 1

    y += 3
    for label in LEFT_CORE[lang]:
        draw_label_value(img, x0, y, label, label_w, value_w, GREEN, fnt, label_left=True)
        y += ROW_H + 1

    y += 6
    draw = ImageDraw.Draw(img)
    cell_gap = 4
    cell_w = (row_w - cell_gap * (RESIST_COLS - 1)) // RESIST_COLS
    fnt_s = font(max(9, label_font_size - 1))
    for i, label in enumerate(LEFT_RESIST[lang]):
        col = i % RESIST_COLS
        row = i // RESIST_COLS
        cx = x0 + col * (cell_w + cell_gap)
        cy = y + row * (RESIST_LABEL_H + 2 + RESIST_VALUE_H + RESIST_CELL_GAP_Y)
        draw.rounded_rectangle(
            [cx, cy, cx + cell_w, cy + RESIST_LABEL_H],
            radius=2,
            fill=RESIST_GREY,
            outline=(40, 40, 40, 255),
            width=1,
        )
        tw, th = text_size(draw, label, fnt_s)
        draw.text(
            (int(cx + 3), int(cy + max(0, (RESIST_LABEL_H - th) // 2) - 1)),
            label,
            font=fnt_s,
            fill=TEXT_WHITE,
        )
        vy = cy + RESIST_LABEL_H + 2
        draw.rounded_rectangle(
            [cx, vy, cx + cell_w, vy + RESIST_VALUE_H],
            radius=2,
            fill=VALUE_BG,
            outline=VALUE_EDGE,
            width=1,
        )
    return img


def build_right_v1(lang: str, panel_h: int, right_w: int) -> Image.Image:
    _, title_r = TITLES[lang]
    img = Image.new("RGBA", (right_w, panel_h), (0, 0, 0, 0))
    draw_panel_chrome(img, title_r)
    fnt = font(10)
    x0 = PAD
    content_w = right_w - PAD * 2
    y = TITLE_H + 4
    dual_gap = 4
    half_w = (content_w - dual_gap) // 2
    label_w_full = 90
    label_w_dual = 72

    for section_title, rows in RIGHT_SECTIONS_V1[lang]:
        y += draw_section_bar(img, x0, y, content_w, section_title) + 3
        for row in rows:
            if row[0] == "full":
                draw_label_value(
                    img, x0, y, row[1], label_w_full, content_w - label_w_full - 3, LABEL_BLUE, fnt, label_left=True
                )
            else:
                draw_label_value(
                    img, x0, y, row[1], label_w_dual, half_w - label_w_dual - 3, LABEL_BLUE, fnt, label_left=True
                )
                draw_label_value(
                    img,
                    x0 + half_w + dual_gap,
                    y,
                    row[2],
                    label_w_dual,
                    half_w - label_w_dual - 3,
                    LABEL_BLUE,
                    fnt,
                    label_left=True,
                )
            y += ROW_H + 1
        y += 3
    return img


def build_right_v2(lang: str, panel_h: int, right_w: int) -> Image.Image:
    """Compact 2–3 column rows; left-aligned labels; value_w by format kind."""
    _, title_r = TITLES[lang]
    img = Image.new("RGBA", (right_w, panel_h), (0, 0, 0, 0))
    draw_panel_chrome(img, title_r)
    fnt = font(9)
    x0 = PAD
    content_w = right_w - PAD * 2
    y = TITLE_H + 4
    col_gap = 3

    for section_title, rows in RIGHT_SECTIONS_V2[lang]:
        y += draw_section_bar(img, x0, y, content_w, section_title) + 3
        for cells in rows:
            n = len(cells)
            # 2-col only for intentional range pair (ATK/Magic); else keep 3-col rhythm
            dual_wide = n == 2 and cells[0][1] == "range"
            cols = 2 if dual_wide else (3 if n < 3 else n)
            slot_w = (content_w - col_gap * (cols - 1)) // cols
            cx = x0
            for label, kind in cells:
                vw = value_w_for(kind)
                lw = slot_w - 2 - vw
                if lw < 46:
                    vw = max(28, slot_w - 2 - 46)
                    lw = slot_w - 2 - vw
                draw_label_value(img, cx, y, label, lw, vw, LABEL_BLUE, fnt, label_left=True)
                cx += slot_w + col_gap
            y += ROW_H + 1
        y += 3
    return img


def build_right_v3(lang: str, panel_h: int, right_w: int) -> Image.Image:
    """v3: fixed label_w (EN/CN same); value = VW_MIN except range → VW_RANGE_V3."""
    _, title_r = TITLES[lang]
    img = Image.new("RGBA", (right_w, panel_h), (0, 0, 0, 0))
    draw_panel_chrome(img, title_r)
    fnt = font(9)
    x0 = PAD
    content_w = right_w - PAD * 2
    y = TITLE_H + 4
    col_gap = 4
    lw = LABEL_W_V3

    for section_title, rows in RIGHT_SECTIONS_V2[lang]:
        y += draw_section_bar(img, x0, y, content_w, section_title) + 3
        for cells in rows:
            n = len(cells)
            dual_wide = n == 2 and cells[0][1] == "range"
            cols = 2 if dual_wide else max(n, 3 if n < 3 else n)
            # Equal slots; fixed lw/vw inside (labels no longer grow/shrink by column)
            slot_w = (content_w - col_gap * (cols - 1)) // cols
            cx = x0
            for label, kind in cells:
                vw = value_w_for_v3(kind)
                # Keep pair inside slot; prefer fixed sizes, shrink only if panel too narrow
                need = lw + 2 + vw
                if need > slot_w:
                    vw = max(VW_MIN_V3 if kind != "range" else VW_RANGE_V3 - 8, slot_w - 2 - lw)
                draw_label_value(img, cx, y, label, lw, vw, LABEL_BLUE, fnt, label_left=True)
                cx += slot_w + col_gap
            y += ROW_H + 1
        y += 3
    return img


def build_right_v4(lang: str, panel_h: int, right_w: int = RIGHT_W_V4) -> Image.Image:
    """v4: colorful 2-col, label 80 left-align, value 80; phys ATK full-row value 240."""
    _, title_r = TITLES[lang]
    img = Image.new("RGBA", (int(right_w), int(panel_h)), (0, 0, 0, 0))
    draw_panel_chrome(img, title_r, title_font_size=FONT_TITLE_V4)
    fnt = font(FONT_LABEL_V4)
    x0 = int(PAD)
    content_w = int(right_w - PAD * 2)
    lw = LABEL_W_V4
    vw_n = VW_NORMAL_V4
    vw_atk = VW_ATK_V4
    col_gap = COL_GAP_V4
    pair_w = lw + 2 + vw_n
    left_col = V4_LEFT_COL[lang]
    right_col = V4_RIGHT_COL[lang]
    n_rows = max(len(left_col), len(right_col))

    # Distribute vertical space to match left panel height
    top_y = TITLE_H + 4
    bottom_pad = CONTENT_BOTTOM_PAD + FOOTER_H + 2
    # 1 full ATK row + n_rows dual rows
    total_rows = 1 + n_rows
    usable = panel_h - top_y - bottom_pad
    # Prefer at least ROW_H_V4 content; remaining as inter-row gaps
    min_step = ROW_H_V4
    if total_rows > 1:
        gap_extra = max(0, usable - total_rows * min_step)
        # gaps between rows (total_rows - 1) + optional after last
        inter = gap_extra // (total_rows - 1) if total_rows > 1 else 0
        step = min_step + inter
    else:
        step = min_step

    y = int(top_y)
    # Full-width physical ATK
    atk_label, atk_ck = V4_ATK_FULL[lang]
    draw_label_value(
        img,
        x0,
        y,
        atk_label,
        lw,
        vw_atk,
        V4_COLOR_MAP[atk_ck],
        fnt,
        label_left=True,
        row_h=ROW_H_V4,
    )
    y += step

    right_x = x0 + pair_w + col_gap
    for i in range(n_rows):
        if i < len(left_col):
            lab, ck = left_col[i]
            draw_label_value(
                img, x0, y, lab, lw, vw_n, V4_COLOR_MAP[ck], fnt, label_left=True, row_h=ROW_H_V4
            )
        if i < len(right_col):
            lab, ck = right_col[i]
            draw_label_value(
                img, right_x, y, lab, lw, vw_n, V4_COLOR_MAP[ck], fnt, label_left=True, row_h=ROW_H_V4
            )
        y += step
    return img


def compose(lang: str, *, version: str = "v1") -> tuple[Image.Image, Image.Image, Image.Image]:
    if version == "v4":
        right_w = RIGHT_W_V4
        id_map = LEFT_IDENTITY_V3
        h_left = measure_left_content_h(len(id_map[lang]))
        # Right matches left height exactly
        h_right = h_left
        left = build_left(
            lang,
            h_left,
            identity=id_map,
            label_w=LABEL_W_L_V4,
            label_font_size=FONT_LABEL_V4,
            title_font_size=FONT_TITLE_V4,
        )
        right = build_right_v4(lang, h_right, right_w)
    elif version == "v3":
        right_w = RIGHT_W_V3
        id_map = LEFT_IDENTITY_V3
        h_left = measure_left_content_h(len(id_map[lang]))
        h_right = measure_right_v2_h(lang)
        left = build_left(lang, h_left, identity=id_map, label_w=LABEL_W_L_V3)
        right = build_right_v3(lang, h_right, right_w)
    elif version == "v2":
        right_w = RIGHT_W_V2
        h_left = measure_left_content_h(len(LEFT_IDENTITY[lang]))
        h_right = measure_right_v2_h(lang)
        left = build_left(lang, h_left)
        right = build_right_v2(lang, h_right, right_w)
    else:
        right_w = RIGHT_W_V1
        h_left = measure_left_content_h(len(LEFT_IDENTITY[lang]))
        h_right = measure_right_v1_h(lang)
        left = build_left(lang, h_left)
        right = build_right_v1(lang, h_right, right_w)

    gap = 6
    canvas_h = max(h_left, h_right)
    canvas = Image.new("RGBA", (LEFT_W + gap + right_w, canvas_h), (0, 0, 0, 0))
    canvas.paste(left, (0, 0), left)
    canvas.paste(right, (LEFT_W + gap, 0), right)
    return canvas, left, right


def save_set(
    en: Image.Image,
    cn: Image.Image,
    left_en: Image.Image,
    left_cn: Image.Image,
    right_en: Image.Image,
    right_cn: Image.Image,
    *,
    preview_dir: Path,
    layers_dir: Path,
    root_copies: bool,
    name_suffix: str = "",
) -> None:
    preview_dir.mkdir(parents=True, exist_ok=True)
    layers_dir.mkdir(parents=True, exist_ok=True)

    suf = name_suffix  # e.g. "" or "_v2" or "_v3"
    left_en.save(layers_dir / f"main_EN{suf}.png")
    left_cn.save(layers_dir / f"main_CN{suf}.png")
    right_en.save(layers_dir / f"detail_EN{suf}.png")
    right_cn.save(layers_dir / f"detail_CN{suf}.png")
    en.save(layers_dir / f"flat_EN{suf}.png")
    cn.save(layers_dir / f"flat_CN{suf}.png")

    if suf in ("_v2", "_v3", "_v4"):
        tag = suf  # _v2 / _v3 / _v4
        pairs = [
            (en, f"Character_Stats_Detail{tag}_EN.png"),
            (cn, f"Character_Stats_Detail{tag}_CN.png"),
            (left_en, f"Character_Stats_Main{tag}_EN.png"),
            (left_cn, f"Character_Stats_Main{tag}_CN.png"),
            (right_en, f"Character_Stats_DetailPanel{tag}_EN.png"),
            (right_cn, f"Character_Stats_DetailPanel{tag}_CN.png"),
        ]
        sheet_name = f"Character_Stats_Detail{tag}_EN_CN_sheet.png"
    else:
        pairs = [
            (en, "Character_Stats_Detail_EN.png"),
            (cn, "Character_Stats_Detail_CN.png"),
            (left_en, "Character_Stats_Main_EN.png"),
            (left_cn, "Character_Stats_Main_CN.png"),
            (right_en, "Character_Stats_DetailPanel_EN.png"),
            (right_cn, "Character_Stats_DetailPanel_CN.png"),
        ]
        sheet_name = "Character_Stats_Detail_EN_CN_sheet.png"

    for im, name in pairs:
        im.save(preview_dir / name)
        if root_copies:
            im.save(OUT / name)

    sheet_h = max(en.size[1], cn.size[1])
    sheet = Image.new("RGBA", (en.size[0] + cn.size[0] + 16, sheet_h), (40, 44, 52, 255))
    sheet.paste(en, (0, 0), en)
    sheet.paste(cn, (en.size[0] + 16, 0), cn)
    sheet.save(preview_dir / sheet_name)
    if root_copies:
        sheet.save(OUT / sheet_name)


def run_v1() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    en, left_en, right_en = compose("en", version="v1")
    cn, left_cn, right_cn = compose("cn", version="v1")
    save_set(
        en, cn, left_en, left_cn, right_en, right_cn,
        preview_dir=PREVIEW,
        layers_dir=LAYERS,
        root_copies=True,
        name_suffix="",
    )
    (OUT / "compose_meta.txt").write_text(
        "version=v1 (default paths)\n"
        f"side_by_side={en.size} main={left_en.size} detail={right_en.size}\n"
        "values=EMPTY buttons=NOT baked\n",
        encoding="utf-8",
    )
    print("OK v1 Main", left_en.size, "| DetailPanel", right_en.size, "| side-by-side", en.size)


def run_v2() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    preview_v2 = PREVIEW / "v2"
    layers_v2 = LAYERS / "v2"
    en, left_en, right_en = compose("en", version="v2")
    cn, left_cn, right_cn = compose("cn", version="v2")
    save_set(
        en, cn, left_en, left_cn, right_en, right_cn,
        preview_dir=preview_v2,
        layers_dir=layers_v2,
        root_copies=False,
        name_suffix="_v2",
    )
    (OUT / "compose_meta_v2.txt").write_text(
        "version=v2 extended (preview/v2/ — v1 untouched)\n"
        f"RIGHT_W={RIGHT_W_V2}\n"
        f"VW_RANGE={VW_RANGE} VW_PAREN={VW_PAREN} VW_NUM={VW_NUM} VW_PCT={VW_PCT} VW_PCT2={VW_PCT2}\n"
        f"side_by_side={en.size} main={left_en.size} detail={right_en.size}\n"
        "added=Boss Damage, Dmg Bonus, Final Damage, Ignore DEF, Buff Duration, Item Drop, Meso Drop\n"
        "layout=max 3 cols; label left-align; value_w by UIStatsInfo format\n"
        "kinds=range(min~max) paren(buffed) num pct(%) pct2(0.00%)\n"
        "source=IO/UITypes/UIStatsInfo.cpp + Character/StatCaps.h + BeiDou CombatStatCaps\n"
        "values=EMPTY buttons=NOT baked\n",
        encoding="utf-8",
    )
    print("OK v2 Main", left_en.size, "| DetailPanel", right_en.size, "| side-by-side", en.size, "->", preview_v2)


def run_v3() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    preview_v3 = PREVIEW / "v3"
    layers_v3 = LAYERS / "v3"
    en, left_en, right_en = compose("en", version="v3")
    cn, left_cn, right_cn = compose("cn", version="v3")
    save_set(
        en, cn, left_en, left_cn, right_en, right_cn,
        preview_dir=preview_v3,
        layers_dir=layers_v3,
        root_copies=False,
        name_suffix="_v3",
    )
    (OUT / "compose_meta_v3.txt").write_text(
        "version=v3 (preview/v3/ — v1/v2 untouched)\n"
        f"RIGHT_W={RIGHT_W_V3} LABEL_W_V3={LABEL_W_V3} LABEL_W_L_V3={LABEL_W_L_V3}\n"
        f"VW_MIN_V3={VW_MIN_V3} VW_RANGE_V3={VW_RANGE_V3}\n"
        f"side_by_side={en.size} main={left_en.size} detail={right_en.size}\n"
        "label_w=unified ~5-6 Chinese chars (EN/CN same)\n"
        "value_tiers=min(most attrs)=VW_MIN_V3; range(ATK/Magic)=VW_RANGE_V3 for 99999 ~ 99999\n"
        "left_added=Job, Damage, AP (ABILITY POINT); Level kept alongside Job; EXP/EP kept\n"
        "left_order=Name,Job,Level,Guild,Fame,Damage,HP,MP,AP,EXP,EP + STR/DEX/INT/LUK + resists\n"
        "source=IO/UITypes/UIStatsInfo.h StatLabel + classic main panel\n"
        "values=EMPTY buttons=NOT baked\n",
        encoding="utf-8",
    )
    print("OK v3 Main", left_en.size, "| DetailPanel", right_en.size, "| side-by-side", en.size, "->", preview_v3)


COMPONENTS_V4 = OUT / "ui" / "detail" / "v4" / "components"
CHIP_H_V4 = ROW_H_V4 - 2  # 16
LEFT_VALUE_W_V4 = LEFT_W - PAD * 2 - LABEL_W_L_V4 - 3  # 205


def _rounded_chip(w: int, h: int, fill, outline=(60, 60, 60, 255)) -> Image.Image:
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle([0, 0, w - 1, h - 1], radius=2, fill=fill, outline=outline, width=1)
    return img


def _panel_blank(w: int, h: int) -> Image.Image:
    """Panel chrome without baked title text (title drawn as editable PS text)."""
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle([0, 0, w - 1, h - 1], radius=3, fill=PANEL_BG, outline=PANEL_EDGE, width=2)
    draw.rectangle([2, 2, w - 3, TITLE_H], fill=TITLE_BG)
    draw.line([2, TITLE_H, w - 3, TITLE_H], fill=PANEL_EDGE, width=1)
    draw.line([3, h - 4, w - 4, h - 4], fill=BEVEL_HI, width=1)
    draw.line([3, h - 3, w - 4, h - 3], fill=BEVEL_LO, width=1)
    return img


def export_v4_components() -> Path:
    """Export reusable 1x component PNGs for PSD assembly + asset shelf."""
    COMPONENTS_V4.mkdir(parents=True, exist_ok=True)
    chip_keys = {
        "pink": PINK,
        "green": GREEN,
        "resist": RESIST_GREY,
        **{k: v for k, v in V4_COLOR_MAP.items()},
    }
    for key, fill in chip_keys.items():
        _rounded_chip(LABEL_W_V4, CHIP_H_V4, fill).save(COMPONENTS_V4 / f"label_chip_{key}.png")

    _rounded_chip(VW_NORMAL_V4, CHIP_H_V4, VALUE_BG, VALUE_EDGE).save(COMPONENTS_V4 / "value_box_80.png")
    _rounded_chip(VW_ATK_V4, CHIP_H_V4, VALUE_BG, VALUE_EDGE).save(COMPONENTS_V4 / "value_box_240.png")
    _rounded_chip(LEFT_VALUE_W_V4, CHIP_H_V4, VALUE_BG, VALUE_EDGE).save(
        COMPONENTS_V4 / f"value_box_{LEFT_VALUE_W_V4}.png"
    )

    # resist cell parts (3-col under left panel)
    row_w = LEFT_W - PAD * 2
    cell_gap = 4
    cell_w = (row_w - cell_gap * (RESIST_COLS - 1)) // RESIST_COLS
    _rounded_chip(cell_w, RESIST_LABEL_H, RESIST_GREY, (40, 40, 40, 255)).save(
        COMPONENTS_V4 / "resist_label.png"
    )
    _rounded_chip(cell_w, RESIST_VALUE_H, VALUE_BG, VALUE_EDGE).save(COMPONENTS_V4 / "resist_value.png")

    h_panel = measure_left_content_h(len(LEFT_IDENTITY_V3["cn"]))
    _panel_blank(LEFT_W, h_panel).save(COMPONENTS_V4 / "panel_bg_main.png")
    _panel_blank(RIGHT_W_V4, h_panel).save(COMPONENTS_V4 / "panel_bg_detail.png")

    # named inventory for manifest
    names = sorted(p.name for p in COMPONENTS_V4.glob("*.png"))
    (COMPONENTS_V4 / "INDEX.txt").write_text("\n".join(names) + "\n", encoding="utf-8")
    print("OK v4 components", len(names), "->", COMPONENTS_V4)
    return COMPONENTS_V4


def _v4_row_step(panel_h: int, n_dual: int) -> tuple[int, int]:
    """Return (top_y, step) matching build_right_v4 spacing."""
    top_y = TITLE_H + 4
    bottom_pad = CONTENT_BOTTOM_PAD + FOOTER_H + 2
    total_rows = 1 + n_dual
    usable = panel_h - top_y - bottom_pad
    min_step = ROW_H_V4
    inter = max(0, usable - total_rows * min_step) // (total_rows - 1) if total_rows > 1 else 0
    return top_y, min_step + inter


def write_v4_psd_jsx(lang: str = "cn") -> Path:
    """Emit ExtendScript that builds componentized PSD (Final + Materials shelf)."""
    comp = COMPONENTS_V4.as_posix()
    out_psd = (OUT / "Character_Stats_Detail_V4.psd").as_posix()
    panel_h = measure_left_content_h(len(LEFT_IDENTITY_V3[lang]))
    n_dual = max(len(V4_LEFT_COL[lang]), len(V4_RIGHT_COL[lang]))
    top_y, step = _v4_row_step(panel_h, n_dual)
    pair_w = LABEL_W_V4 + 2 + VW_NORMAL_V4
    detail_x = LEFT_W + 6
    shelf_x = LEFT_W + 6 + RIGHT_W_V4 + 24
    canvas_w = shelf_x + 420
    canvas_h = max(panel_h, 520)
    title_l, title_r = TITLES[lang]

    # Materials inventory (unique chips)
    mat_items = [
        ("label_chip_pink.png", "mat_label_pink"),
        ("label_chip_green.png", "mat_label_green"),
        ("label_chip_resist.png", "mat_label_resist"),
        ("label_chip_combat.png", "mat_label_combat"),
        ("label_chip_dmg.png", "mat_label_dmg"),
        ("label_chip_crit_rate.png", "mat_label_crit_rate"),
        ("label_chip_crit_dmg.png", "mat_label_crit_dmg"),
        ("label_chip_final.png", "mat_label_final"),
        ("label_chip_boss.png", "mat_label_boss"),
        ("label_chip_ignore.png", "mat_label_ignore"),
        ("label_chip_status.png", "mat_label_status"),
        ("label_chip_buff.png", "mat_label_buff"),
        ("label_chip_stance.png", "mat_label_stance"),
        ("label_chip_drop.png", "mat_label_drop"),
        ("label_chip_meso.png", "mat_label_meso"),
        ("label_chip_honor.png", "mat_label_honor"),
        ("label_chip_ailment.png", "mat_label_ailment"),
        ("value_box_80.png", "mat_value_80"),
        ("value_box_240.png", "mat_value_240"),
        (f"value_box_{LEFT_VALUE_W_V4}.png", "mat_value_left"),
        ("resist_label.png", "mat_resist_label"),
        ("resist_value.png", "mat_resist_value"),
        ("panel_bg_main.png", "mat_panel_main"),
        ("panel_bg_detail.png", "mat_panel_detail"),
    ]

    # Build place list for Final: (file, layerName, x, y, templateMatName|None)
    # templateMatName means duplicate from materials after opening — we place from file each time for reliability
    places: list[tuple[str, str, int, int]] = []
    texts: list[tuple[str, str, int, int, int]] = []  # name, contents, x, y, size

    places.append(("panel_bg_main.png", "Main_PanelBg", 0, 0))
    places.append(("panel_bg_detail.png", "Detail_PanelBg", detail_x, 0))
    texts.append(("Main_Title", title_l, LEFT_W // 2, 4, FONT_TITLE_V4))
    texts.append(("Detail_Title", title_r, detail_x + RIGHT_W_V4 // 2, 4, FONT_TITLE_V4))

    # left identity / core
    y = TITLE_H + 4
    x0 = PAD
    for i, lab in enumerate(LEFT_IDENTITY_V3[lang]):
        places.append(("label_chip_pink.png", f"Main_IdChip_{i}", x0, y))
        places.append((f"value_box_{LEFT_VALUE_W_V4}.png", f"Main_IdVal_{i}", x0 + LABEL_W_L_V4 + 2, y))
        texts.append((f"Main_IdTxt_{i}", lab, x0 + 3, y + 1, FONT_LABEL_V4))
        y += ROW_H + 1
    y += 3
    for i, lab in enumerate(LEFT_CORE[lang]):
        places.append(("label_chip_green.png", f"Main_CoreChip_{i}", x0, y))
        places.append((f"value_box_{LEFT_VALUE_W_V4}.png", f"Main_CoreVal_{i}", x0 + LABEL_W_L_V4 + 2, y))
        texts.append((f"Main_CoreTxt_{i}", lab, x0 + 3, y + 1, FONT_LABEL_V4))
        y += ROW_H + 1
    y += 6
    row_w = LEFT_W - PAD * 2
    cell_gap = 4
    cell_w = (row_w - cell_gap * (RESIST_COLS - 1)) // RESIST_COLS
    for i, lab in enumerate(LEFT_RESIST[lang]):
        col = i % RESIST_COLS
        row = i // RESIST_COLS
        cx = x0 + col * (cell_w + cell_gap)
        cy = y + row * (RESIST_LABEL_H + 2 + RESIST_VALUE_H + RESIST_CELL_GAP_Y)
        places.append(("resist_label.png", f"Main_ResLbl_{i}", cx, cy))
        places.append(("resist_value.png", f"Main_ResVal_{i}", cx, cy + RESIST_LABEL_H + 2))
        texts.append((f"Main_ResTxt_{i}", lab, cx + 3, cy + 1, 9))

    # detail ATK + dual cols
    dy = top_y
    dx0 = detail_x + PAD
    atk_lab, atk_ck = V4_ATK_FULL[lang]
    places.append((f"label_chip_{atk_ck}.png", "Detail_AtkChip", dx0, dy))
    places.append(("value_box_240.png", "Detail_AtkVal", dx0 + LABEL_W_V4 + 2, dy))
    texts.append(("Detail_AtkTxt", atk_lab, dx0 + 3, dy + 1, FONT_LABEL_V4))
    dy += step
    right_x = dx0 + pair_w + COL_GAP_V4
    left_col = V4_LEFT_COL[lang]
    right_col = V4_RIGHT_COL[lang]
    for i in range(n_dual):
        if i < len(left_col):
            lab, ck = left_col[i]
            places.append((f"label_chip_{ck}.png", f"Detail_LChip_{i}", dx0, dy))
            places.append(("value_box_80.png", f"Detail_LVal_{i}", dx0 + LABEL_W_V4 + 2, dy))
            texts.append((f"Detail_LTxt_{i}", lab, dx0 + 3, dy + 1, FONT_LABEL_V4))
        if i < len(right_col):
            lab, ck = right_col[i]
            places.append((f"label_chip_{ck}.png", f"Detail_RChip_{i}", right_x, dy))
            places.append(("value_box_80.png", f"Detail_RVal_{i}", right_x + LABEL_W_V4 + 2, dy))
            texts.append((f"Detail_RTxt_{i}", lab, right_x + 3, dy + 1, FONT_LABEL_V4))
        dy += step

    def esc(s: str) -> str:
        """JS string escape; non-ASCII as \\uXXXX for ExtendScript encoding safety."""
        out: list[str] = []
        for ch in s:
            o = ord(ch)
            if ch in ("\\", '"'):
                out.append("\\" + ch)
            elif o < 32 or o > 126:
                out.append(f"\\u{o:04x}")
            else:
                out.append(ch)
        return "".join(out)

    lines: list[str] = []
    lines.append("// Auto-generated: char-stats v4 componentized PSD")
    lines.append("app.displayDialogs = DialogModes.NO;")
    lines.append("app.preferences.rulerUnits = Units.PIXELS;")
    lines.append(f'var COMP = "{comp}/";')
    lines.append(f'var OUT_PSD = "{out_psd}";')
    lines.append(f"var W = {canvas_w}, H = {canvas_h};")
    lines.append(
        "var doc = app.documents.add(W, H, 72, 'Character_Stats_Detail_V4', "
        "NewDocumentMode.RGB, DocumentFill.TRANSPARENT);"
    )
    lines.append(
        """
function placeFile(path, name, dx, dy) {
  var f = new File(path);
  if (!f.exists) { throw new Error('missing: ' + path); }
  var desc = new ActionDescriptor();
  desc.putPath(charIDToTypeID('null'), f);
  desc.putEnumerated(charIDToTypeID('FTcs'), charIDToTypeID('QCSt'), charIDToTypeID('Qcsa'));
  executeAction(charIDToTypeID('Plc '), desc, DialogModes.NO);
  var layer = app.activeDocument.activeLayer;
  layer.name = name;
  var b = layer.bounds;
  layer.translate(dx - b[0].as('px'), dy - b[1].as('px'));
  return layer;
}
function addText(name, contents, x, y, sizePt, r, g, b) {
  var layer = app.activeDocument.artLayers.add();
  layer.kind = LayerKind.TEXT;
  layer.name = name;
  var ti = layer.textItem;
  ti.kind = TextType.POINTTEXT;
  ti.contents = contents;
  ti.position = [x, y + sizePt];
  try { ti.size = sizePt; } catch (eSz) { try { ti.size = new UnitValue(sizePt, 'pt'); } catch (eSz2) {} }
  try { ti.font = "MicrosoftYaHei"; } catch (e1) {
    try { ti.font = "MicrosoftYaHeiUI"; } catch (e2) {
      try { ti.font = "SimHei"; } catch (e3) {}
    }
  }
  var c = new SolidColor();
  c.rgb.red = r; c.rgb.green = g; c.rgb.blue = b;
  ti.color = c;
  return layer;
}
function ensureGroup(name) {
  var g = app.activeDocument.layerSets.add();
  g.name = name;
  return g;
}
"""
    )
    # Place everything at document root (avoid Place-into-group issues)
    lines.append("// --- Materials shelf (reusable component sprites) ---")
    mx, my = shelf_x, 8
    col = 0
    mat_names: list[str] = []
    for fname, lname in mat_items:
        px = mx + (col % 4) * 100
        py = my + (col // 4) * 28
        if "panel_bg" in fname:
            px = mx
            py = my + 220 + (0 if "main" in fname else 240)
        lines.append(f'placeFile(COMP + "{fname}", "{lname}", {px}, {py});')
        mat_names.append(lname)
        col += 1

    lines.append("// --- Final assembly (component instances) ---")
    final_names: list[str] = []
    for fname, lname, x, y in places:
        lines.append(f'placeFile(COMP + "{fname}", "{esc(lname)}", {int(x)}, {int(y)});')
        final_names.append(lname)

    lines.append("// --- Editable text layers ---")
    text_names: list[str] = []
    for name, contents, x, y, size in texts:
        if name.endswith("_Title"):
            r, g, b = 40, 40, 40
        else:
            r, g, b = 255, 255, 255
        lines.append(
            f'addText("{esc(name)}", "{esc(contents)}", {int(x)}, {int(y)}, {int(size)}, {r}, {g}, {b});'
        )
        text_names.append(name)

    # Organize into layer groups by name (after all placed)
    lines.append(
        """
function findLayer(name) {
  var doc = app.activeDocument;
  var i;
  for (i = 0; i < doc.artLayers.length; i++) {
    if (doc.artLayers[i].name === name) return doc.artLayers[i];
  }
  return null;
}
function moveList(names, group) {
  var i, L;
  for (i = 0; i < names.length; i++) {
    L = findLayer(names[i]);
    if (L) L.move(group, ElementPlacement.INSIDE);
  }
}
"""
    )
    # Nested LayerSets: create children ON parent (LayerSet.move INSIDE is illegal on some PS)
    lines.append("var gMat = ensureGroup('Materials');")
    lines.append("var gFinal = ensureGroup('Final');")
    lines.append("var gMain = gFinal.layerSets.add(); gMain.name = 'Final_Main';")
    lines.append("var gDetail = gFinal.layerSets.add(); gDetail.name = 'Final_Detail';")
    lines.append("var gTexts = gFinal.layerSets.add(); gTexts.name = 'Final_Texts';")
    lines.append(f"moveList([{', '.join(chr(34)+n+chr(34) for n in mat_names)}], gMat);")
    main_ns = [n for n in final_names if n.startswith("Main_")]
    detail_ns = [n for n in final_names if n.startswith("Detail_")]
    lines.append(f"moveList([{', '.join(chr(34)+n+chr(34) for n in main_ns)}], gMain);")
    lines.append(f"moveList([{', '.join(chr(34)+n+chr(34) for n in detail_ns)}], gDetail);")
    lines.append(f"moveList([{', '.join(chr(34)+n+chr(34) for n in text_names)}], gTexts);")

    lines.append(
        """
var outFile = new File(OUT_PSD);
var opts = new PhotoshopSaveOptions();
opts.embedColorProfile = true;
opts.alphaChannels = true;
opts.layers = true;
app.activeDocument.saveAs(outFile, opts, true, Extension.LOWERCASE);
"""
    )
    lines.append(
        f"var __v4Result = {{ ok: true, path: OUT_PSD, places: {len(places)}, texts: {len(texts)}, "
        f"components: {len(mat_items)}, canvas: [W, H] }};"
    )

    jsx_path = OUT / "ui" / "detail" / "v4" / "build_v4_componentized.jsx"
    jsx_path.parent.mkdir(parents=True, exist_ok=True)
    jsx_path.write_text("\n".join(lines), encoding="utf-8")
    print("OK v4 jsx", jsx_path, "places", len(places), "texts", len(texts))
    return jsx_path


def run_v4() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    preview_v4 = PREVIEW / "v4"
    layers_v4 = LAYERS / "v4"
    en, left_en, right_en = compose("en", version="v4")
    cn, left_cn, right_cn = compose("cn", version="v4")
    save_set(
        en, cn, left_en, left_cn, right_en, right_cn,
        preview_dir=preview_v4,
        layers_dir=layers_v4,
        root_copies=False,
        name_suffix="_v4",
    )
    export_v4_components()
    write_v4_psd_jsx("cn")
    (OUT / "compose_meta_v4.txt").write_text(
        "version=v4 (preview/v4/ — v1/v2/v3 untouched)\n"
        f"RIGHT_W={RIGHT_W_V4} LABEL_W_V4={LABEL_W_V4} LABEL_W_L_V4={LABEL_W_L_V4}\n"
        f"VW_NORMAL_V4={VW_NORMAL_V4} VW_ATK_V4={VW_ATK_V4} (phys ATK only)\n"
        f"FONT_LABEL={FONT_LABEL_V4} FONT_TITLE={FONT_TITLE_V4}\n"
        f"side_by_side={en.size} main={left_en.size} detail={right_en.size}\n"
        "layout=full-width Phys ATK + 2-col colorful labels; no section bars\n"
        "label=80px left-align; value=80px except Phys ATK=240\n"
        "height=detail matches main (character data)\n"
        "components=ui/detail/v4/components/ (chips+value boxes+panel bg)\n"
        "psd=componentized Final + Materials shelf + editable text layers\n"
        "source=ref mockup + UIStatsInfo.h detailed + extras (diligence/stance/split DEF)\n"
        "values=EMPTY buttons=NOT baked\n",
        encoding="utf-8",
    )
    print("OK v4 Main", left_en.size, "| DetailPanel", right_en.size, "| side-by-side", en.size, "->", preview_v4)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--v2", action="store_true", help="Build extended v2 into preview/v2/ (keep v1)")
    ap.add_argument("--v3", action="store_true", help="Build v3 into preview/v3/ (keep v1/v2)")
    ap.add_argument("--v4", action="store_true", help="Build v4 into preview/v4/ (keep v1/v2/v3)")
    ap.add_argument(
        "--v4-components",
        action="store_true",
        help="Only export v4 component PNGs + componentized PSD JSX",
    )
    ap.add_argument("--all", action="store_true", help="Build v1, v2, v3, and v4")
    args = ap.parse_args()
    if args.all:
        run_v1()
        run_v2()
        run_v3()
        run_v4()
    elif args.v4_components:
        export_v4_components()
        write_v4_psd_jsx("cn")
    elif args.v4:
        run_v4()
    elif args.v3:
        run_v3()
    elif args.v2:
        run_v2()
    else:
        run_v1()


if __name__ == "__main__":
    main()
