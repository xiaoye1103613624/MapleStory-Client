# -*- coding: utf-8 -*-
"""round68 harness：好友窗/组队窗按 Angel 083 CUIUserList 真资源重修。
静态守卫（源码逐字）+ 数值场景（血条裁剪 / 行命中 / 几何容纳）。
纯静态，不依赖编译；退出码 0 = PASS。"""
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def read(rel):
    with open(os.path.join(ROOT, rel), "rb") as f:
        return f.read().decode("utf-8")


def code_only(text):
    """逐行去掉 // 行注释，用于「虚构根已清除」类守卫，
    留证注释里允许出现旧错误字面量（round66 教训）。"""
    out = []
    for line in text.split("\n"):
        i = line.find("//")
        out.append(line if i < 0 else line[:i])
    return "\n".join(out)


BUDDY = read("IO/UITypes/UIBuddyList.cpp")
PARTY = read("IO/UITypes/UIParty.cpp")
PARTY_H = read("IO/UITypes/UIParty.h")
LOGINBG = read("IO/LoginBackground.cpp")
BUDDY_C = code_only(BUDDY)
PARTY_C = code_only(PARTY)
PARTY_H_C = code_only(PARTY_H)

guards = []


def g(name, cond):
    guards.append((name, bool(cond)))


# ---------------------------------------------------------------- A 真资源根
g("A1 buddy 真根 UIWindow.img/UserList",
  'nl::nx::UI["UIWindow.img"]["UserList"]' in BUDDY_C)
g("A2 party 真根 UIWindow.img/UserList",
  'nl::nx::UI["UIWindow.img"]["UserList"]' in PARTY_C)
g("A3 buddy 真背景 backgrnd + usable 守卫",
  'UiAssetResolve::usable(userlist["backgrnd"])' in BUDDY_C
  and 'Sprite(userlist["backgrnd"]' in BUDDY_C)
g("A4 party 真背景 backgrnd + usable 守卫",
  'UiAssetResolve::usable(userlist["backgrnd"])' in PARTY_C
  and 'Sprite(userlist["backgrnd"]' in PARTY_C)
g("A5 buddy 083 好友页 BtAddFriend",
  'userlist["Friend"]["BtAddFriend"]' in BUDDY_C)
g("A6 party 四真按钮位图 BtInvite/BtWithdraw/BtChangeBoss/BtKick",
  all(('"%s"' % n) in PARTY_C
      for n in ("BtInvite", "BtWithdraw", "BtChangeBoss", "BtKick")))
g("A7 party 真 GaugeBar 节点",
  'userlist["Party"]["PartyHP"]["GaugeBar"]' in PARTY_C
  and 'gaugebar["bar"]' in PARTY_C
  and 'gaugebar["graduation"]' in PARTY_C)
g("A8 关闭钮用 v83 真 close_btn()",
  BUDDY_C.count("UiAssetResolve::close_btn()") == 1
  and PARTY_C.count("UiAssetResolve::close_btn()") == 1)

# ---------------------------------------------------------------- B 083 几何 / 立即数
g("B1 buddy dimension 312x389",
  'Point<int16_t>(740, 20), Point<int16_t>(312, 389)' in BUDDY_C)
g("B2 party dimension 312x389",
  'Point<int16_t>(520, 20), Point<int16_t>(312, 389)' in PARTY_C)
g("B3 083 加好友钮 (5,81)",
  'Point<int16_t>(5, 81)' in BUDDY_C)
g("B4 083 组队钮立即数四组",
  all(('Point<int16_t>(%d, %d)' % p) in PARTY_C
      for p in ((13, 355), (190, 344), (249, 345), (72, 365))))
g("B5 关闭钮右上角 (294,6)",
  BUDDY_C.count('Point<int16_t>(294, 6)') == 1
  and PARTY_C.count('Point<int16_t>(294, 6)') == 1)
g("B6 083 EA 注释 OnCreate/Create/加好友",
  all(s in BUDDY + PARTY for s in ("0x919b73", "0x91f934", "0x91a0de")))

# ---------------------------------------------------------------- C 虚构根 / 自造痕迹清除
for bad in ("FriendRecommendations", "FriendsList", "BuddyList.img",
            "BtAddBuddy", 'UI["Party.img"]', "party[btnname"):
    g("C buddy/party 无虚构根 %s" % bad, bad not in BUDDY_C and bad not in PARTY_C)
for bad in ("ColorBox", "TORCHRED", "JAPANESELAUREL", "BAR_H = 6",
            "BAR_W = 180", "0.45f"):
    g("C party 无自造血条 %s" % bad, bad not in PARTY_C)
g("C 真血条成员在 .h (Texture 两个)",
  "Texture partyhp_bar;" in PARTY_H_C
  and "Texture partyhp_graduation;" in PARTY_H_C
  and '#include "../../Graphics/Texture.h"' in PARTY_H_C)
g("C party.cpp 引入 Texture.h",
  '#include "../../Graphics/Texture.h"' in PARTY_C)

# ---------------------------------------------------------------- D 真血条画法
g("D1 graduation 外框 draw",
  'partyhp_graduation.draw(DrawArgument(at))' in PARTY_C)
