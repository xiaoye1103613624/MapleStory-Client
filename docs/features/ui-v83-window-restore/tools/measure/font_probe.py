# r27-B1b 定版探针：FreeType 口径的 bitmap_top / linespace，以及 CJK 抽样分布
import freetype, json

FONT = r"E:/project/MapleStory-Client/fonts/SimHei/simhei.ttf"
RENDER = freetype.FT_LOAD_RENDER

SAMPLE_CJK = "新手等级家族经验值人气度量敏捷智力运气名称称号技能装备物品金币聊天频道" \
             "设置系统确定取消退出保存应用返回上一页下一队组公会好友手册目录武器防具" \
             "消耗其他特殊角色状态队伍联盟军团转职属性攻击防御魔物速度概率上限下限" \
             "要求限制需要力量运气幸运声望荣誉积分点数剩余已用总计数额单价数量合计" \
             "国内外王国城镇森林洞穴海岸山脉河流桥梁城堡迷宫塔层地下世界时间" \
             "春夏秋冬风雨雪雾云雷光电火土金钢铁石头树木花草鱼虫鸟兽龙虎"

def bt(f, ch):
    try:
        f.load_char(ch, RENDER)
    except Exception:
        return None
    g = f.glyph
    return (g.bitmap_top, g.bitmap.rows, g.advance.x >> 6)

def probe(size):
    f = freetype.Face(FONT)
    f.set_pixel_sizes(0, size)
    # addfont 的 maxrows 口径：ASCII 32..127 最大 bitmap.rows
    maxrows = 0
    for c in range(32, 128):
        m = bt(f, chr(c))
        if m:
            maxrows = max(maxrows, m[1])
    linespace = int(maxrows * 1.35 + 1)
    cjk = {}
    for ch in SAMPLE_CJK:
        if ord(ch) < 128:
            continue
        m = bt(f, ch)
        if m:
            cjk[ch] = m
    cjk_tops = sorted(set(v[0] for v in cjk.values()))
    return {
        "size": size,
        "maxrows_ascii": maxrows,
        "linespace": linespace,
        "digit_bt": bt(f, "0")[0],
        "capA_bt": bt(f, "A")[0],
        "slash_bt": bt(f, "/")[0],
        "paren_bt": bt(f, "(")[0],
        "dash_bt": bt(f, "-")[0],
        "guo_bt": cjk["国"][0],
        "guo_rows": cjk["国"][1],
        "cjk_n": len(cjk),
        "cjk_bt_min": cjk_tops[0],
        "cjk_bt_max": cjk_tops[-1],
        "cjk_bt_hist": {t: sum(1 for v in cjk.values() if v[0] == t) for t in cjk_tops},
        "cjk_rows_max": max(v[1] for v in cjk.values()),
        "digit_adv": bt(f, "0")[2],
        "cjk_adv_min": min(v[2] for v in cjk.values()),
        "cjk_adv_max": max(v[2] for v in cjk.values()),
    }

out = [probe(s) for s in (11, 12, 13, 14, 15, 18)]
with open(r"E:/project/_r27_font_probe.txt", "w", encoding="utf-8") as fp:
    fp.write(json.dumps(out, ensure_ascii=False, indent=1))
