import importlib.util, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')

def find(path):
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

for sid in ['0001000', '0001001', '0001002', '0000008', '0000012', '0000100']:
    n = find('Skill.img/000/' + sid)
    if n is None:
        print(sid, '-> (no string node)')
        continue
    parts = []
    for c in nx.children(n):
        parts.append('%s=%s' % (nx.name(c), nx.value(c)))
    print(sid, '->', '; '.join(parts))
