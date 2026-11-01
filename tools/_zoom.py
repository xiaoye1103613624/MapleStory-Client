import sys
from PIL import Image

# 找用户截图：当前端装备窗 (22-31-38)
src = None
import glob, os
for p in [r'C:\Users\11036\AppData\Roaming\CodeBuddy CN\User\globalStorage\tencent-cloud.coding-copilot',
          r'C:\Users\11036\AppData\Local\Temp',
          r'E:\project']:
    for f in glob.glob(os.path.join(p, '**', '*22-31-38*'), recursive=True):
        src = f
        break
    if src: break
print('src:', src)
if src:
    im = Image.open(src)
    print('size:', im.size)
    # 装备窗区域（1280x720 截图，top-center）：裁 x 330..570, y 40..270 放大 3x
    crop = im.crop((330, 40, 570, 270)).resize((240*3, 230*3), Image.LANCZOS)
    out = r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\zoom_equip_current.png'
    crop.save(out)
    print('saved', out)
