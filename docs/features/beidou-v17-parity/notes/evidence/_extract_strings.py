import re, sys, struct
from pathlib import Path
exe = Path(r"E:\MXD\BeiDou-ClientV17.7\BeiDou.exe")
data = exe.read_bytes()
out = Path(r"E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence")

# ASCII printable runs >= 6
ascii_re = re.compile(rb'[\x20-\x7e]{6,}')
ascii_strs = [m.group().decode('ascii', 'ignore') for m in ascii_re.finditer(data)]

# UTF-16LE printable runs >= 4 chars
utf16 = []
i = 0
while i < len(data)-3:
    if 0x20 <= data[i] <= 0x7e and data[i+1] == 0:
        j = i
        chars = []
        while j+1 < len(data) and 0x20 <= data[j] <= 0x7e and data[j+1] == 0:
            chars.append(chr(data[j])); j += 2
        if len(chars) >= 4:
            utf16.append(''.join(chars))
        i = j
    else:
        i += 1

(out/"beidou_exe_ascii_full.txt").write_text("\n".join(sorted(set(ascii_strs))), encoding="utf-8")
(out/"beidou_exe_utf16_full.txt").write_text("\n".join(sorted(set(utf16))), encoding="utf-8")

# UIWindow.img references
ui_pat = re.compile(r'UIWindow\.img/[A-Za-z0-9_]+')
ui_hits = sorted(set(ui_pat.findall("\n".join(ascii_strs) + "\n" + "\n".join(utf16))))
(out/"beidou_exe_uiwindow_refs.txt").write_text("\n".join(ui_hits), encoding="utf-8")

# Feature keyword hits (native systems)
keywords = [
 "MonsterBook","Maker","Alliance","TradingRoom","PersonalShop","EntrustedShop",
 "Trunk","StoreBank","Messenger","Family","FamilyTree","Wedding","MapleTV",
 "ViciousHammer","VegaSpell","CashGachapon","RemoteGachapon","Incubator",
 "Delivery","Parcel","Duey","Ranking","PartySearch","MonsterCarnival",
 "AriantMatch","Minigame","RpsGame","SpeedQuiz","InitialQuiz","MobGage",
 "DualMobGauge","PartyRace","Memo","Fame","CashShop","ITC","Auction",
 "GuildBBS","SkillMacro","QuestAlarm","UserList","UtilDlgEx","KeyConfig",
 "GameOpt","SysOpt","Channel","WorldMap","MiniMap","Storage","HiredMerchant",
 "Fredrick","Duey","MTS","CashTrading","MemberShop","KarmaScissors",
 "ItemProtector","TreasureBox","NewYearsCard","Transform","Coupon",
 "CharacterClone","createCygnus","AranSkillGuide","SlideMenu","EnergyBar",
 "TemporaryStat","Megaphone","AvatarMegaphone","ItemMegaphone","ArtMegaphone",
 "Admin","GMBoard","Claim","Report","Radio","Title","counsel","raise","plant",
 "EnchantSkill","SkillEx","SkillUp","MacroSkill","MapleEvent","MonsterKilling",
 "muruengRaid","classMatch","FadeYesNo","SoftKeyboard","Notice","FloatNotice",
 "ContextMenu","ShortCut","GameMenu","IconBase","Book","itemSearch",
 "ExceptionItemSearch","MateMessage","CancelRequests","BetaEdition","Reset",
 "Teleport","MultiLine","MinigameTable","AdminClaim","CashTradingRoom"
]
joined = "\n".join(ascii_strs) + "\n" + "\n".join(utf16)
hits = []
for kw in keywords:
    # case-sensitive + simple count
    c = joined.count(kw)
    if c:
        hits.append(f"{kw}\t{c}")
(out/"beidou_exe_keyword_hits.txt").write_text("\n".join(hits), encoding="utf-8")
print(f"ascii={len(set(ascii_strs))} utf16={len(set(utf16))} uiwindow_refs={len(ui_hits)} kw_hits={len(hits)}")
for h in ui_hits: print(h)
