import importlib.util, sys
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx')
print('root children:')
for c in nx.children(0):
    t = nx.node(c)
    print(' ', nx.name(c), 'type=%d' % t[3], 'children=%d' % t[2])
