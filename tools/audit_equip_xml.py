#!/usr/bin/env python3
"""审计：客户端 Character.nx 装备 img（含 inc* 白值） vs 服务端 wz/Character.wz XML，
找出客户端有模型但服务端缺白值 XML 的装备（高版本半移植检测）。

用 nx_inspect.py 的 Nx 类只读解析 PKG4。"""
import os
import sys
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from nx_inspect import Nx

CLIENT_NX = r'E:\MXD\BeiDou-Client-xy\Data\.nxcache\Character.nx'
SERVER_WZ = r'E:\project\BeiDou-Server_s9\gms-server\wz\Character.wz'

# 服务端 Character.wz 目录名 -> 客户端 nx 顶层目录名（通常一致）
EQUIP_DIRS = ['Accessory', 'Cap', 'Cape', 'Coat', 'Dragon', 'Glove', 'Longcoat',
              'Pants', 'Ring', 'Shield', 'Shoes', 'Weapon']


def server_ids(directory):
    d = os.path.join(SERVER_WZ, directory)
    out = set()
    if not os.path.isdir(d):
        return out
    for f in os.listdir(d):
        if f.endswith('.img.xml'):
            out.add(f[:-len('.img.xml')])
    return out


def client_equips(nx, directory):
    """返回 [(img_name, inc_dict)]，img_name 形如 01003242.img"""
    root = nx.find(directory)
    if root is None:
        return []
    out = []
    for img in nx.children(root):
        name = nx.name(img)
        if not (name.endswith('.img') and name[:-4].isdigit()):
            continue
        info = None
        for c in nx.children(img):
            if nx.name(c) == 'info':
                info = c
                break
        inc = {}
        if info is not None:
            for c in nx.children(info):
                n = nx.name(c)
                if n.startswith('inc'):
                    inc[n] = nx.value(c)
        out.append((name, inc))
    return out


def main():
    print('loading', CLIENT_NX, '(mmap, ~8GB sparse)')
    nx = Nx(CLIENT_NX)
    total_client = 0
    total_missing = 0
    total_missing_with_inc = 0
    missing_ids = set()
    per_dir = []
    for d in EQUIP_DIRS:
        ce = client_equips(nx, d)
        se = server_ids(d)
        missing = [(n, inc) for n, inc in ce if n[:-4] not in se]
        miss_inc = [(n, inc) for n, inc in missing if inc]
        total_client += len(ce)
        total_missing += len(missing)
        total_missing_with_inc += len(miss_inc)
        for n, _ in missing:
            missing_ids.add(n[:-4])
        per_dir.append((d, len(ce), len(se), len(missing), len(miss_inc)))

    print('\n%-12s %8s %8s %8s %10s' % ('DIR', 'client', 'server', 'missing', 'miss+inc'))
    for row in per_dir:
        print('%-12s %8d %8d %8d %10d' % row)
    print('\nTOTAL client equips:', total_client)
    print('TOTAL missing XML  :', total_missing)
    print('TOTAL missing WITH inc stats:', total_missing_with_inc)
    print('unique missing ids:', len(missing_ids))

    # 输出带 inc 白值的缺失清单（这些才是真正需要补的），按 id 排序
    print('\n===== MISSING WITH inc STATS (need XML) =====')
    out = []
    for d in EQUIP_DIRS:
        ce = client_equips(nx, d)
        se = server_ids(d)
        for n, inc in ce:
            if inc and n[:-4] not in se:
                out.append((n[:-4], d, inc))
    out.sort()
    for iid, d, inc in out:
        keys = ','.join('%s=%s' % (k, v) for k, v in sorted(inc.items()))
        print('%s  %-10s  %s' % (iid, d, keys))
    print('\ncount:', len(out))


if __name__ == '__main__':
    main()
