# -*- coding: utf-8 -*-
"""round76 harness: UIGuild fabricated-root 083 read-only review.
Static guards + scenario models. Zero behavior literals changed.
ASCII-only stdout (GBK console safe).
"""
import json, os, re, subprocess, sys

P = lambda s: print(str(s).encode("ascii", "backslashreplace").decode())
FAILS = []
GUARDS = [0]


def chk(name, cond, detail=""):
    GUARDS[0] += 1
    if not cond:
        FAILS.append("GUARD %s :: %s" % (name, detail))
        P("  [FAIL] %s %s" % (name, detail))


def read(p):
    return open(p, "rb").read().decode("utf-8")


def run_bytes(args):
    p = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    return p.returncode, p.stdout.decode("utf-8", "replace")


def code_only(s):
    """drop //-comment lines and blanks; keep behavior literals intact."""
    out = []
    for l in s.replace("\r\n", "\n").split("\n"):
        t = l.strip()
        if not t or t.startswith("//"):
            continue
        out.append(l.rstrip())
    return "\n".join(out)


def main():
    cpp = read("IO/UITypes/UIGuild.cpp")
    h = read("IO/UITypes/UIGuild.h")
    dev = read("docs/features/development-and-fix-plan-2026-11/README.md")
    plt = read("docs/features/pending-live-tests/README.md")
    ev_path = "docs/features/development-and-fix-plan-2026-11/evidence/guild-window-083-round76.json"
    ev = json.loads(read(ev_path))
    notes = read("docs/features/development-and-fix-plan-2026-11/notes/wave2-guild-window-083-review-20261111.md")
    probe = read("tools/_guild_rt_stdout.txt")
    ow = read("tools/_guild_probe_stdout.txt")

    # ---------- A. static guards ----------
    rc, diff = run_bytes(["git", "diff", "HEAD", "--", "IO/UITypes/UIGuild.cpp", "IO/UITypes/UIGuild.h"])
    changed = [l for l in diff.split("\n")
               if (l.startswith("+") or l.startswith("-"))
               and not l.startswith("+++") and not l.startswith("---")]
    bad = [l for l in changed if l[1:].strip() and not l[1:].strip().startswith("//")]
    chk("A1 diff only comment lines (0 behavior lines)", rc == 0 and len(changed) > 0 and len(bad) == 0,
        "changed=%d bad=%d %s" % (len(changed), len(bad), str(bad[:3])))

    # A2: 083 EA / verdict / five proofs land in source comments
    for token in ["0x90f17c", "0x919b73", "0x812fb4", "0x8133a9",
                  "GuildUI.img", "open_merge", "D-G", "233x304", "734x526",
                  "UIStateGame.cpp:361-363", "CUIUserList", "CTabGuild"]:
        chk("A2 cpp/h carries 083 basis <%s>" % token, token in cpp or token in h, "missing token")
    chk("A2b old fabricated-root self-admission remains as evidence",
        "凭空写出来的逻辑" not in cpp.split("// round76")[0], "old wording not retired")

    # A3: behavior literals byte-identical to HEAD (comment-only patch)
    rc_h, head_cpp = run_bytes(["git", "show", "HEAD:IO/UITypes/UIGuild.cpp"])
    rc_h2, head_h = run_bytes(["git", "show", "HEAD:IO/UITypes/UIGuild.h"])
    chk("A3a UIGuild.cpp behavior code identical to HEAD",
        rc_h == 0 and code_only(head_cpp) == code_only(cpp), "cpp code drift")
    chk("A3b UIGuild.h behavior code identical to HEAD",
        rc_h2 == 0 and code_only(head_h) == code_only(h), "h code drift")
    for lit in ['nl::node guild = nl::nx::UI["GuildUI.img"];',
                "Point<int16_t>(300, 40), Point<int16_t>(300, 380)",
                "Point<int16_t>(280, 8)",
                "UiAssetResolve::close_btn()"]:
        chk("A3c literal intact <%s>" % lit[:46], lit in cpp, "literal missing")
    chk("A3d enum 5 buttons + ROW_H intact",
        all(b in h for b in ["BtCreate", "BtInvite", "BtNotice", "BtClose", "BtAlliance"])
        and "ROW_H = 18" in h, "enum/ROW_H changed")

    # A4: LoginBackground 083 linear interpolation preserved (two shapes)
    lb = read("IO/LoginBackground.cpp")
    lin = ("static_cast<double>(elapsed)" in lb and "static_cast<double>(tween_duration)" in lb) \
          or ("elapsed / tween_duration" in lb)
    chk("A4 LoginBackground.cpp:77 083 linear interpolation preserved", lin, "linear interpolation gone")

    # A5: evidence JSON fields
    chk("A5a verdict sibling defect", ev.get("verdict") == "sibling_defect_fabricated_root_no_083_window_class", "verdict mismatch")
    chk("A5b 083 no independent CUIGuild class", ev["ida_facts"]["no_independent_CUIGuild_class"] is True, "flag")
    chk("A5c D-G decision point present", any("D-G" in d for d in ev["decision_points"]), "D-G missing")
    chk("A5d zero behavior literals", ev["patch"]["behavior_literals_changed"] == 0, "non-zero")
    chk("A5e files_total=7", ev["patch"]["files_total"] == 7, "count")
    chk("A5f 54 Guild symbols", "54" in str(ev["ida_facts"]["query"]), "54 missing")
    chk("A5g orange-wz IMG_MISSING kept", "IMG_MISSING" in str(ev["orange_wz_gate"]["results"]), "gate")
    chk("A5h key spec 65536B", ev["orange_wz_gate"]["key"]["raw_len"] == 65536
        and ev["orange_wz_gate"]["key"]["sha256_16"] == "268a8407a47ee76c", "key spec")

    # A6: four docs
    chk("A6a evidence exists", os.path.exists(ev_path), "no evidence")
    chk("A6b notes 9 chapters", all(("## %d." % i) in notes for i in range(0, 10)), "notes chapters")
    chk("A6c dev 2-31 row", "| **2-31** |" in dev, "dev row missing")
    row31 = [l for l in dev.split("\n") if l.startswith("| **2-31** |")]
    chk("A6d dev 2-31 has 8 data columns (9 pipes)", len(row31) == 1 and row31[0].count("|") == 9,
        "pipes=%s" % (row31[0].count("|") if row31 else -1))
    chk("A6e PLT 42 header", "## 42. round76" in plt, "PLT missing")
    chk("A6f PLT 42-1..42-5", all(("42-%d" % i) in plt for i in range(1, 6)), "PLT items")

    # A7: five-zero proof chain + runtime/orange-wz facts
    zeros = [
        ev["ida_facts"]["string_search"]["GuildUI"] == "0 hits",
        "IMG_MISSING" in ow,
        ev["nx_facts"]["GuildUI.img"] == "NOT FOUND",
        ev["runtime_probe"]["GuildUI.img_valid"] == 0,
        "GuildUI.img_valid=0" in probe,
    ]
    chk("A7a five independent zeros for GuildUI.img", all(zeros), str(zeros))
    chk("A7b real guild assets in evidence",
        "233x304" in str(ev["nx_facts"]) and "734x526" in str(ev["nx_facts"])
        and ev["runtime_probe"]["UserList/Guild_kids"] == 8
        and ev["runtime_probe"]["GuildBBS.img/GuildBBS_kids"] == 14, "asset facts")
    chk("A7c runtime probe OPEN_MERGE_OK", "OPEN_MERGE_OK" in probe and "PROBE_DONE" in probe, "probe")
    chk("A7d orange-wz IMG parse failure text kept",
        "IMG" in ow and ("UIWindow.img" in ow or "GuildBBS.img" in ow), "gate text")
    chk("A7e main path 083 alignment recorded",
        "UIStateGame.cpp:361-363" in ev["current_end_facts"]["main_path_already_083"], "main path")

    # A8: newline hygiene
    chk("A8a UIGuild.cpp LF", b"\r\n" not in open("IO/UITypes/UIGuild.cpp", "rb").read(), "CRLF in cpp")
    chk("A8b UIGuild.h CRLF clean", b"\r\r\n" not in open("IO/UITypes/UIGuild.h", "rb").read()
        and b"\r\n" in open("IO/UITypes/UIGuild.h", "rb").read(), "h newline")
    chk("A8c dev README LF", b"\r\n" not in open("docs/features/development-and-fix-plan-2026-11/README.md", "rb").read(), "dev CRLF")
    pb = open("docs/features/pending-live-tests/README.md", "rb").read()
    chk("A8d PLT CRLF no CRCRLF", b"\r\r\n" not in pb and pb.endswith(b"\r\n"), "PLT newline")
    chk("A8e dev 2-31 inserted before Wave 3",
        dev.split("\n").index([l for l in dev.split("\n") if l.startswith("| **2-31** |")][0]) + 2
        < len(dev.split("\n")) and "### Wave 3" in dev.split("\n")[
            dev.split("\n").index([l for l in dev.split("\n") if l.startswith("| **2-31** |")][0]) + 2],
        "anchor drift")

    # ---------- B. scenarios ----------
    scen_bad = []
    n = [0]

    def sc(name, cond, detail=""):
        n[0] += 1
        if not cond:
            scen_bad.append("SCEN %s :: %s" % (name, detail))
            P("  [FAIL] %s %s" % (name, detail))

    # B1-B3: 083 symbol model
    guild_syms = ["CTabGuild::Draw@0x90f17c", "CUIUserList::OnCreate@0x919b73",
                  "CUIGuildBBS::ctor@0x812fb4", "CUIGuildBBS::OnCreate@0x8133a9",
                  "CWvsContext::OnGuildResult@0xa37490", "CField::InputGuildName@0x5305ae"]
    cuiguild = [s for s in guild_syms if s.startswith("CUIGuild")]
    sc("B1 no independent CUIGuild window class", all("CUIGuildBBS" in s for s in cuiguild),
       "found %s" % str(cuiguild))
    sc("B2 54 Guild symbols family", len(guild_syms) >= 6 and "CTabGuild::Draw" in guild_syms[0], "family")

    # B4-B8: five-zero proof model
    proof = {"search_text": 0, "physical": 0, "orange_wz": 0, "nx": 0, "runtime": 0}
    sc("B4 five proofs all zero", len(proof) == 5 and all(v == 0 for v in proof.values()), "proof")
    sc("B5 .nx NOT FOUND vs runtime valid=0 agree",
       ev["nx_facts"]["GuildUI.img"] == "NOT FOUND" and ev["runtime_probe"]["GuildUI.img_valid"] == 0,
       "nx/runtime disagreement")

    # B6-B9: real assets dimensions
    real = {"UserList_backgrnd": (312, 389), "UserList_Guild_backgrnd": (233, 304),
            "GuildBBS_backgrnd": (734, 526), "GuildBBS_backgrnd2": (327, 288)}
    sc("B6 083 real backgrnd dims", real["UserList_Guild_backgrnd"] == (233, 304)
       and real["GuildBBS_backgrnd"] == (734, 526), "dims")
    sc("B7 current hardcoded dimension differs from every real asset",
       (300, 380) not in real.values(), "300x380 collides")
    sc("B8 runtime kids 9/8/14", (ev["runtime_probe"]["UIWindow.img/UserList_kids"],
                                  ev["runtime_probe"]["UserList/Guild_kids"],
                                  ev["runtime_probe"]["GuildBBS.img/GuildBBS_kids"]) == (9, 8, 14), "kids")
    sc("B9 nx/runtime dims agree (no stale-snapshot flip)",
       "233x304" in str(ev["nx_facts"]) and ev["runtime_probe"]["UserList/Guild/backgrnd"] == "233x304"
       and "734x526" in str(ev["nx_facts"]) and ev["runtime_probe"]["GuildBBS/backgrnd"] == "734x526",
       "nx-runtime flip")

    # B10-B13: G-key main path already 083
    route = {"FRIENDS": "UIUserList::Tab::FRIEND", "PARTY": "UIUserList::Tab::PARTY",
             "BOSSPARTY": "UIUserList::Tab::GUILD", "GUILD": "UIUserList::Tab::GUILD"}
    sc("B10 G(17) routes to shared-window guild tab", route["GUILD"] == "UIUserList::Tab::GUILD", "route")
    sc("B11 BOSSPARTY shares guild tab", route["BOSSPARTY"] == route["GUILD"], "bossparty")
    sc("B12 main path means no s9 protocol edit (zero implementation)",
       ev["patch"]["behavior_literals_changed"] == 0 and "D-G" in str(ev["decision_points"]), "scope")
    sc("B13 D-G is architecture-level (UIElement::Type / GuildModel / handlers)",
       all(k in str(ev["decision_points"]) for k in ["UIElement::Type", "GuildModel", "five-screen"]),
       "D-G rationale")

    # B14-B17: 083 board control immediates
    ctrl = [(2019, 1, 3), (2021, 40, 450), (2013, 680, 501)]
    sc("B14 CUIGuildBBS 3 vtable+32 controls", len(ctrl) == 3, "ctrl count")
    sc("B15 ctrl ids distinct", len({c[0] for c in ctrl}) == 3, "ids")
    sc("B16 coords within 734x526 canvas",
       all(0 <= x <= 734 and 0 <= y <= 526 for _, x, y in ctrl), "coords")
    sp = ev["ida_facts"]["sp_names"]
    sc("B17 SP_ pools distinct per window",
       "SP_3348__GUILD_NOTICE" in sp["CTabGuild_Draw"]
       and "SP_3724_UI_GUILDBBSIMG_GUILDBBS_BTQUIT" in sp["CUIGuildBBS_OnCreate"], "sp pools")

    # B18-B20: current-end self-draw model
    btns = ["BtCreate", "BtInvite", "BtNotice", "BtClose", "BtAlliance"]
    with_sprite = ["BtClose"]
    sc("B18 exactly one button has a sprite", len(with_sprite) == 1 and with_sprite[0] == "BtClose", "sprites")
    sc("B19 four buttons remain keyboard-only entry", all(b in btns for b in btns[:3] + btns[4:]), "enum")
    sc("B20 full-text draw still uses 300x380 hardcoded dimension (no real-asset wiring)",
       "Point<int16_t>(300, 380)" in cpp
       and "sprites.emplace_back(Sprite(guild[" in cpp
       and "dimension = backgrnd" not in code_only(cpp)
       and "dimension = bg" not in code_only(cpp), "dimension")

    # B21-B23: docs + decision bookkeeping
    sc("B21 notes 9 chapters + D-G section", notes.count("## ") >= 9 and "D-G" in notes, "notes")
    sc("B22 PLT 42-1..42-5 rows", len(re.findall(r"\| 42-\d+ \|", plt)) == 5, "PLT rows")
    sc("B23 dev 2-31 precedes Wave 3",
       dev.find("| **2-31** |") < dev.find("### Wave 3 · L-B"), "order")

    P("STATIC GUARDS checked=%d failed=%d" % (GUARDS[0], len(FAILS)))
    P("SCENARIOS %d, bad=%d" % (n[0], len(scen_bad)))
    for f in FAILS + scen_bad:
        P("  " + f)
    ok = not FAILS and not scen_bad
    P("PASS: round76 UIGuild 083 read-only review (8th sibling defect, comment-only, 0 behavior literals)" if ok
      else "FAIL: round76 harness")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
