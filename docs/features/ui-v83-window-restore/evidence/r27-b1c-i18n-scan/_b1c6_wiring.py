"""B1c-6 接线器 v2：UIMiscShells.cpp 非软键盘展示文案站点机械接进 Locale。

站点形状（与 scan_display_text.py 的尺对齐 + 尺盲区补位）：
  A. change_text("lit")                       ⇒ change_text(Locale::text("k"))
  B. change_text("lit" + var + ...)           ⇒ change_text(Locale::format/format_all("k", …))
     （值 = 字面量段逐字重建、变量段换 {}；同值 ⇒ 同键，跨窗共享归 common.*）
  C. Text(Text::Font::F, A, Color::N, "lit")  ⇒ 字面量换成 Locale::text("k")   [Text ctor 档]
  D. : tip_label(Text::Font::F, A, Color::N, "lit")   [ctor 成员初始化列表 = 尺盲区] ⇒ 同 C。
  E. 三元/跨行 change_text(cond ? (…) : "b")  [跨行 = 尺盲区] ⇒ 本脚本收不到，报告里点名，手工接。
  F. sm_tip(Color::N, "lit")（自由函数第二显示通道）⇒ 与 A/B 同法换（值同 ⇒ 键同）。

owner：花括号深度栈找 enclosing `Class::method`（返回类型 Button::State 不背锅）；
匿名 ns 自由函数按行段人工映射。
键名：小写点分（PLAN §2）；值 = 改动前字面量逐字重建（决策 B）。
"""
import io, re, json, collections

P = 'IO/UITypes/UIMiscShells.cpp'
src = io.open(P, encoding='utf-8').read()
lines = src.split('\n')

ALREADY = re.compile(r'Locale::(text|text_or|tr|format|format_all|enum_text|enum_key)\s*\(')
WL = {"-", " ~ ", "!", "V", "0", "1", "2", "0%", "0.00%"}
KEYLIKE = re.compile(r'^[a-z][a-z0-9_]*\.[a-z0-9_.]*$')
DEF = re.compile(r'\b([A-Z]\w*)::(\w+)\s*\(')
LIT_FULL = re.compile(r'^"((?:[^"\\]|\\.)*)"$')

def unescape(s):
    return s.replace(r'\"', '"').replace(r'\\', '\\')

# ---------- owner：花括号深度栈 ----------
depth = 0
stack = []
owner_at = {}
enclosing = '?'
for i, raw in enumerate(lines, 1):
    m = DEF.search(raw)
    if m and not raw.startswith(('\t', ' ')):
        enclosing = m.group(1)
    j, inq = 0, False
    while j < len(raw):
        c = raw[j]
        if inq:
            if c == '\\':
                j += 2; continue
            if c == '"':
                inq = False
        else:
            if c == '"':
                inq = True
            elif c == '{':
                stack.append(enclosing); depth += 1
            elif c == '}':
                if depth:
                    depth -= 1
                    stack.pop()
                    enclosing = stack[-1] if stack else '?'
        j += 1
    owner_at[i] = enclosing if depth else enclosing

FREE = {}
for a, b, cls in [(190, 217, 'shop'), (118, 150, 'common')]:
    for i in range(a, b + 1):
        FREE[i] = cls

def owner_of(i):
    return FREE.get(i, owner_at.get(i, '?') or '?')

# ---------- 实参扫描 ----------
def find_call_args(s, start):
    i, dep, inq, buf = start, 1, False, []
    while i < len(s):
        c = s[i]
        if inq:
            buf.append(c)
            if c == '\\':
                buf.append(s[i + 1]); i += 2; continue
            if c == '"':
                inq = False
        else:
            if c == '"':
                inq = True; buf.append(c)
            elif c == '(':
                dep += 1; buf.append(c)
            elif c == ')':
                dep -= 1
                if dep == 0:
                    return ''.join(buf), i
                buf.append(c)
            else:
                buf.append(c)
        i += 1
    return None, None

