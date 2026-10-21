# -*- coding: utf-8 -*-
import re
p = r"E:\MXD\20dalu_079MS\20dalu\wz\UI.wz\Login.img.xml"
t = open(p, encoding="utf-8", errors="ignore").read()
j = t.find('<imgdir name="Loading">')
print("Loading", j)
print(t[j:j+2000])
print("==== Notice backgrnd ====")
k = t.find('<imgdir name="Notice">')
print(t[k:k+600])
