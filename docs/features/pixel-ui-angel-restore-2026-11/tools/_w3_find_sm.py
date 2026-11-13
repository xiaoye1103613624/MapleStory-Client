from pathlib import Path

root = Path(r"C:\Users\11036\.cursor\projects\e-project-MapleStory-Client\agent-tools")
for f in sorted(root.glob("*.txt"), key=lambda p: p.stat().st_mtime, reverse=True):
    t = f.read_text(encoding="utf-8", errors="replace")
    if "SkillMacro/backgrnd" in t:
        print(
            f.name,
            "len",
            len(t),
            "has207",
            '"width":207' in t,
            "haspng",
            "iVBOR" in t,
        )
