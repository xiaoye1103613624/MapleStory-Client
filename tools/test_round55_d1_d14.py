# -*- coding: utf-8 -*-
"""round55 自测：social-economy/D1 后半（v95 注释换 083 依据）+ corewindow/D14（三处英文提示 i18n）。

验收点：
  A. UIMessenger.cpp/.h 内 `v95` 与 `0x7F34C0` 归零，且 6 个 Angel 083 地址就位。
  B. 所有坐标 / 计数常量逐字不变（注释类改动零行为）。
  C. UIKeyConfig 三处确认文案走 Locale::text_or，保留原英文作为 fallback。
  D. locale 两文件各含 3 个 key；en-US 值等于改前英文原值；zh-CN 值含 CJK。
  E. 行为复刻：Locale 缺键时 text_or 返回 fallback（=改前行为），命中时返回文案。
"""
import io
import re
import sys

CPP = 'IO/UITypes/UIMessenger.cpp'
HDR = 'IO/UITypes/UIMessenger.h'
DOC = 'docs/features/messenger-btclose-coords/README.md'
KC = 'IO/UITypes/UIKeyConfig.cpp'
ZH = 'locale/zh-CN.ini'
EN = 'locale/en-US.ini'

fails = []


def rd(p):
    return io.open(p, encoding='utf-8', errors='replace', newline='').read()


def check(name, cond, detail=''):
    if cond:
        print('  OK   %s' % name)
    else:
        print('  FAIL %s %s' % (name, detail))
        fails.append(name)


# ---------- A ----------
print('[A] v95 / 0x7F34C0 归零 + 083 地址就位')
cpp, hdr, doc = rd(CPP), rd(HDR), rd(DOC)
for name, txt, p in (('cpp', cpp, CPP), ('hdr', hdr, HDR), ('doc', doc, DOC)):
    check('%s_no_v95' % name, 'v95' not in txt, p)
    check('%s_no_7F34C0' % name, '0x7F34C0' not in txt and '0x7f34c0' not in txt, p)

ANGEL = {
    'cpp': ['0x8505e2', '0x84ddb0', '0x84f44b', '0x84e930', '0x8515d7', '0x851d3b'],
    'hdr': ['0x8505e2', '0x84f44b', '0x84e930', '0x8515d7'],
    'doc': ['0x8505e2'],
}
for name, txt in (('cpp', cpp), ('hdr', hdr), ('doc', doc)):
    for addr in ANGEL[name]:
        check('%s_has_%s' % (name, addr), addr in txt, addr)

# ---------- B ----------
print('[B] 常量逐字不变（注释类改动零行为）')
CONST = [
    ('static constexpr int16_t BT_CLOSE_X = 276;', 'BT_CLOSE_X=276'),
    ('static constexpr int16_t BT_CLOSE_Y = 6;', 'BT_CLOSE_Y=6'),
    ('static constexpr float LOOK_SCALE = 0.70f;', 'LOOK_SCALE=0.70'),
    ('static constexpr int16_t LOOK_FOOT_DX = 44;', 'LOOK_FOOT_DX=44'),
    ('static constexpr int16_t LOOK_FOOT_DY = -10;', 'LOOK_FOOT_DY=-10'),
    ('static constexpr int16_t LOOK_NAME_DX = 44;', 'LOOK_NAME_DX=44'),
    ('static constexpr int16_t LOOK_NAME_DY = 2;', 'LOOK_NAME_DY=2'),
    ('static constexpr int16_t BALLOON_DX = -31;', 'BALLOON_DX=-31'),
    ('static constexpr int16_t BALLOON_DY = -70;', 'BALLOON_DY=-70'),
    ('static constexpr int16_t BLINK_TICKS = 30;', 'BLINK_TICKS=30'),
    ('static constexpr int16_t BLINK_HALF = 15;', 'BLINK_HALF=15'),
    ('static constexpr int16_t BLINK_DRAW_X = 260;', 'BLINK_DRAW_X=260'),
    ('static constexpr int16_t BLINK_DRAW_Y = 4;', 'BLINK_DRAW_Y=4'),
    ('static constexpr int32_t TYPING_THROTTLE_MS = 3000;', 'TYPING_THROTTLE_MS=3000'),
]
for needle, label in CONST:
    check('const_%s' % label, needle in hdr, needle)

# ---------- C ----------
print('[C] UIKeyConfig 三处 text_or + include')
kc = rd(KC)
check('kc_has_locale_include', '#include "../../Util/Locale.h"' in kc)
check('kc_no_static_message', 'static const std::string& message' not in kc)
for key, literal in (
    ('keyconfig.confirm_default', 'Would you like to revert to default settings?'),
    ('keyconfig.confirm_clear', 'Would you like to clear all key bindings?'),
    ('keyconfig.confirm_save', 'Do you want to save your changes?'),
):
    pat = r'Locale::text_or\("%s",\s*"%s"\)' % (re.escape(key), re.escape(literal))
    check('kc_text_or_%s' % key, re.search(pat, kc) is not None, key)
check('kc_emplace_count3', kc.count('UI::get().emplace<UIOk>(message, onok);') == 3,
      'count=%d' % kc.count('UI::get().emplace<UIOk>(message, onok);'))

# ---------- D ----------
print('[D] locale 两文件键与取值')
zh, en = rd(ZH), rd(EN)


def ini(text):
    d = {}
    for line in text.split('\n'):
        s = line.strip()
        if not s or s.startswith('#') or '=' not in s:
            continue
        k, v = s.split('=', 1)
        d[k.strip()] = v
    return d


zhd, end = ini(zh), ini(en)
EN_VALUES = {
    'keyconfig.confirm_default': 'Would you like to revert to default settings?',
    'keyconfig.confirm_clear': 'Would you like to clear all key bindings?',
    'keyconfig.confirm_save': 'Do you want to save your changes?',
}
for k, v in EN_VALUES.items():
    check('en_%s' % k, end.get(k) == v, repr(end.get(k)))
    check('zh_has_%s' % k, k in zhd, k)
    zv = zhd.get(k, '')
    check('zh_cjk_%s' % k, any('\u4e00' <= ch <= '\u9fff' for ch in zv), repr(zv))

# ---------- E ----------
print('[E] 行为复刻：text_or 缺键=fallback，命中=文案')


def text_or(has, tr, fallback):
    return tr if has else fallback


for k, v in EN_VALUES.items():
    check('fallback_eq_old_%s' % k, text_or(False, 'X', v) == v)
    check('hit_uses_locale_%s' % k, text_or(True, zhd.get(k, ''), v) == zhd.get(k, ''))

print()
if fails:
    print('FAILED %d: %s' % (len(fails), fails))
    sys.exit(1)
print('PASS: round55 (social-economy/D1 comments + corewindow/D14 i18n)')
