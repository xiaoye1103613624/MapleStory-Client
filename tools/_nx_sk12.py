import importlib.util
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)

skill = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/Skill.nx')
out = []


def find(nx, path):
    i = 0
    for part in path.split('/'):
        hit = None
        for c in nx.children(i):
            if nx.name(c) == part:
                hit = c
                break
        if hit is None:
            return None
        i = hit
    return i


def tree(nx, i, depth, lines, maxd=4):
    lines.append('  ' * depth + nx.name(i) if depth else nx.name(i))
    if depth >= maxd:
        return
    for c in nx.children(i):
        tree(nx, c, depth + 1, lines, maxd)


for sid in ('0000012', '0001001'):
    root = find(skill, sid + '.img')
    out.append('=== %s.img -> %s' % (sid, 'FOUND' if root is not None else 'MISSING'))
    if root is not None:
        lines = []
        tree(skill, root, 0, lines, 3)
        out.extend(lines[:60])

with open('E:/project/_skill12.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
print('ok')
