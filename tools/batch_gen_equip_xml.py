#!/usr/bin/env python3
"""批量补齐服务端 Character.wz XML：对"服务端套装引用的装备"中缺 XML 的，
从客户端 Character.nx 读 info 节点生成明文 XML。

范围：xy_set_item.enabled=1 的 item_ids 引用（玩家实际会穿的套装装备）。
属性驱动：只输出客户端 nx 实际存在的字段，不臆造。
"""
import os
import re
import sys
import xml.etree.ElementTree as ET
import pymysql

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from nx_inspect import Nx

CLIENT_NX = r'E:\MXD\BeiDou-Client-xy\Data\.nxcache\Character.nx'
SERVER_WZ = r'E:\project\BeiDou-Server_s9\gms-server\wz\Character.wz'
EQUIP_DIRS = ['Accessory', 'Cap', 'Cape', 'Coat', 'Dragon', 'Glove', 'Longcoat',
              'Pants', 'Ring', 'Shield', 'Shoes', 'Weapon']

# nx value -> xml value 清理
def clean(v):
    return v.strip().strip("'") if isinstance(v, str) else str(v)

def main():
    # 1. 客户端 nx 建索引: 8位id -> (dir, node index)
    nx = Nx(CLIENT_NX)
    index = {}  # '01003242' -> (dir, img_node)
    for d in EQUIP_DIRS:
        root = nx.find(d)
        if root is None:
            continue
        for img in nx.children(root):
            name = nx.name(img)
            if name.endswith('.img') and name[:-4].isdigit():
                index[name[:-4]] = (d, img)

    # 2. 服务端已有 XML
    have = set()
    for d in EQUIP_DIRS:
        p = os.path.join(SERVER_WZ, d)
        if os.path.isdir(p):
            have.update(f[:-len('.img.xml')] for f in os.listdir(p) if f.endswith('.img.xml'))

    # 3. 套装引用 id
    conn = pymysql.connect(host='127.0.0.1', user='root', password='root',
                           database='beidou', charset='utf8mb4')
    try:
        with conn.cursor() as c:
            c.execute("SELECT item_ids FROM xy_set_item WHERE enabled=1")
            rows = c.fetchall()
    finally:
        conn.close()
    ref = set()
    for (item_ids,) in rows:
        for tok in re.split(r'[,\s]+', str(item_ids or '')):
            tok = tok.strip()
            if tok.isdigit():
                if len(tok) == 7:
                    tok = '0' + tok
                if len(tok) == 8 and tok[:2] == '01':
                    ref.add(tok)

    need = sorted(i for i in ref if i not in have and i in index)
    no_client = sorted(i for i in ref if i not in have and i not in index)
    print('referenced:', len(ref), 'already have:', len(ref & have),
          'need gen:', len(need), 'no client model:', len(no_client))

    generated = 0
    for iid in need:
        d, img = index[iid]
        info = None
        for c in nx.children(img):
            if nx.name(c) == 'info':
                info = c
                break
        if info is None:
            continue
        # 构建 XML
        lines = ['<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',
                 '<imgdir name="%s.img">' % iid,
                 '  <imgdir name="info">']
        for ch in nx.children(info):
            n = nx.name(ch)
            _, _, _, ntype, data, w, h = nx.node(ch)
            if ntype == 1:  # int
                lines.append('    <int name="%s" value="%s"/>' % (n, clean(nx.value(ch))))
            elif ntype == 2:  # real -> int (装备字段都是整数)
                try:
                    fv = float(clean(nx.value(ch)))
                    lines.append('    <int name="%s" value="%d"/>' % (n, int(fv)))
                except ValueError:
                    pass
            elif ntype == 3:  # string
                lines.append('    <string name="%s" value="%s"/>' % (n, clean(nx.value(ch))))
            elif ntype == 4:  # vector
                v = clean(nx.value(ch)).strip('()').split(',')
                if len(v) == 2:
                    lines.append('    <vector name="%s" x="%s" y="%s"/>' % (n, v[0].strip(), v[1].strip()))
        lines += ['  </imgdir>', '</imgdir>']
        xml = '\r\n'.join(lines) + '\r\n'
        # 校验
        try:
            ET.fromstring(xml)
        except ET.ParseError as e:
            print('SKIP invalid XML', iid, e)
            continue
        out = os.path.join(SERVER_WZ, d, iid + '.img.xml')
        with open(out, 'w', encoding='utf-8', newline='') as f:
            f.write(xml)
        generated += 1
    print('generated:', generated)

    if no_client:
        outf = os.path.join(os.path.dirname(os.path.abspath(__file__)), '_setitem_no_client_model.txt')
        with open(outf, 'w', encoding='utf-8') as f:
            f.write('\n'.join(no_client))
        print('no client model list ->', outf)

if __name__ == '__main__':
    main()
