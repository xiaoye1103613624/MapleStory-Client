# -*- coding: utf-8 -*-
import json
p = r"E:\MXD\BeiDou-Client-xy\REA反编译\wz_dump\UIWindow_img.json"
d = json.load(open(p, encoding="utf-8"))

def summarize(obj, prefix="", depth=0):
    if depth > 4:
        return
    if not isinstance(obj, dict):
        print(prefix + repr(obj)[:120])
        return
    keys = [k for k in obj.keys() if not str(k).startswith("_")]
    w = obj.get("width")
    h = obj.get("height")
    o = obj.get("origin")
    if w is not None:
        print("%s[canvas %sx%s origin=%s]" % (prefix, w, h, o))
    for k in keys:
        v = obj[k]
        if isinstance(v, dict):
            ww, hh, oo = v.get("width"), v.get("height"), v.get("origin")
            extra = ""
            if ww is not None:
                extra = " %sx%s origin=%s" % (ww, hh, oo)
            print("%s%s%s" % (prefix, k, extra))
            summarize(v, prefix + "  ", depth + 1)
        else:
            print("%s%s=%s" % (prefix, k, v))

for name in ["InitialQuiz", "SpeedQuiz"]:
    print("====", name, "====")
    summarize(d[name])
    print()
