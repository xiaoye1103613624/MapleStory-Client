from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png').convert('RGB')
# top rows region zoom 4x
crop = im.crop((0, 20, 175, 140))
crop = crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST)
crop.save(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_zoom_top.png')
print('saved', crop.size)
