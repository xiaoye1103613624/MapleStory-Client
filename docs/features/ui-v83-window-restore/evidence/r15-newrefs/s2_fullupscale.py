# r15 s2: upscaling helper for visual inspection + edge-profile dump.
import os, io, sys
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
PATHS = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
    "img3": r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png",
    "img4": r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png",
    "img5": r"C:\Users\11036\Desktop\正确客户端截图\打开快捷键，向上时快捷键处于收起状态.png",
}
scale = int(sys.argv[1]) if len(sys.argv) > 1 else 4
for key in sorted(PATHS):
    im = Image.open(PATHS[key]).convert("RGBA")
    up = im.resize((im.size[0] * scale, im.size[1] * scale), Image.NEAREST)
    up.save(os.path.join(OUTDIR, "full_%s_%dx.png" % (key, scale)))
print("done")
