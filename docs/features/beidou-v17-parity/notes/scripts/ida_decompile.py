# IDA 9.4 decompile + fallback disassembly for EAs listed in a text file
import idc
import ida_funcs
import ida_hexrays
import ida_bytes
import ida_lines
import ida_name

IN_LIST = r"C:\Users\11036\AppData\Local\Temp\opencode\dec_targets.txt"
OUT = r"C:\Users\11036\AppData\Local\Temp\opencode\dec_out.txt"

hexrays_ok = ida_hexrays.init_hexrays_plugin()

eas = []
with open(IN_LIST, "r", encoding="utf-8") as fh:
    for line in fh:
        line = line.strip()
        if line:
            eas.append(int(line, 16))

buf = []
for ea in eas:
    f = ida_funcs.get_func(ea)
    if f is None:
        buf.append("===== 0x%08X : NOT A FUNCTION =====" % ea)
        continue
    name = ida_funcs.get_func_name(f.start_ea)
    buf.append("")
    buf.append("=" * 90)
    buf.append("===== 0x%08X  %s  (size=%d)" % (f.start_ea, name, f.end_ea - f.start_ea))
    buf.append("=" * 90)
    done = False
    if hexrays_ok:
        try:
            cf = ida_hexrays.decompile(f.start_ea)
            if cf is not None:
                buf.append(str(cf))
                done = True
            else:
                w = ida_hexrays.get_warnings()
                buf.append("<<decompile returned None; warnings=%r>>" % (w,))
        except Exception as ex:
            buf.append("<<decompile failed: %s>>" % ex)
    if not done:
        buf.append("----- disassembly -----")
        cur = f.start_ea
        while cur < f.end_ea:
            line = idc.generate_disasm_line(cur, 0)
            line = ida_lines.tag_remove(line) if line else ""
            buf.append("0x%08X  %s" % (cur, line))
            nxt = idc.next_head(cur, f.end_ea)
            if nxt <= cur:
                break
            cur = nxt

with open(OUT, "w", encoding="utf-8") as fh:
    fh.write("\n".join(buf))
print("[dec] wrote %d functions -> %s (hexrays=%s)" % (len(eas), OUT, hexrays_ok))
idc.qexit(0)
