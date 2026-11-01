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

def raw_string(i):
    # strings table: u16 length prefix + bytes
    return None

# read raw bytes of a string node via data field (offset into string table area)
def get_str(node_idx):
    t = nx.node(node_idx)
    if t[3] != 3:
        return None
    off = t[4]
    length = int.from_bytes(nx.data[off:off+2], 'little')
    return nx.data[off+2:off+2+length]

for sid in ['0000008', '0000012', '0000100', '0001000', '0001001', '0001002', '0001003']:
    n = find('Skill.img/' + sid)
    if n is None:
        print(sid, '-> (no node)')
        continue
    out = []
    for c in nx.children(n):
        if nx.name(c) == 'name':
            b = get_str(c)
            out.append('name=' + (b.decode('gbk', 'replace') if b else '?'))
    print(sid, '->', '; '.join(out) if out else '(no name)')
