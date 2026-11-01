import importlib.util, sys, io, struct
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

for jid in ['000', '100', '200', '300', '400', '500', '1000', '2000']:
    n = find('Skill.img/' + jid)
    if n is None:
        print(jid, '-> (no node)')
        continue
    for c in nx.children(n):
        if nx.name(c) == 'bookName':
            print(jid, 'bookName =', nx.value(c))
