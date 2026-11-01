import importlib.util, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')

def ctx(off, before=300, after=300):
    chunk = nx.data[off-before:off+after]
    return chunk.decode('utf-8', 'replace')

for off in (0x4e9bde, 0x68c05e, 0x5f38f7):
    print('=== around', hex(off), '===')
    s = ctx(off)
    # keep only printable CJK/ascii runs
    import re
    runs = re.findall(r'[\u4e00-\u9fffA-Za-z0-9 :/\.\-\(\)]{2,}', s)
    print(' | '.join(runs))
    print()
