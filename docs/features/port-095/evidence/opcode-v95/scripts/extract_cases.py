import re, json, os, sys


def strip_comments(s):
    return re.sub(r'/\*0x[0-9a-fA-F]+\*/', '', s)


class P:
    def __init__(self, src):
        self.s = strip_comments(src)
        self.n = len(self.s)

    def skip(self, i):
        while i < self.n and self.s[i] in ' \t\r\n':
            i += 1
        return i

    def paren(self, i):
        depth = 0
        j = i
        while j < self.n:
            if self.s[j] == '(':
                depth += 1
            elif self.s[j] == ')':
                depth -= 1
                if depth == 0:
                    return self.s[i + 1:j].strip(), j + 1
            j += 1
        raise RuntimeError('paren')

    def block(self, i):
        i = self.skip(i)
        if self.s[i] == '{':
            i += 1
            stmts = []
            while True:
                i = self.skip(i)
                if self.s[i] == '}':
                    return stmts, i + 1
                st, i = self.stmt(i)
                if st is not None:
                    stmts.append(st)
        st, i = self.stmt(i)
        return ([st] if st else []), i

    def switch(self, i):
        i += 1
        groups = []
        default = []
        pending = []
        cur = None
        while True:
            i = self.skip(i)
            if i >= self.n:
                return ('switch', groups, default), i
            if self.s[i] == '}':
                if cur is not None:
                    groups.append((pending, cur))
                return ('switch', groups, default), i + 1
            m = re.match(r'case\s+(-?\d+)\s*:', self.s[i:])
            if m:
                if cur is not None:
                    groups.append((pending, cur))
                    pending = []
                pending.append(int(m.group(1)))
                cur = []
                i += m.end()
                continue
            if self.s.startswith('default', i):
                i = self.skip(i + 7)
                if self.s[i] == ':':
                    i += 1
                if cur is not None:
                    groups.append((pending, cur))
                    pending = []
                cur = None
                body = []
                while True:
                    i = self.skip(i)
                    if i >= self.n or self.s[i] == '}':
                        break
                    if re.match(r'case\s+-?\d+\s*:', self.s[i:]):
                        break
                    st, i = self.stmt(i)
                    if st is not None:
                        body.append(st)
                default = body
                continue
            if cur is None:
                cur = []
            st, i = self.stmt(i)
            if st is not None and not (st[0] == 'stmt' and st[1].strip() == 'break;'):
                cur.append(st)

    def stmt(self, i):
        i = self.skip(i)
        m = re.match(r'LABEL_\d+:', self.s[i:])
        if m:
            return None, i + m.end()
        if self.s.startswith('if', i) and self.s[i + 2] in ' (':
            c, j = self.paren(self.skip(i + 2))
            th, j = self.block(j)
            j2 = self.skip(j)
            els = []
            if self.s.startswith('else', j2):
                els, j2 = self.block(self.skip(j2 + 4))
            return ('if', c, th, els), j2
        if self.s.startswith('switch', i):
            c, j = self.paren(self.skip(i + 6))
            j = self.skip(j)
            node, j2 = self.switch(j)
            return node, j2
        if self.s[i] == '{':
            b, j = self.block(i)
            return ('block', b), j
        j = self.s.find(';', i)
        if j < 0:
            j = self.n
        return ('stmt', self.s[i:j + 1].strip()), j + 1


def cond_range(c):
    c = c.strip()
    m = re.match(r'\(unsigned int\)\(nType - (-?\d+)\) > (0x[0-9a-fA-F]+|\d+)$', c)
    if m:
        x = int(m.group(1))
        y = int(m.group(2), 0)
        return ('not', (x, x + y))
    m = re.match(r'\(unsigned int\)\(nType - (-?\d+)\) <= (0x[0-9a-fA-F]+|\d+)$', c)
    if m:
        x = int(m.group(1))
        y = int(m.group(2), 0)
        return ('in', (x, x + y))
    m = re.match(r'nType == (-?\d+)$', c)
    if m:
        return ('eq', int(m.group(1)))
    m = re.match(r'nType > (-?\d+)$', c)
    if m:
        return ('gt', int(m.group(1)))
    m = re.match(r'nType <= (-?\d+)$', c)
    if m:
        return ('le', int(m.group(1)))
    return ('unknown', c)


