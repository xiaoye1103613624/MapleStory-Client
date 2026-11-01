# -*- coding: utf-8 -*-
"""Second pass: 80 ctor sites live in functions absent from dec_send/ (first pass)."""
import json, subprocess, sys

DB = "3c1162a2"
PORT = "13338"

FNS = ["0x5dbef0", "0x627e20", "0x9a6dd0"]


def rpc(tool, args, timeout=180):
    with open('_a_tmp.json', 'w', encoding='utf-8') as f:
        json.dump(args, f)
    out = subprocess.run(['python', 'mcp_call.py', PORT, tool, '_a_tmp.json', str(timeout)],
                         capture_output=True, text=True, encoding='utf-8').stdout
    d = json.loads(out)
    res = d['result']
    return res['structuredContent'] if 'structuredContent' in res else json.loads(res['content'][0]['text'])


def iq(q, timeout=180):
    return rpc('insn_query', {"database": DB, "queries": [q]}, timeout)['result'][0]


def all_matches(q, timeout=180):
    q = dict(q)
    allm, off = [], 0
    while True:
        q['offset'] = off
        r = iq(q, timeout)
        allm += r['matches']
        nx = (r.get('cursor') or {}).get('next')
        if not nx:
            break
        off = nx
    return allm


sites = []
for fn in FNS:
    calls = all_matches({"mnem": "call", "func": fn, "count": 400,
                         "include_disasm": True, "max_scan_insns": 80000})
    ctors = [m for m in calls if '??0COutPacket@@QAE@J@Z' in m['disasm']]
    print(fn, 'calls', len(calls), 'ctors', len(ctors), file=sys.stderr)
    for c in ctors:
        a = int(c['addr'], 16)
        prev = all_matches({"start": hex(max(a - 32, 0)), "end": hex(a), "count": 40,
                            "include_disasm": True})
        sites.append({"fn": fn, "addr": c['addr'], "prev": [p['disasm'] for p in prev]})

json.dump(sites, open('trunc2_ctor_sites.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
print('total ctor sites', len(sites))
