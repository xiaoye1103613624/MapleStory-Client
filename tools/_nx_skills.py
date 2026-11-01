import importlib.util, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/Skill.nx')

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

node = find('000.img/skill')
print('000.img/skill children:')
for c in nx.children(node):
    print(' ', nx.name(c))
    # check invisible
    for cc in nx.children(c):
        if nx.name(cc) == 'invisible':
            print('    invisible =', nx.value(cc))
