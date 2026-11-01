#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Generate docs/features/port-095/notes/opcode-v95.md from IDA decompilation artifacts.

Inputs (all in this directory):
  dec_processpacket.c, dec_cwvscontext_onpacket.c, dec_cfield_onpacket.c,
  dec/*.c, dec_maps.json, cfield_map.json, dec_send/*.json
Output:
  <repo>/docs/features/port-095/notes/opcode-v95.md
"""
import io
import json
import os
import re
import sys

BASE = os.path.dirname(os.path.abspath(__file__))
OUT = r"E:\project\MapleStory-Client\docs\features\port-095\notes\opcode-v95.md"

CASE_RE = re.compile(r"^\s*case\s+(\d+)\s*:")
CALL_RE = re.compile(r"([A-Za-z_][\w:<>~]*::[A-Za-z_]\w*)\s*\(")
LABEL_RE = re.compile(r"^(LABEL_\d+|\$LN\d+_\d+|[A-Za-z_]\w*)\s*:\s*(/\*.*\*/)?\s*$")


def read(path):
    with open(os.path.join(BASE, path), "r", encoding="utf-8") as f:
        return f.read()


def strip_comments(code):
    code = re.sub(r"/\*.*?\*/", " ", code, flags=re.S)
    code = re.sub(r"//[^\n]*", " ", code)
    return code


HELPERS = (
    "CInPacket::", "COutPacket::", "StringPool::", "ZAllocEx", "ZRef<", "ZXString",
    "_CxxThrowException", "CMSException::", "CUtilDlg::Notice", "CWnd::", "CCtrl",
    "CDialog::", "CUtilDlgEx::", "CUIFadeYesNo::", "TSingleton", "ZArray", "ZList",
    "::operator", "CWvsContext::SetNewFadeWnd", "get_tab_from_item_typeindex",
)


def parse_cases(code, default_label="无处理"):
    """Return [(labels, handler, kind)] for every case inside every switch block."""
    code = strip_comments(code)
    out = []
    for sm in re.finditer(r"\bswitch\s*\(", code):
        i = code.index("{", sm.end()) if "{" in code[sm.end():] else -1
        if i < 0:
            continue
        depth, j = 1, i + 1
        while j < len(code) and depth:
            if code[j] == "{":
                depth += 1
            elif code[j] == "}":
                depth -= 1
            j += 1
        block = code[i + 1:j - 1]
        labels, state = [], "label"
        for raw in block.splitlines():
            line = raw.strip()
            if not line:
                continue
            m = CASE_RE.match(line)
            if m:
                labels.append(int(m.group(1)))
                state = "label"
                continue
            if line.startswith("default"):
                state = "default"
                continue
            if state == "label":
                if line.startswith(("if ", "switch")):
                    continue
                if "+ 44))(" in line or (line.startswith("((") and "vfptr[" in line):
                    out.append((labels, "[virtual]", "virtual"))
                    labels, state = [], "done"
                    continue
                c = CALL_RE.search(line)
                if c:
                    if any(c.group(1).startswith(h) for h in HELPERS):
                        continue
                    out.append((labels, c.group(1), "call"))
                    labels, state = [], "done"
                    continue
                if line.startswith(("break;", "return;", "continue;")):
                    out.append((labels, default_label, "none"))
                    labels, state = [], "done"
                    continue
    return out


def table(rows, headers):
    txt = ["| " + " | ".join(headers) + " |", "|" + "|".join(["---"] * len(headers)) + "|"]
    for r in rows:
        txt.append("| " + " | ".join(str(x) for x in r) + " |")
    return "\n".join(txt)


def opcode_rows(cases, prefix=""):
    rows = []
    for labels, handler, kind in cases:
        for n in labels:
            name = handler
            if kind == "virtual":
                name = "→ vtable 虚调用（本次确认为 CField::OnClock）"
            rows.append((n, prefix + name))
    rows.sort(key=lambda r: r[0])
    return rows


