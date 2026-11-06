import idautils, ida_funcs, ida_name, ida_bytes, idaapi, idc
try:
    ib=idaapi.get_imagebase()
except Exception:
    ib=0
print("imagebase",hex(ib))
# 解密串 VA（beidou_v177_decrypted_strings.tsv）
vas={0xb21e10:"backgrnd/1/back",0xb21db0:"Bt/1/BtClose",0xb21d80:"funckey/backspacea",
     0xb21d54:"funckey/enter",0xb21d30:"keyd",0xb21d20:"%d/normal/0",0xb21d10:"%d/pressed/0",
     0xb21f18:"explicit?Wedding",0xb21de0:"funckey/backgrnd?"}
for va,label in vas.items():
    # 该 VA 是否是有效 EA
    try:
        b=ida_bytes.get_bytes(va,48)
    except Exception as e:
        b=None
    prev=b[:48].split(b"\x00")[0][:48] if b else b""
    xr=list(idautils.XrefsTo(va))
    fs=set()
    for x in xr:
        f=ida_funcs.get_func(x.frm)
        if f: fs.add((hex(f.start_ea),ida_name.get_name(f.start_ea)))
    print(f"{hex(va)} {label}: bytes={prev!r} xrefs={len(xr)} funcs={sorted(fs)[:6]}")
