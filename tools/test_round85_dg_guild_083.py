# -*- coding: utf-8 -*-
"""round85 D-G UIGuild entry convergence + orange-wz key spec: static guards + scenarios.

Verification contract (owner self-test, no CI/eval tampering):
  A. Static guards on the 3 patched cpp files + UIGuild.h + UIStateGame.cpp +
     AGENTS.md + evidence JSON + retest scripts.
  B. Behavioural scenarios: 4-entry routing truth table + toggle semantics.

Run:  python tools/test_round85_dg_guild_083.py   -> PASS / TEST_RC=0
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def rd(rel):
    with open(os.path.join(ROOT, rel), "r", encoding="utf-8") as f:
        return f.read()


def strip_comments(text):
    """Drop // line comments so a doc mention of emplace<UIGuild> is not a false hit."""
    out = []
    for line in text.splitlines():
        i = line.find("//")
        out.append(line if i < 0 else line[:i])
    return "\n".join(out)


GUARDS = []
SCEN = []


def chk(name, cond):
    GUARDS.append((name, bool(cond)))
    if not cond:
        print("[FAIL] " + name)


def sc(name, cond):
    SCEN.append((name, bool(cond)))
    if not cond:
        print("[SCEN-FAIL] " + name)


# ------------------------------------------------------------------ A. static guards
sb = rd("IO/UITypes/UIStatusBar.cpp")
sb_code = strip_comments(sb)
ms = rd("IO/UITypes/UIMiscShells.cpp")
ms_code = strip_comments(ms)
gd = rd("IO/UITypes/UIGuild.cpp")
gh = rd("IO/UITypes/UIGuild.h")
st = rd("IO/UIStateGame.cpp")
ag = rd("AGENTS.md")

# A1: no user-reachable code instantiates the fabricated-root floating window
for label, code in [("UIStatusBar", sb_code), ("UIMiscShells", ms_code)]:
    chk("A1 " + label + " no emplace<UIGuild> (comment-stripped)",
        "emplace<UIGuild>" not in code)
    chk("A1 " + label + " no get_element<UIGuild> (comment-stripped)",
        "get_element<UIGuild>" not in code)

# A2: UIStatusBar BT_COMMUNITY_GUILD -> exact FRIENDS/PARTY toggle template, Tab::GUILD
chk("A2a GUILD tab declared", "auto tab = UIUserList::Tab::GUILD;" in sb_code)
guild_case = sb_code[sb_code.find("case Buttons::BT_COMMUNITY_GUILD:"):]
guild_case = guild_case[:guild_case.find("remove_menus();")]
chk("A2b GUILD uses get_element<UIUserList>", "get_element<UIUserList>()" in guild_case)
chk("A2c GUILD same-tab toggle deactivate", "userlist->deactivate()" in guild_case)
chk("A2d GUILD same-tab toggle makeactive", "userlist->makeactive()" in guild_case)
chk("A2e GUILD cross-tab change_tab", "userlist->change_tab(tab)" in guild_case)
chk("A2f GUILD emplace(tab)", "emplace<UIUserList>(tab)" in guild_case)

# A3: UIStatusBar BT_MENU_UNION -> Tab::UNION shared window
union_case = sb_code[sb_code.find("case Buttons::BT_MENU_UNION:"):]
union_case = union_case[:union_case.find("remove_menus();")]
chk("A3a UNION emplace", "emplace<UIUserList>(UIUserList::Tab::UNION)" in union_case)
chk("A3b UNION change_tab", "change_tab(UIUserList::Tab::UNION)" in union_case)
chk("A3c UNION no UIGuild", "UIGuild" not in union_case)

# A4: UIMiscShells case '7' and 'U'/'u' -> Tab::GUILD
chk("A4a case '7' emplace UIUserList GUILD",
    re.search(r"case '7':\s*(?://[^\n]*\n\s*)*UI::get\(\)\.emplace<UIUserList>\(UIUserList::Tab::GUILD\);", ms_code) is not None)
# 'U' appears in 5 send_key bodies (UIAranSkillGuide/UIUtilDlgExAvatar/...).
# Anchor on the unique round85 D-G comment so the slice is the patched block only.
u_anchor = ms.find("// round85 D-G：U 键同样收敛")
chk("A4-u round85 U-key anchor present", u_anchor >= 0)
u_raw = ms[u_anchor: u_anchor + 800] if u_anchor >= 0 else ""
u_case = strip_comments(u_raw)
u_case = u_case[:u_case.find("deactivate();")]
chk("A4b U key get_element<UIUserList>", "get_element<UIUserList>()" in u_case)
chk("A4c U key emplace GUILD", "emplace<UIUserList>(UIUserList::Tab::GUILD)" in u_case)
chk("A4d U key change_tab(GUILD)", "change_tab(UIUserList::Tab::GUILD)" in u_case)
chk("A4e U key no UIGuild", "UIGuild" not in u_case)

