import importlib.util, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)
nx = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')

for t in ['团队治疗', '疾风步', '蜗牛投掷术', '精灵的祝福', '冒险家']:
    b = t.encode('gbk')
    idx = nx.data.find(b)
    print(t, '->', hex(idx) if idx >= 0 else 'miss')
    if idx >= 0:
        # dump surrounding readable GBK strings
        chunk = nx.data[idx-200:idx+200]
        # split on non-printable
        import re
        # decode as gbk with replace and split on control chars
        s = chunk.decode('gbk', 'replace')
        parts = [p for p in re.split(r'[\x00-\x1f]+', s) if len(p) >= 2]
        print('   ctx:', ' | '.join(parts[:20]))