def split_plus(arg):
    parts, i, inq, buf = [], 0, False, []
    while i < len(arg):
        c = arg[i]
        if inq:
            buf.append(c)
            if c == '\\':
                buf.append(arg[i + 1]); i += 2; continue
            if c == '"':
                inq = False
        else:
            if c == '"':
                inq = True; buf.append(c)
            elif c == '+':
                parts.append(''.join(buf)); buf = []
            else:
                buf.append(c)
        i += 1
    parts.append(''.join(buf))
    return [p.strip() for p in parts]

def split_commas(arg):
    """按顶层 ',' 分段（引号/括号内不算）。sm_tip(Color, msg) 用。"""
    parts, i, inq, dep, buf = [], 0, False, 0, []
    while i < len(arg):
        c = arg[i]
        if inq:
            buf.append(c)
            if c == '\\':
                buf.append(arg[i + 1]); i += 2; continue
            if c == '"':
                inq = False
        else:
            if c == '"':
                inq = True; buf.append(c)
            elif c == '(':
                dep += 1; buf.append(c)
            elif c == ')':
                dep -= 1; buf.append(c)
            elif c == ',' and dep == 0:
                parts.append(''.join(buf)); buf = []
            else:
                buf.append(c)
        i += 1
    parts.append(''.join(buf))
    return [p.strip() for p in parts]

CT = re.compile(r'(?<!\w)change_text\(')
SM = re.compile(r'\bsm_tip\(')
TC = re.compile(r'Text::Font::(\w+)\s*,\s*[\w:]+\s*,\s*Color::Name::\w+[^;,]*,\s*"((?:[^"\\]|\\.)*)"')

sites = []
for i, raw in enumerate(lines, 1):
    handled_ct = False
    for pat, sink in ((CT, 'change_text'), (SM, 'sm_tip')):
        m = pat.search(raw)
        if not m:
            continue
        # 只看本 sink 调用后的那段是否已是 Locale::（同行别处已接不算）
        seg0 = raw[m.end():m.end() + 12]
        if ALREADY.match(seg0.lstrip()) and sink == 'change_text':
            handled_ct = True
            continue
        if sink == 'change_text':
            handled_ct = True
        arg, endi = find_call_args(raw, m.end())
        if arg is None:
            sites.append({'ln': i, 'sink': sink, 'shape': 'cross-line', 'cls': owner_of(i)})
            continue
        if sink == 'sm_tip':
            parts_all = split_commas(arg)
            parts = split_plus(parts_all[1])  # msg 段内部还可能拼变量
            lits = [(LIT_FULL.match(p).group(1) if LIT_FULL.match(p) else None) for p in parts]
        else:
            parts = parts_all = split_plus(arg)
            lits = [(LIT_FULL.match(p).group(1) if LIT_FULL.match(p) else None) for p in parts]
        disp = [unescape(x) for x in lits if x is not None]
        disp = [x for x in disp if x not in WL and not KEYLIKE.match(x) and x != '']
        if not disp:
            continue
        sites.append({'ln': i, 'sink': sink, 'shape': 'call', 'parts': parts, 'lits': lits,
                      'pre_arg': parts_all[0] + ', ' if sink == 'sm_tip' else '',
                      'cls': owner_of(i), 'arg_start': m.end(), 'arg_end': endi})
    if handled_ct:
        continue  # 一行不会同时有 Text ctor 要接（UIContextMenu 的 Text(...) 是构造临时件，单独行）
    for m in TC.finditer(raw):
        t = unescape(m.group(2))
        if t in WL or KEYLIKE.match(t) or t == '':
            continue
        sites.append({'ln': i, 'sink': 'Text ctor', 'shape': 'tc', 'lit': t,
                      'cls': owner_of(i), 'lit_span': (m.start(2) - 1, m.end(2) + 1)})

