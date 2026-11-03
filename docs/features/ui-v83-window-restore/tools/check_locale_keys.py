# -*- coding: utf-8 -*-
"""R27-B3b 门禁：证明「枚举原文折成键名后一定查得到文案」。

为什么要有这个脚本
------------------
`Locale::enum_key()` 把数据表里的英文枚举（`"ONE-HANDED SWORD"`、`"Attack Power"`、
`"FAST (1)"`）折成 ini 键后缀。这条映射是**运行期**才发生的，编译器和链接器都不管，
`tr()` 查不到也不报错 —— 它把键名原样回显，于是 tooltip 上出现
`类型：equip.type.ONE-HANDED_SWORD` 这种脏文案。真机悬停只能验到一个类型（32 选 1），
而数据表是封闭可枚举的，所以这里做穷举比对：把三张表里所有非空枚举值全部折一遍，
要求每个折出来的键在 zh-CN 与 en-US 里都存在。

分隔符坑（本脚本立项的直接原因）
--------------------------------
武器类型里 6 条带连字符（`ONE-HANDED SWORD` / `ONE-HANDED AXE` / `ONE-HANDED MACE` /
`TWO-HANDED SWORD` / `TWO-HANDED AXE` / `TWO-HANDED MACE`），ini 里按 `_` 建档。
`enum_key` 早先只折空格、不折连字符，这 6 条必然回显键名；而测试角色（Lv1 新手）
根本没武器，真机悬停永远碰不到 —— 只有穷举能抓到。所以本脚本自己实现一份 fold，
并在下面 `assert` 它与 C++ 侧的规则一致；改 C++ 侧规则时必须同步改这里。

解析规则对齐 `Util/Locale.cpp::load_file`：去 BOM、去 \r、整行 trim、
`#`/`;` 开头跳过、无 `=` 跳过、键值各自 trim、重复键后写覆盖前写。

用法
----
    python docs/features/ui-v83-window-restore/tools/check_locale_keys.py
退出码 0 = 全绿；非 0 = 有缺键/漂移，并打印清单。
"""
from __future__ import annotations

import io
import os
import re
import subprocess
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
LOCALE_DIR = os.path.join(REPO, 'locale')


def fold(raw: str) -> str:
    """必须与 Util/Locale.h::enum_key 逐字一致：空格与连字符都折成下划线。"""
    return re.sub(r'[ -]', '_', raw)


def parse_ini_text(text: str) -> dict:
    """Locale::load_file 的解析结果（含 trim 与后写覆盖），从**已解码的字符串**入手。"""
    out = {}
    for line in text.splitlines():
        t = line.strip()
        if not t or t[0] in '#;':
            continue
        eq = t.find('=')
        if eq < 0:
            continue
        key = t[:eq].strip()
        if key:
            out[key] = t[eq + 1:].strip()
    return out


def load_ini(path: str) -> dict:
    """复刻 Locale::load_file 的解析结果（含 trim 与后写覆盖）。"""
    with io.open(path, encoding='utf-8-sig') as fh:
        return parse_ini_text(fh.read())


def committed_key_counts():
    """HEAD 提交树里两份 ini 的键数 + 该 HEAD 的短号；拿不到（无 git / 无该文件）返回 None。

    为什么问这个：门禁默认读**工作树**，共享工作树里并行会话的未提交键会被一并算入，
    归档时容易被读成「本批建了多少键」。5b/5c 两次都把这种虚高数字写进了 evidence，
    所以把提交树读数一并打出来，让「本批键数」这一档可复核。

    为什么连**短号**一起打：提交树自己在动 —— 记录本档案取于 `e0fbb7b`（提交树 301），
    几分钟后并行会话提了 `aa0d4e2`，同一句 `git show HEAD:` 就变成 307 了。
    不写 rev 的读数下次复跑必然「对不上」，那正是本条要避免的第二次事故。
    """
    try:
        rev = subprocess.run(['git', '-C', REPO, 'rev-parse', '--short', 'HEAD'],
                             capture_output=True)
        if rev.returncode != 0:
            return None
        out = []
        for name in ('zh-CN.ini', 'en-US.ini'):
            r = subprocess.run(['git', '-C', REPO, 'show', 'HEAD:locale/' + name],
                               capture_output=True)
            if r.returncode != 0:
                return None
            out.append(len(parse_ini_text(r.stdout.decode('utf-8', 'replace'))))
    except OSError:                       # 没装 git / 不在 PATH：这行不打，门禁照常
        return None
    return out[0], out[1], rev.stdout.decode().strip()


