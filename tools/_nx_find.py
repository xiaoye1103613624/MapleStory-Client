import importlib.util
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx')

hits = []
def walk(i, path):
    name = nx.name(i)
    p = path + '/' + name
    low = p.lower()
    if 'equip' in low and 'tooltip' not in low:
        t = nx.node(i)
        extra = ''
        if t[3] == 5:
            extra = 'bitmap %dx%d' % (t[5], t[6])
        hits.append((p, 'type=%d children=%d %s' % (t[3], t[2], extra)))
    for c in nx.children(i):
        walk(c, p)

for root in nx.children(0):
    walk(root, '')

for h in hits[:80]:
    print(h[0], h[1])
print('total:', len(hits))
