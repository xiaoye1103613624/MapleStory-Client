import os

ROOT = 'E:/project/MapleStory-Client/IO/UITypes'
# 原生 v83 通用窗体关闭钮 = Basic.img/BtClose（12x12 蓝底白叉）。
# 取证：R_equip.png / R_stats.png 关闭钮区域像素直方图 {(0,0,0):40,(85,204,255):28,
# (221,255,255):12} 与 nx 导出 Basic.img/BtClose/normal/0 完全一致；
# BtClose2 是 14x14 空心白框黑叉，参考截图里没有窗口用它。
# BtClose3 在本包不存在，故各站点的实际取图一直落到错误的 BtClose2。
# 按字节替换：源文件 BOM/CRLF 混用，文本方式重写会污染行尾与编码。
OLD = b'["Basic.img"]["BtClose2"]'
NEW = b'["Basic.img"]["BtClose"]'
total = 0

for fn in sorted(os.listdir(ROOT)):
    if not fn.endswith('.cpp') or fn == 'UIQuestLog.cpp':
        continue
    p = os.path.join(ROOT, fn)
    b = open(p, 'rb').read()
    n = b.count(OLD)
    if not n:
        continue
    open(p, 'wb').write(b.replace(OLD, NEW))
    print(fn, n)
    total += n

print('sites:', total)
