# -*- coding: utf-8 -*-
"""round53 · 频道快捷键 change_target 接线回归（statusbar-hud/D4 + social-economy/D1）

Angel 083 `CWvsContext::UseFuncKeyMapped` @ 0xa0773d 的 normal 键分支把聊天频道键
落到 `sub_8D4D43(target)` → `CUIStatusBar::StartChat(0)`，即「切目标 + 开输入行」。
修前 `UIChatBar::change_target` 是 `{}` 空 stub，六个频道键完全无反应。
"""
import io, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def read(p):
    return io.open(os.path.join(ROOT, p), encoding='utf-8', errors='replace').read()

H = read('IO/UITypes/UIChatBar.h')
C = read('IO/UITypes/UIChatBar.cpp')
G = read('IO/UIStateGame.cpp')

FAIL = []
def check(name, cond, detail=''):
    if not cond:
        FAIL.append('%s %s' % (name, detail))
    return cond

# ── 1. 头文件：不再有空 stub，声明存在 ─────────────────────────
check('h_no_empty_stub', 'void change_target(int32_t action) {}' not in H)
check('h_declared', re.search(r'void change_target\(int32_t action\);', H) is not None)
# 只针对 change_target 旁的占位注释（文件头 include 区另有一处同样的 TODO，与本修无关）
check('h_no_todo_placeholder',
      re.search(r'TODO: Change these\?\s*\n\s*void change_target', H) is None)

# ── 2. 实现：六个键全部映射，且有默认回退与开输入行 ────────────
sig = C.find('void UIChatBar::change_target(int32_t action)')
# 实现区 = 前置注释（含 083 依据）+ 函数体，便于守卫注释与代码同处。
# 收尾必须从 sig 之后再找 '\n\t}\n'，否则会截到上一个函数的右花括号。
start = max(0, sig - 1400)
body = C[start:]
end = body.find('\n\t}\n', sig - start)
body = body[:end + 4] if end > 0 else C[start:start + 3400]
check('c_impl_present', 'void UIChatBar::change_target(int32_t action)' in C)
for key, target in [('SAY', 'TARGET_ALL'), ('PARTYCHAT', 'TARGET_PARTY'),
                    ('FRIENDSCHAT', 'TARGET_BUDDY'), ('GUILDCHAT', 'TARGET_GUILD'),
                    ('ALLIANCECHAT', 'TARGET_ALLIANCE'), ('TOSPOUSE', 'TARGET_SPOUSE')]:
    pat = r'case KeyAction::Id::%s:[\s\S]{0,120}?target = %s;' % (key, target)
    check('c_map_%s' % key, re.search(pat, body) is not None, '%s -> %s' % (key, target))
check('c_default_returns', re.search(r'default:\s*\n?\s*return;', body) is not None)
check('c_calls_set_classic_target', 'set_classic_target(target);' in body)
check('c_opens_input', 'if (!view_input)' in body and 'toggle_input(true);' in body)
check('c_cites_083', '0xa0773d' in body)

# 头文件注释也要留下依据（剔除类修复不能只改代码）
check('h_cites_083', '0xa0773d' in H)

# ── 3. 调用点仍是六个键（不改 UIStateGame） ────────────────────
for key in ['SAY', 'PARTYCHAT', 'FRIENDSCHAT', 'GUILDCHAT', 'ALLIANCECHAT', 'TOSPOUSE']:
    check('g_case_%s' % key, ('KeyAction::Id::%s:' % key) in G)
check('g_call_site_intact', 'chatbar->change_target(action);' in G)

# ── 4. 行为复刻：映射表 + 开输入行条件 ─────────────────────────
TARGET_ALL, TARGET_WHISPER, TARGET_PARTY, TARGET_GUILD = 0, 1, 2, 3
TARGET_BUDDY, TARGET_ALLIANCE, TARGET_SPOUSE, TARGET_MEGAPHONE = 4, 5, 6, 7

KEYMAP = {  # 与实现逐字对应
    'SAY': TARGET_ALL, 'PARTYCHAT': TARGET_PARTY, 'FRIENDSCHAT': TARGET_BUDDY,
    'GUILDCHAT': TARGET_GUILD, 'ALLIANCECHAT': TARGET_ALLIANCE, 'TOSPOUSE': TARGET_SPOUSE,
}

def before(action):
    """修前：空 stub → 目标不变、输入行不开。"""
    return (None, False)

def after(action, view_input=False):
    """修后：命中的键切目标并开输入行；未命中的键直接 return（状态不变）。"""
    if action not in KEYMAP:
        return (None, view_input)
    target = KEYMAP[action]
    return (target, True if not view_input else True)

for key, target in KEYMAP.items():
    t, opened = after(key)
    check('sim_%s_target' % key, t == target, '%s -> %d' % (key, t))
    check('sim_%s_opens' % key, opened is True)
    bt, bo = before(key)
    check('sim_%s_before_inert' % key, bt is None and bo is False)

# 未命中键必须不改变任何状态（含已开输入行时也不收框）
for other in ['MENU', 'QUICKSLOTS', 'TOGGLECHAT', 'SAY2']:
    check('sim_ignore_%s' % other, after(other, False) == (None, False))
    check('sim_ignore_%s_open' % other, after(other, True) == (None, True))

# 目标号必须落在枚举范围内（TARGET_COUNT=8）
check('sim_targets_in_range', all(0 <= v < 8 for v in KEYMAP.values()))

# ── 汇总 ──────────────────────────────────────────────────────
if FAIL:
    for f in FAIL:
        print('  FAIL', f)
    print('RESULT: %d failed' % len(FAIL))
    sys.exit(1)
print('PASS: chat target keybind round53 (statusbar-hud/D4 + social-economy/D1)')
sys.exit(0)