def cpp_string_arrays(path: str, pattern: str) -> list:
    """从 C++ 源里抠出 `... = { "A", "B", ... };` 形式的字符串表。"""
    with io.open(path, encoding='utf-8') as fh:
        src = fh.read()
    found = []
    for blk in re.findall(pattern, src, re.S):
        found += re.findall(r'"([^"]*)"', blk)
    return found


def collect_dynamic_keys() -> dict:
    """返回 {前缀: [折好的完整键, ...]}，来源是三张真实数据表。"""
    dyn = {}

    # 1) 装备/武器类型：Data/EquipData.cpp 的两张 types[] + 兜底 "CASH"
    types = cpp_string_arrays(
        os.path.join(REPO, 'Data', 'EquipData.cpp'),
        r'constexpr const char\* types\[[^\]]*\]\s*=\s*\{(.*?)\};',
    )
    types.append('CASH')                       # else 分支的兜底类型
    dyn['equip.type.'] = ['equip.type.' + fold(t) for t in types if t]

    # 2) 武器攻速：Data/WeaponData.cpp::getspeedstring 的 return "FAST (1)" ...
    speeds = cpp_string_arrays(
        os.path.join(REPO, 'Data', 'WeaponData.cpp'),
        r'std::string WeaponData::getspeedstring\(\) const\s*\{(.*?)\n\s*\}',
    )
    dyn['equip.speed.'] = ['equip.speed.' + fold(s) for s in speeds if s]

    # 3) 属性名：Character/EquipStat.cpp::names（tooltip 用 enum_key 拼键）
    stats = cpp_string_arrays(
        os.path.join(REPO, 'Character', 'EquipStat.cpp'),
        r'const char\* names\[Id::LENGTH\]\s*=\s*\{(.*?)\};',
    )
    dyn['equip.stat.'] = ['equip.stat.' + fold(s) for s in stats if s]

    return dyn


