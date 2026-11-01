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

for sid in ['0000008', '0000012', '0000100', '0001000', '0001001', '0001002', '0001003', '0009000']:
    n = find('Skill.img/' + sid)
    if n is None:
        print(sid, '-> (no node)')
        continue
    out = []
    for c in nx.children(n):
        if nx.name(c) == 'name':
            t = nx.node(c)
            s = nx.strings[t[4]]
            out.append('name=' + s.encode('latin-1', 'replace').decode('gbk', 'replace'))
    print(sid, '->', '; '.join(out) if out else '(no name)')