# A5: both cpp include UIUserList.h (compile prerequisite)
chk("A5a UIStatusBar includes UIUserList.h", '#include "UIUserList.h"' in sb)
chk("A5b UIMiscShells includes UIUserList.h", '#include "UIUserList.h"' in ms)

# A6: UIUserList signatures used by the patch exist
uh = rd("IO/UITypes/UIUserList.h")
chk("A6a UIUserList(uint16_t tab) ctor", "UIUserList(uint16_t tab);" in uh)
chk("A6b GUILD enum member", re.search(r"enum Tab\s*\{[^}]*\bGUILD\b", uh, re.S) is not None)
chk("A6c UNION enum member", re.search(r"enum Tab\s*\{[^}]*\bUNION\b", uh, re.S) is not None)
chk("A6d change_tab(uint8_t)", "void change_tab(uint8_t tabid);" in uh)
chk("A6e get_tab()", "uint16_t get_tab();" in uh)

# A7: main G-key path still 083-aligned (regression guard)
chk("A7a UIStateGame G(17) -> Tab::GUILD", "tab = UIUserList::Tab::GUILD;" in st)
chk("A7b UIStateGame emplace<UIUserList>(tab)", "emplace<UIUserList>(tab);" in st)
chk("A7c UIStateGame 083 shared-window comment",
    "共用窗的第 3 页签" in st and "不是独立 UIGuild 浮窗" in st)
chk("A7d UIStateGame no emplace<UIGuild> (comment-stripped)",
    "emplace<UIGuild>" not in strip_comments(st))

# A8: UIGuild class kept as dead-code compatible placeholder (architecture-level NOT done)
chk("A8a UIGuild class still declared", "class UIGuild : public UIElement" in gh)
chk("A8b Type::GUILD still present", "static constexpr Type TYPE = UIElement::Type::GUILD;" in gh)
chk("A8c round85 convergence comment in UIGuild.cpp", "round85 D-G 已按 m13177 授权收口" in gd)

# A9: AGENTS.md key spec corrected to two specs
chk("A9a AGENTS.md 128B DEFAULT_KEY spec", "128 字节" in ag and "WzAESConstant.DEFAULT_KEY" in ag)
chk("A9b AGENTS.md 65536B .nx spec", "65536 字节完整 key 表" in ag and "key_gms" in ag)
chk("A9c AGENTS.md WzMutableKey mechanism", "getTrimmedUserKey()" in ag and "i < 128; i += 16" in ag)
chk("A9d AGENTS.md falsifies zero-key-only", "真因是 key 规格错配" in ag)
chk("A9e AGENTS.md no unconditional 65536B-only claim",
    "必须是 **65536 字节完整 key 表**" not in ag)

# A10: evidence JSON records 3 experiments, exp3 isError:false
ev = json.loads(rd("docs/features/development-and-fix-plan-2026-11/evidence/guild-window-083-round85.json"))
exps = ev["orange_wz_key_spec"]["experiments"]
chk("A10a evidence 3 experiments", len(exps) == 3)
chk("A10b exp1 zero key fails", "IMG 解析失败" in exps[0]["result"])
chk("A10c exp2 real 65536B fails", "IMG 解析失败" in exps[1]["result"] and "87384" in exps[1]["key"])
chk("A10d exp3 128B succeeds", "isError:false" in exps[2]["result"] and "128B" in exps[2]["key"])
chk("A10e two_specs split", set(ev["orange_wz_key_spec"]["two_specs"].keys()) ==
    {"orange_wz_MCP", "nx_nlnx_tools_wz_py"})
chk("A10f patch residual NONE", ev["patch"]["residual_emplace_UIGuild"].startswith("NONE"))
chk("A10g no protocol changes", ev["patch"]["protocol_changes"] == 0)
chk("A10h architecture-level NOT done", "architecture_level_NOT_done" in ev["decisions"])

# A11: four docs anchors present
dev = rd("docs/features/development-and-fix-plan-2026-11/README.md")
plt = rd("docs/features/pending-live-tests/README.md")
notes_path = "docs/features/development-and-fix-plan-2026-11/notes/wave2-guild-dg-resolution-20261111.md"
chk("A11a dev-plan 2-32 row", "**2-32**" in dev and "#devfix-guild-dg-resolution" in dev)
chk("A11b PLT section 43", "## 43. round85" in plt and "43-5" in plt)
chk("A11c PLT section 42 updated", "round85 已按用户 m13177 授权" in plt)
chk("A11d notes file exists", os.path.exists(os.path.join(ROOT, notes_path)))
chk("A11e evidence round85 file exists",
    os.path.exists(os.path.join(ROOT, "docs/features/development-and-fix-plan-2026-11/evidence/guild-window-083-round85.json")))

# A12: 128B key file + retest script exist
chk("A12a 128B key b64 file", os.path.exists(os.path.join(ROOT, "tools/_gms_key_128.b64")))
chk("A12b retest_128 script", os.path.exists(os.path.join(ROOT, "tools/_mcp_retest_128.py")))
chk("A12c retest_128 uses 128 keyfile", "_gms_key_128.b64" in rd("tools/_mcp_retest_128.py"))

