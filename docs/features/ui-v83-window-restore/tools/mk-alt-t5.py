import os, shutil
src = r'E:\MXD\BeiDou-Client-xy'
dst = r'E:\MXD\ms-alt-t5'
skip_dirs = {'Data', 'Accessory', 'REA反编译', '__pycache__'}
skip_ext = {'.log', '.png', '.py', '.ps1', '.json'}
skip_files = {'Settings', 'xy-config.yaml', 'MapleStory.exe', 'MapleStory.pdb',
              'MapleStoryTest.exe', 'MapleStoryTest.pdb'}
MAX = 200_000_000
n = 0
for root, dirs, files in os.walk(src):
    rel = os.path.relpath(root, src)
    top = rel.split(os.sep)[0]
    if top in skip_dirs:
        dirs[:] = []
        continue
    for f in files:
        if f in skip_files:
            continue
        ext = os.path.splitext(f)[1].lower()
        if ext in skip_ext or ext.startswith('.bak'):
            continue
        fp = os.path.join(root, f)
        try:
            sz = os.path.getsize(fp)
        except OSError:
            continue
        if sz > MAX:
            print('skip big', f, sz)
            continue
        outp = os.path.join(dst, rel, f) if rel != '.' else os.path.join(dst, f)
        os.makedirs(os.path.dirname(outp), exist_ok=True)
        shutil.copy2(fp, outp)
        n += 1
print('copied', n)