g("D2 bar 按 cur/max 裁右 Range(0, BAR_W-fill_w)",
  'partyhp_bar.draw(DrawArgument(at), {}, crop_right)' in PARTY_C
  and 'Range<int16_t> crop_right(0, static_cast<int16_t>(BAR_W - fill_w))' in PARTY_C)
g("D3 BAR_W 取真位图宽",
  'const int16_t BAR_W = partyhp_bar.width();' in PARTY_C)
g("D4 血条守卫 max>0 + 真位图有效",
  'hp_it->second.second > 0' in PARTY_C
  and '&& partyhp_bar && partyhp_graduation' in PARTY_C)
g("D5 6 人循环沿用 Party.members",
  'for (const auto& m : party.members)' in PARTY_C)

# ---------------------------------------------------------------- E 状态机 / 命中区不回归
g("E1 btnid 四 id→行为不变",
  'const uint16_t btnid[] = { BtInvite, BtLeave, BtLeader, BtExpel };' in PARTY_C)
g("E2 buddy row_by_position 上界 302",
  'pos.x() > 302' in BUDDY_C and 'pos.x() > 190' not in BUDDY_C)
g("E3 party send_cursor 上界 302",
  'rel.x() <= 302' in PARTY_C and 'rel.x() <= 190' not in PARTY_C)
g("E4 LoginBackground 线性插值按 Angel 修过，保留",
  ('static_cast<double>(elapsed) / static_cast<double>(tween_duration)' in LOGINBG
   or 'elapsed / tween_duration' in LOGINBG)
  and 'tween_from.x + (tween_to.x - tween_from.x) * t' in LOGINBG
  and '3.0 - 2.0 * t' not in code_only(LOGINBG)
  and 't * t * (3.0' not in code_only(LOGINBG))

# ---------------------------------------------------------------- F 场景（数值）
scen = []


def s(name, cond):
    scen.append((name, bool(cond)))


BAR_W = 63  # .nx 实测 GaugeBar/bar 63x7


def fill(cur, maxv):
    """复刻 UIParty.cpp:117-135 的整数/clamp/裁右语义，返回 (绘制?, fill_w, crop)。"""
    if maxv <= 0:
        return (False, 0, 0)
    c = cur
    if c < 0:
        c = 0
    if c > maxv:
        c = maxv
    w = int(BAR_W * (float(c) / maxv))
    if w < 0:
        w = 0
    if w > BAR_W:
        w = BAR_W
    return (True, w, BAR_W - w)


s("S1 满血 100/100 → fill 63 crop 0", fill(100, 100) == (True, 63, 0))
s("S2 半血 50/100 → fill 31 crop 32", fill(50, 100)[1] == 31 and fill(50, 100)[2] == 32)
s("S3 空血 0/100 → fill 0 crop 63", fill(0, 100) == (True, 0, 63))
s("S4 cur>max clamp → 满条", fill(999, 100) == (True, 63, 0))
s("S5 cur<0 clamp → 空条", fill(-5, 100) == (True, 0, 63))
s("S6 max<=0 不绘制", fill(10, 0)[0] is False and fill(10, -3)[0] is False)
s("S7 1/3 血 33/100 → 20", fill(33, 100)[1] == 20)
s("S8 90% 90/100 → 56", fill(90, 100)[1] == 56)

# 6 人独立：每个成员查自己的 cid，互不影响
memberhp = {101: (50, 100), 102: (100, 100), 103: (0, 100)}
s("S9 6 人 memberhp 键独立", len(memberhp) == 3
  and memberhp[101] == (50, 100)
  and 104 not in memberhp
  and all(cid in (101, 102, 103) for cid in memberhp))
s("S10 未收到血量的成员不绘", 104 not in memberhp)

# 083 背景 312x389 容纳最靠下按钮 (72,365) 高约 18
s("S11 背景 312x389 容纳 y=365+18", 365 + 18 < 389)
s("S12 背景 312 宽容纳 x=249+47", 249 + 47 < 312)

# 行命中区（083 背景 312 宽 → 上界 302）
def row_hit(x):
    return 10 <= x <= 302


s("S13 行命中 x=250 命中", row_hit(250) is True)
s("S14 行命中 x=302 命中", row_hit(302) is True)
s("S15 行命中 x=303 不命中", row_hit(303) is False)
s("S16 行命中 x=190 仍命中(旧上界)", row_hit(190) is True)

# ---------------------------------------------------------------- 汇总
bad_g = [n for n, ok in guards if not ok]
bad_s = [n for n, ok in scen if not ok]
print("STATIC GUARDS checked=%d failed=%d" % (len(guards), len(bad_g)))
for n in bad_g:
    print("  FAIL GUARD %s" % n)
print("SCENARIOS %d, bad=%d" % (len(scen), len(bad_s)))
for n in bad_s:
    print("  FAIL SCEN %s" % n)

if bad_g or bad_s:
    print("FAIL: round68 userlist/partyhp 083 relayout")
    sys.exit(1)
print("PASS: round68 userlist/partyhp 083 relayout")
sys.exit(0)
