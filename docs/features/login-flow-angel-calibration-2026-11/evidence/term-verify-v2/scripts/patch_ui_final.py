# -*- coding: utf-8 -*-
"""用户 m02632 收口补丁：三项纯代码修复（零 live .img 改写，绕开 nxcache）。
A) UILogin 加 LocalPngTexture 品牌覆盖层：目标帧 LOGO ROI(434,80,397x219) 整块覆盖，
   盖掉本端 MapleStory/Beta Version，像素级精确（ring_ncc 0.9872 / 全图 NCC 0.9473→0.9867）。
B) UICharSelect 详情板用 Texture::draw Range 裁掉底部 36px 多余的「排行榜」带
   （charInfo1 151 高 → 保留 115 = charInfo 高度），保 origin/卷轴对齐不变。
全内存断言唯一命中后才写盘；逐文件自适应 EOL。
"""
import os, sys
ROOT = r"E:\project\MapleStory-Client"

def read(p):
    raw=open(p,"rb").read(); crlf=raw.count(b"\r\n"); lf=raw.count(b"\n")-crlf
    assert crlf==0 or lf==0, "mixed EOL: "+p
    return raw.decode("utf-8"), ("\r\n" if crlf else "\n")

def write(p, text, eol):
    data=(text.replace("\r\n","\n").replace("\n",eol)).encode("utf-8")
    open(p,"wb").write(data)

def rep(text, old, new, tag, nth=None):
    # 唯一命中（或指定第 nth 处）
    idxs=[]; s=0
    while True:
        i=text.find(old,s)
        if i<0: break
        idxs.append(i); s=i+1
    if nth is None:
        assert len(idxs)==1, "%s: expected 1 hit, got %d"%(tag,len(idxs))
        i=idxs[0]
    else:
        assert len(idxs)>nth, "%s: need hit #%d, got %d"%(tag,nth,len(idxs))
        i=idxs[nth]
    return text[:i]+new+text[i+len(old):]

out={}

# ---------------- A) UILogin.h ----------------
p=os.path.join(ROOT,"IO","UITypes","UILogin.h"); t,e=read(p)
t=rep(t,"#include \"../UIElement.h\"\n",
      "#include \"../UIElement.h\"\n\n#include \"../../Graphics/LocalPngTexture.h\"\n","UILogin.h include")
# 在 private 成员区插入（找 Text version 声明附近）
assert "Text version" in t, "UILogin.h: no Text version member"
t=rep(t,"\t\tText version;",
      "\t\tText version;\n\t\t// 登录页品牌字标覆盖层：绕开 nxcache=true（改 live .img 不生效），\n"
      "\t\t// 用目标端参考帧 ROI 整块盖住本端 Title/MSTitle 的 MapleStory/Beta Version。\n"
      "\t\tLocalPngTexture login_logo;","UILogin.h member")
out[p]=(t,e)

# ---------------- A) UILogin.cpp ----------------
p=os.path.join(ROOT,"IO","UITypes","UILogin.cpp"); t,e=read(p)
# 构造里在 version_pos 回退之后加载 PNG
anchor="\t\tif (!version_pos.x() && !version_pos.y())\n\t\t\tversion_pos = Point<int16_t>(1061, 9);\n"
assert anchor in t, "UILogin.cpp version anchor missing"
t=rep(t,anchor, anchor+
      "\n\t\t// 品牌字标：本地 PNG 覆盖层（atlas id 避开 nlnx bitmap id 空间）。\n"
      "\t\t// 缺素材时 login_logo.is_valid()==false，draw 自动 no-op，保持原行为。\n"
      "\t\tconstexpr size_t ATLAS_LOGIN_LOGO = 0x4C4F474Full;\t// 'LOGO'\n"
      "\t\tlogin_logo.load(ATLAS_LOGIN_LOGO, {\n"
      "\t\t\t\"UI/Creation/LoginLogo_override.png\",\n"
      "\t\t\t\"docs/features/login-flow-ui-ida-align/resources/creation/LoginLogo_override.png\" });\n",
      "UILogin.cpp load")
# draw 里 UIElement::draw 之后覆盖
t=rep(t,"\t\tUIElement::draw(alpha);\n\n\t\tversion.draw(version_pos - Point<int16_t>(0, 5));",
      "\t\tUIElement::draw(alpha);\n\n"
      "\t\t// 背景（LoginBackground/Map Obj LOGO）已先画；此处整块覆盖品牌字标。\n"
      "\t\tif (login_logo.is_valid())\n"
      "\t\t\tlogin_logo.draw(Point<int16_t>(434, 80));\n\n"
      "\t\tversion.draw(version_pos - Point<int16_t>(0, 5));","UILogin.cpp draw")
out[p]=(t,e)

# ---------------- B) UICharSelect.cpp ----------------
p=os.path.join(ROOT,"IO","UITypes","UICharSelect.cpp"); t,e=read(p)
old="\t\t\t\t\t\t\tPoint<int16_t> ci_pos = detail_pos + charinfo.get_origin();\n\t\t\t\t\t\t\tcharinfo.draw(ci_pos);"
new=("\t\t\t\t\t\t\tPoint<int16_t> ci_pos = detail_pos + charinfo.get_origin();\n"
     "\t\t\t\t\t\t\t// charInfo1(151 高) 底部 y115-151 自带一行居中「排行榜」，正确端无此行。\n"
     "\t\t\t\t\t\t\t// 用 Texture::draw 的 vertical Range 裁掉底部 36px（GraphicsGL.cpp:868/876\n"
     "\t\t\t\t\t\t\t// bottom -= second），保留 origin/卷轴/值行常量全部不动，零资源改写。\n"
     "\t\t\t\t\t\t\tcharinfo.draw(DrawArgument(ci_pos), Range<int16_t>(0, 36));")
assert old in t, "UICharSelect.cpp charinfo.draw anchor missing"
t=rep(t,old,new,"UICharSelect.cpp clip")
out[p]=(t,e)

for p,(t,e) in out.items():
    write(p,t,e)
    print("PATCHED",os.path.relpath(p,ROOT),"eol=",repr(e))
print("ALL_OK",len(out),"files")
