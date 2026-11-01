# 逐串 bitmap_top 分布：客户端实际会画出来的中文词，决定 bt_tall 该取哪一档
import freetype, json
from collections import Counter

FONT = r"E:/project/MapleStory-Client/fonts/SimHei/simhei.ttf"
RENDER = freetype.FT_LOAD_RENDER

WORDS = ["新手", "初心者", "战士", "法师", "弓箭手", "飞侠", "海盗", "骑士", "战神", "龙神", "双弩", "狂龙", "濤",
         "转职", "技能", "装备", "属性", "能力", "道具", "金币", "任务", "聊天", "频道", "系统", "设置",
         "怪物", "手册", "目录", "队伍", "家族", "公会", "好友", "联盟", "名称", "称号", "经验值", "人气度",
         "等级", "体力", "魔力", "力量", "敏捷", "智力", "运气", "命中", "回避", "速度", "攻击", "防御",
         "对谁聊天", "一般", " whispered", "军团", "学院", "勋章", "个人", "情报", "状态", "窗口", "按钮"]

def tops(word, size):
    f = freetype.Face(FONT)
    f.set_pixel_sizes(0, size)
    out = []
    for ch in word:
        if ord(ch) < 128:
            continue
        try:
            f.load_char(ch, RENDER)
        except Exception:
            continue
        out.append((ch, f.glyph.bitmap_top, f.glyph.bitmap.rows))
    return out

res = {}
for size in (11, 12):
    hist = Counter()
    detail = {}
    for w in WORDS:
        t = tops(w, size)
        if not t:
            continue
        detail[w] = "".join("%s%d" % (c, bt) for c, bt, _ in t)
        for c, bt, rows in t:
            hist[bt] += 1
    res[size] = {"hist": dict(sorted(hist.items())), "detail": detail}

with open(r"E:/project/_r27_word_bt.txt", "w", encoding="utf-8") as fp:
    fp.write(json.dumps(res, ensure_ascii=False, indent=1))