# ---------- 键分配：两遍（先归集每值的 owner 集，再命名），5b 先例 = 同值跨窗各窗留自己的键 ----------
used = set()
keys = []
value_owners = collections.defaultdict(set)
value_order = []

def slug(v):
    core = re.sub(r'[^a-z0-9]+', '_', v.lower()).strip('_')
    return (core[:44] or 'txt')

for s in sites:
    if s['shape'] == 'cross-line':
        continue
    v = s['lit'] if s['shape'] == 'tc' else ''.join(
        unescape(l) if l is not None else '{}' for p, l in zip(s['parts'], s['lits']))
    s['value'] = v
    if v not in value_owners:
        value_order.append(v)
    cls = (s['cls'] or 'misc').lower()
    if cls.startswith('ui') and len(cls) > 2:
        cls = cls[2:]          # 5b 先例：UIAdmin → admin（键前缀不带 UI）
    if cls in ('?', 'misc'):
        cls = 'misc'
    value_owners[v].add(cls)

value2key = {}
for v in value_order:
    owners = sorted(value_owners[v])
    hint = slug(v)
    if len(owners) == 1:
        cand = ['%s.%s' % (owners[0], hint)]
    else:
        cand = ['common.%s' % hint] + ['%s.%s' % (o, hint) for o in owners]
    picked = None
    for c in cand:
        if c not in used and not any(k == c for k, _ in keys):
            picked = c
            break
    if picked is None:
        picked = cand[0]
        n = 2
        while any(k == picked for k, _ in keys):
            picked = '%s_%d' % (cand[0], n); n += 1
    used.add(picked)
    value2key[v] = picked
    keys.append((picked, v))

edits = []
for s in sites:
    if s['shape'] == 'cross-line':
        continue
    key = value2key[s['value']]
    s['key'] = key
    if s['shape'] == 'tc':
        edits.append((s['ln'], s['lit_span'][0], s['lit_span'][1], 'Locale::text("%s")' % key))
    else:
        v = s['value']
        var_exprs = [p for p, l in zip(s['parts'], s['lits']) if l is None]
        nv = v.count('{}')
        if nv == 0:
            call = 'Locale::text("%s")' % key
        elif nv == 1 and len(var_exprs) == 1:
            call = 'Locale::format("%s", %s)' % (key, var_exprs[0])
        else:
            call = 'Locale::format_all("%s", {%s})' % (key, ', '.join(var_exprs))
        call = s.get('pre_arg', '') + call
        edits.append((s['ln'], s['arg_start'], s['arg_end'], call))

io.open(P + '.bak', 'w', encoding='utf-8', newline='').write(src)
byline = collections.defaultdict(list)
for ln, a, b, rep in edits:
    byline[ln].append((a, b, rep))
out = list(lines)
n = 0
for ln, ed in byline.items():
    s = out[ln - 1]
    for a, b, rep in sorted(ed, reverse=True):
        s = s[:a] + rep + s[b:]
        n += 1
    out[ln - 1] = s
io.open(P, 'w', encoding='utf-8', newline='\n').write('\n'.join(out))

trim_warn = [(k, v) for k, v in keys if v != v.strip()]
with io.open('_b1c6_keys.txt', 'w', encoding='utf-8', newline='\n') as f:
    f.write('# R27-B1c-6 建键表：%d 键（值=原字面量逐字重建，决策 B 不翻译）\n' % len(keys))
    for k, v in keys:
        f.write('%s=%s\n' % (k, v))
    for k, v in trim_warn:
        f.write('# TRIM-WARN %s=%r\n' % (k, v))

print('sites=%d edits=%d keys=%d' % (len(sites), n, len(keys)))
print('cross-line(手工):', [s['ln'] for s in sites if s['shape'] == 'cross-line'])
print('sm_tip 行:', [s['ln'] for s in sites if s.get('sink') == 'sm_tip'])
print('trim_warn:', trim_warn)
json.dump(sites, io.open('_b1c6_sites.json', 'w', encoding='utf-8'), ensure_ascii=False, default=str)
