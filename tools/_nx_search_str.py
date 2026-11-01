import importlib.util, struct
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')

targets = ['自动装备', '物品装备', '自动矿物', '角色能力', '角色装备', '现有道具', '帽子', '眼睛', '战斗力']
data = nx.data
for t in targets:
    for enc in ('utf-8', 'gbk'):
        b = t.encode(enc)
        idx = data.find(b)
        if idx >= 0:
            print('FOUND', t, enc, 'at file offset', hex(idx))
            break
    else:
        print('miss', t)
