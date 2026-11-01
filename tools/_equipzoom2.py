from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png').convert('RGB')
crop = im.crop((0, 125, 175, 305))
crop = crop.resize((crop.width * 4, crop.height * 4), Image.NEAREST)
crop.save(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_zoom_bottom.png')
print('saved', crop.size)