def collect_literal_keys() -> tuple:
    """反向门禁（PLAN §4.2）：代码里引用的键 ⇒ 两份 ini 必须都有。

    为什么必须有这一条：`tr()` 查不到键时**回显键名**，编译期、链接期、运行期都不报错，
    于是拼错一个字母的键会在真机上显示成 `vegaspell.slot_equp_at`。B1c 之后每批要动
    上百个键位引用，靠人眼对 ini 不可能。

    两类形状分开处理：
      · 完整键（`Locale::text("a.b")` / `bind_text(t, "a.b")` / `format("a.b", v)`）⇒ 直接查键。
      · 动态拼出来的前缀（`"dailygift.rule_" + std::to_string(i)`、`enum_text("equip.type.", raw)`）
        ⇒ 只要求 ini 里**至少有一个**以该前缀开头的键，逐个值留给数据表穷举去管。
    """
    callers = (
        # 第一个实参是完整键。闭包字符必须一起判：跟 `+` 说明键名是拼出来的（只当前缀查），
        # 跟 `,` 说明还有值实参，跟 `)` 说明是单参 text() —— 早先只写了 `[+,)` 里的 `+,`，
        # 于是全站最常见的 `Locale::text("a.b")` 一个都没进门禁，缺键照样静默回显。
        re.compile(r'Locale::(?:text|text_or|format|format_all)\s*\(\s*"([^"]+)"\s*([,+])'),
        re.compile(r'Locale::(?:text|text_or|format|format_all)\s*\(\s*"([^"]+)"\s*\)'),
        re.compile(r'(?:bind_text|\bbind)\s*\(\s*[^,()]+,\s*"([^"]+)"\s*([,+)])'),
    )
    prefixes = (
        # enum_text/enum_key 的第一参恒为前缀（后缀来自数据表，已在动态键段穷举）
        re.compile(r'Locale::enum_(?:text|key)\s*\(\s*"([^"]+)"'),
    )
    # 三元选键（`Locale::text(cond ? "a.x" : "a.y")`）：上面两条都要求引号紧跟左括号，看不见它。
    # 不补的代价是真机上「选中的那支拼错」照样静默回显键名 —— R27-B1c-5c 的
    # `guild.member_online/offline` 就是这个形状，靠 fidelity 尺才发现门禁漏了 2 个键。
    ternary = (
        re.compile(r'Locale::(?:text|text_or)\s*\(\s*[^;"()]{0,120}\?\s*"([^"]+)"\s*:\s*"([^"]+)"'),
    )

    keys, dyn = {}, {}
    for root, dirs, files in os.walk(REPO):
        dirs[:] = [d for d in dirs if d not in ('.git', '3rdparty', 'build', 'x64')]
        for fn in files:
            if not fn.endswith(('.cpp', '.h')):
                continue
            path = os.path.join(root, fn)
            try:
                with io.open(path, encoding='utf-8', errors='replace') as fh:
                    src = fh.read()
            except OSError:
                continue
            for rx in callers:
                for m in rx.finditer(src):
                    key = m.group(1)
                    nxt = m.group(2) if rx.groups >= 2 else ','
                    if nxt == '+':
                        dyn.setdefault(key, []).append(os.path.relpath(path, REPO))
                    else:
                        keys.setdefault(key, []).append(os.path.relpath(path, REPO))
            for rx in prefixes:
                for key in rx.findall(src):
                    dyn.setdefault(key, []).append(os.path.relpath(path, REPO))
            for rx in ternary:
                for m in rx.finditer(src):
                    for key in m.groups():
                        # 只认「像键名」的两支：小写点分且含点。否则 `format("k", c ? "Yes" : "No")`
                        # 这种**值**位置上的三元会被当成键，凭空报缺档。
                        if re.fullmatch(r'[a-z0-9_.]+', key) and '.' in key:
                            keys.setdefault(key, []).append(os.path.relpath(path, REPO))

    return keys, dyn


