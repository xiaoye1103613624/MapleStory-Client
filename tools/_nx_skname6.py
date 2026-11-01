import importlib.util, sys, io, struct
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')

_offsets = struct.unpack_from('<%dQ' % nx.string_count, nx.data, nx.string_table)

def raw_str(idx):
    off = _offsets[idx]
    length = struct.unpack_from('<H', nx.data, off)[0]
    return nx.data[off+2:off+2+length]

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

for sid in ['0000008', '0000012', '0000100', '0001000', '0001001', '0001002', '0001003']:
    n = find('Skill.img/' + sid)
    if n is None:
        print(sid, '-> (no node)')
        continue
    for c in nx.children(n):
        if nx.name(c) == 'name':
            t = nx.node(c)
            b = raw_str(t[4])
            print(sid, 'utf8:', b.decode('utf-8', 'replace'), '| gbk:', b.decode('gbk', 'replace'))