# ------------------------------------------------------------------ B. scenarios
# Mini state machine mirroring the FRIENDS/PARTY toggle template now used by GUILD.
class Win:
    def __init__(self, tab, active):
        self.tab = tab
        self.active = active
        self.log = []


def community_click(existing, tab):
    """Return (result_tab, active, actions). Replicates UIStatusBar GUILD case."""
    actions = []
    if existing is None:
        actions.append("emplace:" + tab)
        return (tab, True, actions)
    w = existing
    if w.tab == tab:
        if w.active:
            w.active = False
            actions.append("deactivate")
        else:
            w.active = True
            actions.append("makeactive")
    else:
        if not w.active:
            w.active = True
            actions.append("makeactive")
        w.tab = tab
        actions.append("change_tab:" + tab)
    return (w.tab, w.active, actions)


def u_key(existing):
    """Replicates UIMiscShells 'U'/'u' case."""
    actions = []
    if existing is None:
        actions.append("emplace:GUILD")
        return ("GUILD", True, actions)
    w = existing
    if not w.active:
        w.active = True
        actions.append("makeactive")
    if w.tab != "GUILD":
        w.tab = "GUILD"
        actions.append("change_tab:GUILD")
    return (w.tab, w.active, actions)


# B1-B6: GUILD community toggle truth table (6 groups)
t, a, act = community_click(None, "GUILD")
sc("B1 no-window -> emplace GUILD active", t == "GUILD" and a and act == ["emplace:GUILD"])
t, a, act = community_click(Win("GUILD", True), "GUILD")
sc("B2 GUILD+active -> deactivate", t == "GUILD" and not a and act == ["deactivate"])
t, a, act = community_click(Win("GUILD", False), "GUILD")
sc("B3 GUILD+inactive -> makeactive", t == "GUILD" and a and act == ["makeactive"])
t, a, act = community_click(Win("FRIEND", True), "GUILD")
sc("B4 FRIEND+active -> change_tab only", t == "GUILD" and a and act == ["change_tab:GUILD"])
t, a, act = community_click(Win("FRIEND", False), "GUILD")
sc("B5 FRIEND+inactive -> makeactive+change_tab",
   t == "GUILD" and a and act == ["makeactive", "change_tab:GUILD"])
t, a, act = community_click(Win("UNION", True), "GUILD")
sc("B6 UNION+active -> change_tab only", t == "GUILD" and a and act == ["change_tab:GUILD"])

# B7-B10: 4-entry routing truth table
sc("B7 case '7' routes to GUILD", "emplace<UIUserList>(UIUserList::Tab::GUILD)" in ms_code)
sc("B8 BT_MENU_UNION routes to UNION", "emplace<UIUserList>(UIUserList::Tab::UNION)" in union_case)
sc("B9 U key no-window -> emplace GUILD",
   u_key(None) == ("GUILD", True, ["emplace:GUILD"]))
t, a, act = u_key(Win("PARTY", True))
sc("B10 U key existing different tab -> change_tab (no reopen)",
   t == "GUILD" and a and act == ["change_tab:GUILD"])

# B11-B13: 083 semantics vs fabricated root
sc("B11 083 GUILD tab index 3 (FRIEND,PARTY,GUILD)",
   re.search(r"FRIEND,\s*PARTY,\s*GUILD", uh) is not None)
sc("B12 fabricated root still zero-instantiated (residual NONE)",
   ev["patch"]["residual_emplace_UIGuild"].startswith("NONE"))
sc("B13 s9 protocol untouched", ev["patch"]["protocol_changes"] == 0
   and ev["patch"]["s9_handler_changes"] == 0 and ev["patch"]["GuildModel_changes"] == 0)

# B14-B16: orange-wz key spec truth table
sc("B14 zero key -> parse fail", "IMG 解析失败" in exps[0]["result"])
sc("B15 real 65536B -> parse fail (spec mismatch)", "IMG 解析失败" in exps[1]["result"])
sc("B16 128B DEFAULT_KEY -> isError:false", "isError:false" in exps[2]["result"])

# ------------------------------------------------------------------ summary
failed = [n for n, ok in GUARDS if not ok]
bad = [n for n, ok in SCEN if not ok]
print("STATIC GUARDS checked=%d failed=%d" % (len(GUARDS), len(failed)))
print("SCENARIOS %d, bad=%d" % (len(SCEN), len(bad)))
for n in failed:
    print("  FAIL " + n)
for n in bad:
    print("  SCEN-FAIL " + n)

if failed or bad:
    print("FAIL: round85 D-G guild convergence verification")
    sys.exit(1)

print("PYCOMPILE_OK")
print("PASS: round85 D-G UIGuild entry convergence (083 shared window) + orange-wz 128B key spec")
sys.exit(0)