def negate(r):
    k = r[0]
    if k == 'in':
        return ('not', r[1])
    if k == 'not':
        return ('in', r[1])
    if k == 'eq':
        return ('ne', r[1])
    if k == 'ne':
        return ('eq', r[1])
    if k == 'gt':
        return ('le', r[1])
    if k == 'le':
        return ('gt', r[1])
    return ('unknown', 'X')


def inter(ivs, r, truth=True):
    if not truth:
        r = negate(r)
    out = []
    for lo, hi in ivs:
        k = r[0]
        if k == 'in':
            a, b = r[1]
            l = max(lo, a)
            h = min(hi, b)
            if l <= h:
                out.append((l, h))
        elif k == 'not':
            a, b = r[1]
            if lo < a:
                out.append((lo, min(hi, a - 1)))
            if hi > b:
                out.append((max(lo, b + 1), hi))
        elif k == 'eq':
            v = r[1]
            if lo <= v <= hi:
                out.append((v, v))
        elif k == 'ne':
            v = r[1]
            if lo == hi == v:
                pass
            else:
                if lo <= v - 1 <= hi:
                    out.append((lo, v - 1))
                if lo <= v + 1 <= hi:
                    out.append((v + 1, hi))
        elif k == 'gt':
            l = max(lo, r[1] + 1)
            if l <= hi:
                out.append((l, hi))
        elif k == 'le':
            h = min(hi, r[1])
            if lo <= h:
                out.append((lo, h))
        else:
            out.append((lo, hi))
    return out


HELPERS = ('CInPacket::', 'CMSException::', 'ZAllocEx::', 'StringPool::',
           'ZXString', '_CxxThrowException', 'CUserPool::GetUser', '::Alloc',
           'CClientSocket::', 'CWvsApp::')


def callname(txt):
    for m in re.finditer(r'([A-Za-z_]\w*)::(\w+)\s*\(', txt):
        full = m.group(1) + '::' + m.group(2)
        if any(h in full for h in HELPERS):
            continue
        if m.group(1) == m.group(2):
            continue
        return full
    return None


def walk(stmts, ivs, out):
    for s in stmts:
        if s[0] == 'if':
            _, c, th, els = s
            r = cond_range(c)
            t = inter(ivs, r, True)
            if t:
                walk(th, t, out)
            f = inter(ivs, r, False)
            if f and els:
                walk(els, f, out)
        elif s[0] == 'block':
            walk(s[1], ivs, out)
        elif s[0] == 'switch':
            vals = [v for vs, _ in s[1] for v in vs]
            for vs, body in s[1]:
                for v in vs:
                    sub = [(max(lo, v), min(hi, v)) for lo, hi in ivs if lo <= v <= hi]
                    if sub:
                        walk(body, sub, out)
            rest = []
            for lo, hi in ivs:
                cur = lo
                for v in sorted(vals):
                    if v < cur or v > hi:
                        continue
                    if cur <= v - 1:
                        rest.append((cur, v - 1))
                    cur = max(cur, v + 1)
                if cur <= hi:
                    rest.append((cur, hi))
            if s[2] and rest:
                walk(s[2], rest, out)
        elif s[0] == 'stmt':
            nm = callname(s[1])
            if nm:
                for lo, hi in ivs:
                    out.append((lo, hi, nm))


def extract(path):
    src = open(path, encoding='utf-8').read()
    body = src[src.index('{'):]
    stmts, _ = P(body).block(0)
    out = []
    walk(stmts, [(0, 65535)], out)
    out.sort()
    merged = []
    for lo, hi, nm in out:
        if merged and merged[-1][1] + 1 == lo and merged[-1][2] == nm:
            merged[-1] = (merged[-1][0], hi, nm)
        else:
            merged.append((lo, hi, nm))
    return merged


if __name__ == '__main__':
    res = {}
    for f in sorted(os.listdir('dec')):
        if not f.endswith('.c'):
            continue
        try:
            res[f] = extract(os.path.join('dec', f))
        except Exception as ex:
            res[f] = ['ERR: %s' % ex]
    json.dump(res, open('dec_maps.json', 'w'), indent=1)
    for f, m in res.items():
        print('==', f, len(m))
        if m and isinstance(m[0], str):
            print('  ', m[0])
            continue
        for lo, hi, nm in m[:100]:
            print('  %5d-%-5d %s' % (lo, hi, nm))
