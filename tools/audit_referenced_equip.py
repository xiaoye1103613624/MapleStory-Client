#!/usr/bin/env python3
"""审计：服务端实际引用的装备（套装成员/掉落）中有多少缺 Character.wz XML。
只关注"服务端会真的发放"的装备，而非客户端 nx 里全部 3 万件美术资源。"""
import os
import re
import pymysql

SERVER_WZ = r'E:\project\BeiDou-Server_s9\gms-server\wz\Character.wz'
EQUIP_DIRS = ['Accessory', 'Cap', 'Cape', 'Coat', 'Dragon', 'Glove', 'Longcoat',
              'Pants', 'Ring', 'Shield', 'Shoes', 'Weapon']

# 收集服务端所有 XML id
server_ids = set()
for d in EQUIP_DIRS:
    p = os.path.join(SERVER_WZ, d)
    if os.path.isdir(p):
        for f in os.listdir(p):
            if f.endswith('.img.xml'):
                server_ids.add(f[:-len('.img.xml')])

conn = pymysql.connect(host='127.0.0.1', user='root', password='root',
                       database='beidou', charset='utf8mb4')
try:
    with conn.cursor() as c:
        c.execute("SELECT set_id, set_name, item_ids FROM xy_set_item WHERE enabled=1")
        rows = c.fetchall()
finally:
    conn.close()

print('enabled set rows:', len(rows))
ref = {}  # 8位id -> set info
for sid, name, item_ids in rows:
    if not item_ids:
        continue
    for tok in re.split(r'[,\s]+', str(item_ids)):
        tok = tok.strip()
        if tok.isdigit() and len(tok) == 7:
            iid = '0' + tok  # 1003242 -> 01003242
            ref.setdefault(iid, []).append((sid, name))

print('referenced equip ids (from enabled sets):', len(ref))
missing = [(iid, v) for iid, v in sorted(ref.items()) if iid not in server_ids]
print('referenced but MISSING XML:', len(missing))
for iid, v in missing[:80]:
    print(' ', iid, [x[1] for x in v][:2])