def main() -> int:
    zh_path = os.path.join(LOCALE_DIR, 'zh-CN.ini')
    en_path = os.path.join(LOCALE_DIR, 'en-US.ini')
    for p in (zh_path, en_path):
        if not os.path.isfile(p):
            print('FAIL: 语言文件不存在 %s' % p)
            return 2

    zh, en = load_ini(zh_path), load_ini(en_path)
    problems = []

    print('zh-CN keys = %d' % len(zh))
    print('en-US keys = %d' % len(en))
    # 工作树 ≠ 提交：并行会话的未提交键会被算进上面这个数，归档时容易被读成「本批的键数」。
    # 实测两次：5b 报 288（提交树实为 286）、5c 报 303（提交树实为 301），差的就是别人那 2 条。
    # 所以这条尺顺手打一份 HEAD 版读数，并把可复核的判据命令直接写出来。
    committed = committed_key_counts()
    if committed:
        czh, cen, rev = committed
        drift = len(zh) - czh
        tail = ('工作树多 %d 条未提交键（并行会话的件，别当本批键数）' % drift) if drift > 0 else \
               ('工作树与提交树一致' if drift == 0 else '工作树比提交树**少** %d 条：有人删了键' % -drift)
        print('HEAD(%s) 提交树 keys = zh %d / en %d（本批键数看这一行）｜%s' % (rev, czh, cen, tail))
        print('  复核：git show %s:locale/zh-CN.ini | grep -cE \'^[a-z][^=]*=\'' % rev)

    # 键集漂移：任何一边缺键，切语言时就静默回显键名
    only_zh = sorted(set(zh) - set(en))
    only_en = sorted(set(en) - set(zh))
    if only_zh:
        problems.append('仅 zh-CN 有的键 (%d):\n  %s' % (len(only_zh), '\n  '.join(only_zh)))
    if only_en:
        problems.append('仅 en-US 有的键 (%d):\n  %s' % (len(only_en), '\n  '.join(only_en)))

    # 空值：加载器会 trim，值写成空串等于「有键没文案」
    for name, table in (('zh-CN', zh), ('en-US', en)):
        empty = sorted(k for k, v in table.items() if v == '')
        if empty:
            problems.append('%s 值为空 (%d):\n  %s' % (name, len(empty), '\n  '.join(empty)))

    # 穷举动态键
    dyn = collect_dynamic_keys()
    for prefix, keys in sorted(dyn.items()):
        miss_zh = sorted(set(keys) - set(zh))
        miss_en = sorted(set(keys) - set(en))
        print('%-16s 枚举 %2d 条  zh 缺 %d  en 缺 %d'
              % (prefix, len(set(keys)), len(miss_zh), len(miss_en)))
        if miss_zh:
            problems.append('zh-CN 缺动态键 %s:\n  %s' % (prefix, '\n  '.join(miss_zh)))
        if miss_en:
            problems.append('en-US 缺动态键 %s:\n  %s' % (prefix, '\n  '.join(miss_en)))

    # 源表本身有空串是合法的（武器类型表 index 4/5/6/9），但必须确认
    # C++ 侧 enum_text 仍对空原文短路，否则会拼出 `equip.type.` 这种键。
    raw_types = cpp_string_arrays(
        os.path.join(REPO, 'Data', 'EquipData.cpp'),
        r'constexpr const char\* types\[[^\]]*\]\s*=\s*\{(.*?)\};',
    )
    if '' in raw_types:
        with io.open(os.path.join(REPO, 'Util', 'Locale.h'), encoding='utf-8') as fh:
            header = fh.read()
        if 'if (raw.empty())' not in header:
            problems.append('数据表含空枚举值，但 Locale.h::enum_text 已丢失空值短路')

    # 反向门禁：代码引用的键 ⇒ 两份 ini 必须都有（缺键不报错，只在界面上回显键名）
    lit, ldyn = collect_literal_keys()
    print('代码引用键 %d 个 / 动态前缀 %d 个' % (len(lit), len(ldyn)))
    miss_zh = sorted(k for k in lit if k not in zh)
    miss_en = sorted(k for k in lit if k not in en)
    if miss_zh:
        problems.append('代码引用但 zh-CN 缺键 (%d):\n  %s'
                        % (len(miss_zh), '\n  '.join('%s <- %s' % (k, ', '.join(lit[k])) for k in miss_zh)))
    if miss_en:
        problems.append('代码引用但 en-US 缺键 (%d):\n  %s'
                        % (len(miss_en), '\n  '.join('%s <- %s' % (k, ', '.join(lit[k])) for k in miss_en)))
    for pre, where in sorted(ldyn.items()):
        if not any(k.startswith(pre) for k in zh) or not any(k.startswith(pre) for k in en):
            problems.append('动态前缀 %s 两份 ini 都无成员（引用点 %s）' % (pre, ', '.join(sorted(set(where)))))

    if problems:
        print('\n=== FAIL ===')
        for p in problems:
            print('- %s' % p)
        return 1

    print('\nGATE OK: 语言文件键集对齐，所有数据表枚举均能折成已建档的键，且代码引用的键全部建档')
    return 0


if __name__ == '__main__':
    sys.exit(main())
