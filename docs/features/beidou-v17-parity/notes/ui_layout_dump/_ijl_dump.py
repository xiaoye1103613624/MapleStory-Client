import io

paths = [
    r"E:\MXD\BeiDou-ClientV17.7\ijl15.dll",
    r"E:\MXD\BeiDou-Client-xy\ijl15.dll",
]

out = []
data = None
used = None
for p in paths:
    try:
        with open(p, "rb") as f:
            data = f.read()
        used = p
        break
    except OSError as e:
        out.append("skip %s (%s)" % (p, e))

if data is None:
    out.append("NO DLL FOUND")
else:
    out.append("file=%s size=%d" % (used, len(data)))
    start = 0x2c700
    end = min(0x2cc00, len(data))
    region = data[start:end]
    # 以 \x00 分割成字符串，记录其文件偏移
    chunks = []
    cur = b""
    cur_off = start
    for i, b in enumerate(region):
        if b == 0:
            if cur:
                chunks.append((cur_off, cur))
            cur = b""
            cur_off = start + i + 1
        else:
            if not cur:
                cur_off = start + i
            cur += bytes([b])
    if cur:
        chunks.append((cur_off, cur))

    for off, raw in chunks:
        try:
            s = raw.decode("gbk")
        except Exception:
            try:
                s = raw.decode("latin-1")
            except Exception:
                s = repr(raw)
        printable = all(0x20 <= ord(ch) < 0x7f or ord(ch) > 0x7f for ch in s)
        out.append("0x%x len=%d %s %s" % (off, len(raw), s, "" if printable else "(?)"))

with io.open(r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/_ijl_dump.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
