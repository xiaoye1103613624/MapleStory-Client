import importlib.util, sys
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)

out = open('e:/project/_fontscan.txt', 'w', encoding='utf-8')
for f in ['UI', 'Basic']:
    try:
        nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/%s.nx' % f)
    except Exception as e:
        out.write('%s open fail %s\n' % (f, e)); continue
    def walk(i, path, depth):
        name = nx.name(i)
        p = path + '/' + name
        kids = nx.children(i)
        if len(kids) >= 8:
            names = [nx.name(c) for c in kids]
            if all(len(n) == 1 and (n.isalpha() or n.isdigit()) for n in names):
                sizes = []
                for c in kids[:40]:
                    t = nx.node(c)
                    sizes.append('%s:%dx%d' % (nx.name(c), t[5], t[6]) if t[3] == 5 else '%s:t%d' % (nx.name(c), t[3]))
                out.write('%s %s  (%d kids) %s\n' % (f, p, len(kids), ' '.join(sizes)))
        if depth < 3:
            for c in kids:
                walk(c, p, depth + 1)
    for root in nx.children(0):
        walk(root, '', 1)
out.close()
print('done')
