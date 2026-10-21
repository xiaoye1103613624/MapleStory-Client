# -*- coding: utf-8 -*-
import re
p = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\Basic.img.xml"
t = open(p, encoding="utf-8", errors="ignore").read()
i = t.find('<imgdir name="Cursor">')
print("Cursor idx", i)
# print first 2500 chars of Cursor
chunk = t[i:i+4000]
print(chunk[:4000])
# list first-level children names under Cursor
# find canvas/imgdir name= right after Cursor until depth returns
names = re.findall(r'<(?:imgdir|canvas) name="([^"]+)"', chunk)
print("names sample:", names[:40])
