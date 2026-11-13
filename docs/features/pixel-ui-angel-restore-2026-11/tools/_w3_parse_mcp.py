import re
from pathlib import Path

raw = Path(
    r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools\fad81958-0e05-452e-8764-faaf872682d9.txt"
).read_text(encoding="utf-8", errors="replace")

for label in [
    "Item/backgrnd",
    "Item/FullBackgrnd",
    "Skill/backgrnd",
    "SkillMacroEx/backgrnd",
    "Equip/BtPet1/disabled/0",
    "Equip/BtPetEquipShow/normal/0",
]:
    i = raw.find(label)
    if i < 0:
        print("miss", label)
        continue
    chunk = raw[i : i + 1200]
    w = re.search(r'"width":(\d+)', chunk)
    h = re.search(r'"height":(\d+)', chunk)
    print(label, "size", f"{w.group(1) if w else '?'}x{h.group(1) if h else '?'}")