def merge_ranges(pairs):
    """[(lo,hi,handler)] merged where adjacent & identical handler."""
    pairs = sorted(set(pairs))
    out = []
    for lo, hi, h in pairs:
        if out and out[-1][2] == h and out[-1][1] + 1 == lo:
            out[-1] = (out[-1][0], hi, h)
        else:
            out.append((lo, hi, h))
    return out


def rng(lo, hi):
    return str(lo) if lo == hi else "%d–%d" % (lo, hi)


def main():
    out = io.StringIO()
    w = out.write

    # ---------- top level ----------
    pp = parse_cases(read("dec_processpacket.c"))
    w("# v95 Opcode 表（GMS v95 客户端静态导出）\n\n")
    w("> 证据源：`E:\\MXD\\Maplestory95.i64`（未加壳 GMS v95 客户端 + 符号），IDA Pro 9.4 / ida-pro-mcp（13338）。\n")
    w("> 生成脚本：`E:\\project\\_ms_probe\\gen_opcode_doc.py`；原始反编译见 `docs/features/port-095/evidence/opcode-v95/`。\n")
    w("> 本表用于**语义参照**（字段顺序、二级分发、系统归属），不作为 095 中文客户端已确认协议——中文端为加壳二进制，需抓包校准（见 §7）。\n\n")
    w("## 1. 顶层：`CClientSocket::ProcessPacket` @ `0x4b00f0`\n\n")
    w("opcode 由 `CInPacket::Decode2` 读出（**uint16**），随后分流：\n\n")
    rows = []
    rows.append(("0–15", "stage → `CLogin::OnPacket`", "登录阶段"))
    for labels, handler, kind in pp:
        for n in labels:
            rows.append((str(n), "`%s`" % handler, "连接层特例"))
    rows.append(("21, 22, 24–27", "stage → `CLogin::OnPacket`", "登录阶段（CLogin 二级分发）"))
    rows.append(("28–140", "`CWvsContext::OnPacket`", "全局上下文（106 个 opcode）"))
    rows.append(("141–432", "stage → `CField::OnPacket` / `CCashShop::OnPacket`", "g_pStage 虚调用；地图内为 CField"))
    rows.append(("其他", "g_pStage->OnPacket（无处理则丢弃）", "—"))
    w(table(rows, ["opcode", "处理函数", "说明"]))
    w("\n\n关键证据：`dec_processpacket.c`；`(unsigned int)(v4 - 28) > 0x70` 即 28..140 归 CWvsContext，其余交 `g_pStage`。\n")

    # ---------- CWvsContext ----------
    ctx = parse_cases(read("dec_cwvscontext_onpacket.c"))
    w("\n## 2. `CWvsContext::OnPacket`（28–140，106 项）\n\n")
    ctx_rows = opcode_rows(ctx)
    w(table(ctx_rows, ["opcode", "处理函数"]))
    got = {r[0] for r in ctx_rows}
    gaps = [n for n in range(28, 141) if n not in got]
    w("\n\n未使用/无处理：%s（default 直接 return）。\n" % "、".join(str(g) for g in gaps))

    # ---------- CField ----------
    cf = read("dec_cfield_onpacket.c")
    cf_cases = parse_cases(cf)
    ranges = json.load(open(os.path.join(BASE, "cfield_map.json"), encoding="utf-8"))
    rpairs = [(r["lo"], r["hi"], r["handler"]) for r in ranges if not r["handler"].startswith("[case]")]
    rpairs = [(lo, hi, h) for lo, hi, h in rpairs if not (lo == hi and lo in {n for labels, _, _ in cf_cases for n in labels})]
    merged = merge_ranges(rpairs)
    w("\n## 3. `CField::OnPacket`（141–432）\n\n")
    w("### 3.1 区间路由（转交各子系统池/对话框）\n\n")
    w(table([(rng(lo, hi), "`%s`" % h) for lo, hi, h in merged], ["opcode 区间", "转交"]))
    w("\n\n### 3.2 直接 case（CField 自身处理）\n\n")
    own = opcode_rows(cf_cases)
    own = [r for r in own if r[0] != 432] + [(432, "CWvsContext::OnLogoutGift")]
    own.sort(key=lambda r: r[0])
    w(table(own, ["opcode", "处理函数"]))
    handled = {r[0] for r in own} | {n for lo, hi, _ in merged for n in range(lo, hi + 1)}
    unused = [n for n in range(141, 433) if n not in handled]
    w("\n\n### 3.3 141–432 中无处理/未使用\n\n")
    w("、".join(str(n) for n in unused) + "\n")
    w("\n其中 382–396 在**商城阶段**由 `CCashShop::OnPacket` 处理（见 §4.20），CField 本身忽略；196 命中后直接 `return`；163 为 CField 虚函数 `OnClock`（子操作码 0=事件倒计时 / 1=设置时刻 / 2=创建时钟 / 3=停止，见 `dec_clock.json`）。\n")
    w("\n注：179–277 除 196 外整体交 `CUserPool`。\n")

    # ---------- second level ----------
    w("\n## 4. 二级分发\n\n")
    maps = json.load(open(os.path.join(BASE, "dec_maps.json"), encoding="utf-8"))

    def dump_map(fname, title, note="", drop_hi=False):
        rows = []
        for lo, hi, h in maps.get(fname, []):
            if lo == 0 and hi == 65535:
                continue
            if drop_hi and hi == 65535:
                continue
            rows.append((rng(lo, hi), "`%s`" % h))
        rows.sort(key=lambda r: int(r[0].split("–")[0]))
        w("\n### %s\n\n" % title)
        if note:
            w(note + "\n\n")
        w(table(rows, ["子 opcode", "处理函数"]) + "\n")

    dump_map("_OnPacket_CUserPool__QAEXJAAVCInPacket___Z.c", "4.1 CUserPool::OnPacket（179–276）")
    dump_map("_OnUserRemotePacket_CUserPool__IAEXJAAVCInPacket___Z.c", "4.2 CUserRemote::OnPacket（210–230）",
             note="两次 switch：211–214 共用 `OnAttack(nType)`；210/222/223/225–229 在“延迟加载/切换中”分支里仍会处理（第二条 switch 用同一 nType）。")
    w("\n### 4.3 CUserCommon::OnPacket（181–209）\n\n")
    w("先 `Decode4` 取角色 ID 查 `CUserPool::GetUser`；nType=182 且角色存在时按 181 处理（自己发的聊天）。\n\n")
    w(table([("181", "`CUser::OnChat(bIsFromOutsideOfMap=0)`"),
             ("182", "`CUser::OnChat(bIsFromOutsideOfMap=1)`"),
             ("183", "`CUser::OnADBoard`"), ("184", "`CUser::OnMiniRoomBalloon`"),
             ("185", "`CUser::SetConsumeItemEffect`"), ("186", "`CUser::ShowItemUpgradeEffect`"),
             ("187", "`CUser::ShowItemHyperUpgradeEffect`"), ("188", "`CUser::ShowItemOptionUpgradeEffect`"),
             ("189", "`CUser::ShowItemReleaseEffect`"), ("190", "`CUser::ShowItemUnreleaseEffect`"),
             ("191", "`CUser::OnHitByUser`"), ("192", "`CUser::OnTeslaTriangle`"),
             ("193", "`CUser::OnFollowCharacter`"), ("194", "`CUser::OnShowPQReward`"),
             ("195", "`CUser::OnSetPhase`"), ("197", "`CUser::ShowRecoverUpgradeCountEffect`"),
             ("198–205", "`CUser::OnPetPacket`"), ("206–208", "`CUser::OnDragonPacket`"),
             ("196, 209", "无处理")],
            ["子 opcode", "处理函数"]))
    dump_map("_OnPacket_CUserLocal__QAEXJAAVCInPacket___Z.c", "4.4 CUserLocal::OnPacket（231–276）", "CUserPool 在 231–276 直接转 `CUserLocal::OnPacket`（单参数版本），下表为该函数内部 switch。")
    dump_map("_OnPacket_CMobPool__QAEXJAAVCInPacket___Z.c", "4.5 CMobPool::OnPacket（284–310）")
    dump_map("_OnMobPacket_CMobPool__IAEXJAAVCInPacket___Z.c", "4.6 CMob::OnPacket（287–309）")
    dump_map("_OnPacket_CNpcPool__QAEXJAAVCInPacket___Z.c", "4.7 CNpcPool::OnPacket（311–318）")
    dump_map("_OnNpcPacket_CNpcPool__IAEXJAAVCInPacket___Z.c", "4.8 CNpc::OnPacket（314–316）")
    dump_map("_OnNpcTemplatePacket_CNpcPool__IAEXJAAVCInPacket___Z.c", "4.9 CNpcTemplate::OnPacket（317）")
    dump_map("_OnPacket_CSummonedPool__QAEXJAAVCInPacket___Z.c", "4.10 CSummonedPool（278–283）")
    dump_map("_OnPacket_CEmployeePool__QAEXJAAVCInPacket___Z.c", "4.11 CEmployeePool（319–321）")
    dump_map("_OnPacket_CDropPool__QAEXJAAVCInPacket___Z.c", "4.12 CDropPool（322–324）")
    dump_map("_OnPacket_CMessageBoxPool__QAEXJAAVCInPacket___Z.c", "4.13 CMessageBoxPool（325–327）")
    dump_map("_OnPacket_CAffectedAreaPool__QAEXJAAVCInPacket___Z.c", "4.14 CAffectedAreaPool（328–329）")
    dump_map("_OnPacket_CTownPortalPool__QAEXJAAVCInPacket___Z.c", "4.15 CTownPortalPool（330–331）")
    dump_map("_OnPacket_COpenGatePool__QAEXJAAVCInPacket___Z.c", "4.16 COpenGatePool（332–333）")
    dump_map("_OnPacket_CReactorPool__QAEXJAAVCInPacket___Z.c", "4.17 CReactorPool（334–337）")
    dump_map("_OnPacket_CMapLoadable__UAEXJAAVCInPacket___Z.c", "4.18 CMapLoadable（144–146）")
    dump_map("_OnPacket_CLogin__UAEXJAAVCInPacket___Z.c", "4.19 CLogin（0–27，含 141–146 转发）")
    dump_map("_OnPacket_CCashShop__UAEXJAAVCInPacket___Z.c", "4.20 CCashShop（382–396）",
             note="392/393 共用 `OnCashItemGachaponResult`；389、394 无处理。")
    dump_map("_OnPacket_CScriptMan__QAEXJAAVCInPacket___Z.c", "4.21 CScriptMan（363）")
    dump_map("_OnPacket_CUIMessenger__SAXAAVCInPacket___Z.c", "4.22 CUIMessenger（372）",
             note="子码 3 = `OnInvite`（在 switch 之前单独判断）。")
    dump_map("_OnPacketBase_CMiniRoomBaseDlg__SAXAAVCInPacket___Z.c", "4.23 CMiniRoomBaseDlg::OnPacketBase（373）",
             note="无对话框实例时只处理 2/3/5/14（静态邀请/进入/SSN）；有实例时 3/4/6/9/10，其余交虚函数。")

    w("\n### 4.24 CRPSGameDlg（371）\n\n")
    w(table([("8", "打开对话框（`CDialog::DoModal`）"), ("6, 7, 9–12, 14", "`CRPSGameDlg::ProcessPacket`"),
             ("13", "`CWnd::Destroy`")], ["子 opcode", "行为"]))
    w("\n\n### 4.25 CParcelDlg（381）\n\n")
    w(table([("8", "`SetParcelDlg`（打开包裹对话框，Decode1 决定 nMode=2/0）"),
             ("23", "`RemoveParcel(SN)` + 结果提示（3=删除成功，其他=领取成功）"),
             ("24", "`PARCEL::Decode` + `AddNewParcel` + 新包裹提示"),
             ("25", "快速投递提醒：`DecodeStr(sender)` + `Decode1` + `CUIFadeYesNo::CreateParcelAlarm`"),
             ("26", "新建空白对话框 `CParcelDlg(nMode=1)`"),
             ("27", "`Decode1` + `CUIFadeYesNo::CreateParcelAlarm`"),
             ("其他(默认)", "`NoticeResult(nResult)` + `SetCtrlEnabled`；nResult=18 时按 nMode 关闭或 `ResetSendInfo`")],
            ["子 opcode", "行为"]))
    w("\n\n### 4.26 CTrunkDlg（368）\n\n")
    w(table([("9", "`CmpPutItem` 刷新“放入”列表并定位滚动条"),
             ("13, 15, 19", "`SetGetItems` + `InvalidateRect`（取回列表）"),
             ("22", "打开仓库对话框 `SetTrunkDlg`"),
             ("10", "提示：背包已满"),
             ("11, 16", "提示：金币不足"),
             ("12", "提示：该物品无法取回"),
             ("17", "提示：仓库已满"),
             ("23", "提示：物品/金币无法移动，请联系客服"),
             ("24", "`Decode1`；非 0 则 `DecodeStr` 后提示，否则“交易失败”"),
             ("其他(默认)", "提示：由于错误，交易未发生")],
            ["子 opcode", "行为"]))
    dump_map("_OnPacket_CFuncKeyMappedMan__QAEXJAAVCInPacket___Z.c", "4.27 CFuncKeyMappedMan（398–400）")
    dump_map("_OnPacket_CMapleTVMan__QAEXJAAVCInPacket___Z.c", "4.28 CMapleTVMan（404–409）")
    dump_map("_OnPacket_CBattleRecordMan__QAEXJAAVCInPacket___Z.c", "4.29 CBattleRecordMan（420–423）",
             note="CField 把 420–423 交给本池，其中 420、423 无处理。")

    # shop dialogs: 包体由各 dialog 自己 Decode1 取子操作码，这里给出子码集合
    for fname, title, extra in (
        ("_OnPacket_CShopDlg__SAXJAAVCInPacket___Z.c", "4.30 CShopDlg（364 打开 / 365 子操作码）",
         "365 的子码：0（出售列表刷新，`CmpSellItem`）、1–5、8–10、13–19（购买/回退/结果提示，含 `CUtilDlg::Notice`）。"),
        ("_OnPacket_CAdminShopDlg__SAXJAAVCInPacket___Z.c", "4.31 CAdminShopDlg（366 打开 / 367 子操作码）",
         "367 的子码 1–11 经 `v18`/`v20` 两次 Decode1 展开，10/11 会弹 `CUtilDlgEx`（见反编译正文）。"),
        ("_OnPacket_CStoreBankDlg__SAXJAAVCInPacket___Z.c", "4.32 CStoreBankDlg（369 打开 / 370 子操作码）",
         "370 的子码为 30–34 与 35('#'/取钱) 36('$') 37('%') 38('&')（见反编译正文）。"),
    ):
        cases = parse_cases(read(os.path.join("dec", fname)))
        subs = sorted({n for labels, _, _ in cases for n in labels})
        w("\n### %s\n\n" % title)
        w("子操作码：%s。%s原始反编译：`evidence/opcode-v95/dec/%s`。\n"
          % ("、".join(str(s) for s in subs) if subs else "未自动提取", extra, fname))

    # ---------- splice ----------
    # §5（发送表）由 gen_send_section.py 生成，§6–§8 为人工维护；
    # 本脚本只重写文件头 + §1–§4，避免覆盖后续章节，也不再写 send_ops.json（由 merge_send.py 负责）。
    head = out.getvalue()
    md = io.open(OUT, encoding="utf-8").read()
    i = md.find("## 5. 发送")
    if i < 0:
        i = len(md)
    merged = head + "\n" + md[i:]
    io.open(OUT, "w", encoding="utf-8", newline="\n").write(merged)
    print("rows: ctx=%d cfield=%d; doc %d -> %d lines"
          % (len(ctx_rows), len(own), md.count("\n") + 1, merged.count("\n") + 1))


if __name__ == "__main__":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")
    main()
