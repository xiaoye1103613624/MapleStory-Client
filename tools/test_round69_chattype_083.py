# -*- coding: utf-8 -*-
"""round69 D7 只读取证 harness：
A. 静态守卫 —— 083 真值证据落盘、零风险注释口径纠正、自造痕迹清除、回归守卫。
B. 场景 —— 用纯 Python 复刻 083 SetChatType/ChangeChatWndSize/StartChat/EndChat/
   UseFuncKeyMapped(case16)/OnGameStageChanged 的状态机真值表，逐条与反编译立即数比对。

行为字面量（chat_type 赋值 / CLASSIC_LOG_H_* 常量 / button_pressed 分支）本轮不动，
harness 显式守卫它们「仍在」，防止口径纠正被误做成行为变更。
"""
import json, os, re, sys

ROOT = "."
CPP = "IO/UITypes/UIChatBar.cpp"
H = "IO/UITypes/UIChatBar.h"
CFG = "Configuration.h"
NOTES = "docs/features/development-and-fix-plan-2026-11/notes/wave2-chattype-083-review-20261111.md"
EV = "docs/features/development-and-fix-plan-2026-11/evidence/chattype-083-round69.json"
IDA = "tools/_ida_chattype"

fails = []
checked = 0


def rd(p):
    with open(p, "r", encoding="utf-8", errors="replace") as fp:
        return fp.read()


def guard(name, cond):
    global checked
    checked += 1
    if not cond:
        fails.append(name)


def code_only(s):
    """去掉 // 行注释，只留代码（守卫用于「字面量仍在」判定）。"""
    return "\n".join(ln.split("//", 1)[0] for ln in s.split("\n"))


cpp = rd(CPP)
h = rd(H)
cfg = rd(CFG)
cpp_code = code_only(cpp)
h_code = code_only(h)

print("=" * 72)
print("A. 静态守卫")
print("=" * 72)

# --- A1 083 反编译取证逐字落盘 ---
for f in ["SetChatType.dec.txt", "ChangeChatWndSize.dec.txt", "OnButtonClicked.dec.txt",
          "StartChat.dec.txt", "EndChat.dec.txt", "UseFuncKeyMapped.dec.txt",
          "OnGameStageChanged.dec.txt", "OnCreate_CallSite.dec.txt",
          "xrefs_SetChatType.txt", "symbols_raw.txt", "decompile_summary.json"]:
    guard("A1 evidence " + f, os.path.isfile(os.path.join(IDA, f)))

setct = rd(os.path.join(IDA, "SetChatType.dec.txt")) if os.path.isfile(os.path.join(IDA, "SetChatType.dec.txt")) else ""
chg = rd(os.path.join(IDA, "ChangeChatWndSize.dec.txt")) if os.path.isfile(os.path.join(IDA, "ChangeChatWndSize.dec.txt")) else ""
obc = rd(os.path.join(IDA, "OnButtonClicked.dec.txt")) if os.path.isfile(os.path.join(IDA, "OnButtonClicked.dec.txt")) else ""
start = rd(os.path.join(IDA, "StartChat.dec.txt")) if os.path.isfile(os.path.join(IDA, "StartChat.dec.txt")) else ""
end = rd(os.path.join(IDA, "EndChat.dec.txt")) if os.path.isfile(os.path.join(IDA, "EndChat.dec.txt")) else ""
ufk = rd(os.path.join(IDA, "UseFuncKeyMapped.dec.txt")) if os.path.isfile(os.path.join(IDA, "UseFuncKeyMapped.dec.txt")) else ""
gsc = rd(os.path.join(IDA, "OnGameStageChanged.dec.txt")) if os.path.isfile(os.path.join(IDA, "OnGameStageChanged.dec.txt")) else ""

# --- A2 083 立即数逐字（反编译）---
guard("A2 SetChatType nType1 h24/835=513", "0x8d4c50" in setct and "= 24;" in setct and "= 513;" in setct)
guard("A2 SetChatType nType2 h24/835=486", "= 486;" in setct)
guard("A2 SetChatType nType3 v6<26||>507 → 70", "if ( v6 < 26 || v6 > 507 )" in setct and "= 70;" in setct)
guard("A2 SetChatType h%13==0 → +2", "v7 % 13 == 0" in setct and "v7 + 2" in setct)
guard("A2 SetChatType MakeCtrlEdit(0) nType1", "MakeCtrlEdit(this, a2: 0)" in setct)
guard("A2 SetChatType MakeCtrlEdit(1) nType2/3", setct.count("MakeCtrlEdit(this, a2: 1)") >= 2)
guard("A2 scrollbar id 1010", "a3: 1010" in setct and "a3: 1010" in chg)
guard("A2 ChangeChatWndSize ±13", "(a4 == 1 ? 13 : -13)" in chg)
guard("A2 ChangeChatWndSize clamp 26/507", ">= 507" in chg and "< 26" in chg and "= 26;" in chg and "= 507;" in chg)
guard("A2 ChangeChatWndSize 509-h / CConfig+23", "509 - *(this + 831)" in chg and "+ 23" in chg)
guard("A2 OnButtonClicked 1003→1 / 1004→3", "case 0x3EBu" in obc and "nType: 1" in obc and "case 0x3ECu" in obc and "nType: 3" in obc)
guard("A2 OnButtonClicked 无 nType2 按钮", "nType: 2" not in obc)
guard("A2 StartChat 1→2", "nType: 2" in start and "result == 1" in start)
guard("A2 EndChat 2→1", "nType: 1" in end and "== 2" in end)
guard("A2 UseFuncKeyMapped case16 3→1 else 3", "0xa07905" in ufk and "0xa0790e" in ufk and ufk.count("nType: 3") >= 1 and ufk.count("nType: 1") >= 1)
guard("A2 OnGameStageChanged 2→1", "0xa04161" in gsc and "nType: 1" in gsc)

# --- A3 零风险注释口径纠正已落 ---
guard("A3 .h 不再称「原生 SetChatType 三态」", "原生 SetChatType 三态" not in h)
guard("A3 .h 标注 26..507 只在 nType3", "26..507 只在 nType3" in h)
guard("A3 .h 标注 083 无紧凑固定高度", "083 没有「紧凑态固定高度」" in h)
guard("A3 .h 标注 nType2 焦点驱动不持久", "焦点驱动, 不持久" in h and "非 083 原生 SetChatType" in h)
guard("A3 .cpp 展开侧改「本端 s9 适配」", "本端展开侧（s9 适配）" in cpp)
guard("A3 .cpp 收起侧改「本端 s9 适配」", "本端收起侧（s9 适配）" in cpp)
guard("A3 .cpp set_classic_log 标注非 083", "s9 适配；083 的 nType2 是" in cpp)
guard("A3 Configuration.h 标注非 083 原生", "非 083 原生 SetChatType" in cfg)
guard("A3 决策点 D7-A 已登记", os.path.isfile(NOTES) and "D7-A" in rd(NOTES))
guard("A3 evidence JSON 已落", os.path.isfile(EV))

# --- A4 行为字面量逐字不动（本轮只纠注释，不改状态机）---
guard("A4 CLASSIC_LOG_H_DEFAULT=80", "static constexpr int16_t CLASSIC_LOG_H_DEFAULT = 80;" in h_code)
guard("A4 CLASSIC_LOG_H_MIN=26", "static constexpr int16_t CLASSIC_LOG_H_MIN = 26;" in h_code)
guard("A4 CLASSIC_LOG_H_MAX=507", "static constexpr int16_t CLASSIC_LOG_H_MAX = 507;" in h_code)
guard("A4 CLASSIC_LOG_H_COMPACT=120", "static constexpr int16_t CLASSIC_LOG_H_COMPACT = 120;" in h_code)
guard("A4 LOG_LINE_H=13", "static constexpr int16_t LOG_LINE_H = 13;" in h_code)
guard("A4 chat_type=3 初值", "int8_t chat_type = 3;" in h_code)
guard("A4 BtMax chat_type==2 分支仍在", "if (chat_type == 2)" in cpp_code)
guard("A4 BtMin chat_type==3 分支仍在", "if (chat_type == 3)" in cpp_code)
guard("A4 set_classic_log 三态赋值仍在", "chat_type = 1;" in cpp_code and "chat_type = 3;" in cpp_code)
guard("A4 ChatType setting key chat.type 仍在", 'ShortEntry("chat.type", "3")' in cfg)
guard("A4 真按钮根 Basic.img/BtMax|BtMin 仍在", 'basic["BtMax"]' in cpp_code and 'basic["BtMin"]' in cpp_code)

# --- A5 真资源根经门禁 + .nx 实测（evidence 记录）---
ev = json.loads(rd(EV)) if os.path.isfile(EV) else {}
gate = ev.get("orange_wz_gate", {})
guard("A5 orange-wz 结构层 WORKS", gate.get("structure") == "WORKS")
guard("A5 内容层 IMG 解析失败留证", "IMG" in str(gate.get("content_error", "")) and "StatusBar.img" in str(gate.get("content_error", "")))
guard("A5 65536B GMS key 规格", gate.get("key_raw_len") == 65536 and gate.get("key_sha256_16") == "268a8407a47ee76c")
guard("A5 .nx Basic.img BtMax/BtMin 真存在", ev.get("nx_facts", {}).get("Basic_BtMax") is True and ev.get("nx_facts", {}).get("Basic_BtMin") is True)
guard("A5 .nx StatusBar.img/chat 不存在", ev.get("nx_facts", {}).get("StatusBar_chat") is False)

# --- A6 跨面回归：LoginBackground 线性插值（按 Angel 修过，前几轮保留）---
lb = code_only(rd("IO/LoginBackground.cpp"))
guard("A6 LoginBackground 线性插值仍在", "t * t * (3.0" not in lb and "tween_duration" in lb)
guard("A6 无 095 依据字面量", "Maplestory95" not in (cpp + h + cfg) and "port-095" not in (cpp + h + cfg))

print("STATIC GUARDS checked=%d failed=%d" % (checked, len(fails)))

print("=" * 72)
print("B. 083 状态机真值表场景（纯模型，逐条对齐反编译立即数）")
print("=" * 72)

# 083 CUIStatusBar 的字段（以 this+831 = h, this+833 = rows, this+835 = 509-h,
# this+832 = nType, CConfig+23 = 持久化高度）
class SB083:
    def __init__(self, cfg_h=80):
        self.nType = 3
        self.h = 0
        self.rows = 0
        self.top = 0  # this+835
        self.cfg_h = cfg_h
        self.edit = None
        self.set_chat_type(3)

    def set_chat_type(self, n):
        if n == self.nType and self.h != 0:
            return
        self.nType = n
        if n == 1:
            self.h, self.rows, self.top, self.edit = 24, 1, 513, 0
        elif n == 2:
            self.h, self.rows, self.top, self.edit = 24, 1, 486, 1
        elif n == 3:
            v = self.cfg_h
            self.h = 70 if (v < 26 or v > 507) else v
            self.rows = self.h // 13
            if self.h % 13 == 0:
                self.h += 2
            self.top = 509 - self.h
            self.edit = 1
        self.ensure_scroll()

    def ensure_scroll(self):
        # 只记录 id 1010 滚动条存在性
        self.scroll_id = 1010

    def change_size(self, delta13):
        # ChangeChatWndSize @0x8d616c（逐字）：
        #   a4==1 时 v11 = h>=507 → 早退；a4==0 时 v11 = h<26 → 早退；
        #   v9 = h ± 13；if v9>=507 → 507；elif v9<=26 → 26；
        #   rows = h/13（在 +2 之前算）；if h%13==0 → h += 2；top = 509-h；写回 CConfig+23。
        v10 = self.h
        if delta13 > 0 and v10 >= 507:
            return False
        if delta13 < 0 and v10 < 26:
            return False
        v9 = v10 + (13 if delta13 > 0 else -13)
        if v9 >= 507:
            h = 507
        elif v9 <= 26:
            h = 26
        else:
            h = v9
        self.rows = h // 13
        if h % 13 == 0:
            h += 2
        self.h = h
        self.top = 509 - h
        self.cfg_h = h
        self.edit = 1
        return True

    def start_chat(self):
        if self.nType == 1:
            self.set_chat_type(2)
        return self.nType

    def end_chat(self):
        if self.nType == 2:
            self.set_chat_type(1)
        return self.nType

    def funckey16(self):
        if self.nType == 3:
            self.set_chat_type(1)
        else:
            self.set_chat_type(3)
        return self.nType

    def stage_changed(self):
        if self.nType == 2:
            self.set_chat_type(1)
        return self.nType


scen = []
def S(name, cond):
    scen.append((name, bool(cond)))

# S1-S3 三态初值
s = SB083(cfg_h=80)
S("S1 nType3 h=80 rows=6 top=429 edit=1", (s.nType, s.h, s.rows, s.top, s.edit) == (3, 80, 6, 429, 1))
s.set_chat_type(1)
S("S2 nType1 h=24 rows=1 top=513 edit=0", (s.nType, s.h, s.rows, s.top, s.edit) == (1, 24, 1, 513, 0))
s.set_chat_type(2)
S("S3 nType2 h=24 rows=1 top=486 edit=1", (s.nType, s.h, s.rows, s.top, s.edit) == (2, 24, 1, 486, 1))

# S4 nType3 越界回落 70（不是 clamp）
s = SB083(cfg_h=10)
S("S4 cfg=10 → 回落 h=70 rows=5 top=439", (s.nType, s.h, s.rows, s.top) == (3, 70, 5, 439))
s = SB083(cfg_h=600)
S("S5 cfg=600 → 回落 h=70", s.h == 70)

# S6 h%13==0 → +2
s = SB083(cfg_h=52)  # 52 % 13 == 0
S("S6 cfg=52 → h=54 rows=4 top=455", (s.h, s.rows, s.top) == (54, 4, 455))

# S7 nType3 合法 cfg=507
s = SB083(cfg_h=507)
# 083 边界怪异点（SetChatType @0x8d4aab / ChangeChatWndSize @0x8d627e 同款）：
# clamp 到 507 之后才判 h%13==0 → h += 2，得到 509（越过 507 上限），top = 509-509 = 0。
# 26 同理 → 28。写进 evidence/notes，不能按「想当然的 clamp 稳定值」造假绿。
S("S7 cfg=507 → h=509 rows=39 top=0（clamp 后 +2 越界）", (s.h, s.rows, s.top) == (509, 39, 0))

# S8-S10 ChangeChatWndSize ±13 + clamp
s = SB083(cfg_h=80)
s.change_size(+1)
S("S8 +13 → h=93 rows=7", (s.h, s.rows) == (93, 7))
s.change_size(-1)
S("S9 -13 → h=80", s.h == 80)
for _ in range(40):
    s.change_size(+1)
S("S10 连续加 → 稳定 509（507+2）", s.h == 509)
for _ in range(60):
    s.change_size(-1)
S("S11 连续减 → 稳定 28（26+2）", s.h == 28)
s2 = SB083(cfg_h=507)  # h 已 509（clamp 后 +2 越界）
S("S12 拖到 509 后 +1 早退（083 v10>=507 守卫）", s2.change_size(+1) is False and s2.h == 509)

# S13-S15 StartChat/EndChat 焦点驱动
s = SB083(); s.set_chat_type(1)
S("S13 StartChat 1→2", s.start_chat() == 2 and s.edit == 1)
S("S14 EndChat 2→1", s.end_chat() == 1 and s.edit == 0)
s3 = SB083()  # nType3
S("S15 StartChat 在 nType3 不变 2", s3.start_chat() == 3)

# S16-S17 funckey16 双向
s = SB083()  # 3
S("S16 case16 3→1", s.funckey16() == 1)
S("S17 case16 再按 1→3", s.funckey16() == 3)

# S18 OnGameStageChanged 2→1，3 不动
s = SB083(); s.set_chat_type(2)
S("S18 切图 2→1", s.stage_changed() == 1)
s = SB083()  # 3
S("S19 切图 3 不动", s.stage_changed() == 3)

# S20 nType2 不由按钮触发（OnButtonClicked 只有 1/3）
S("S20 083 按钮仅 1/3，nType2 焦点瞬态", "nType: 2" not in obc)

for name, ok in scen:
    print(("  OK  " if ok else "  FAIL") + " " + name)
bad = [n for n, ok in scen if not ok]
print("SCENARIOS %d, bad=%d" % (len(scen), len(bad)))

ok = len(fails) == 0 and len(bad) == 0
if fails:
    print("FAILED GUARDS:", fails)
if bad:
    print("FAILED SCENARIOS:", bad)
print("PASS: round69 chat-type 083 read-only review" if ok else "FAIL")
sys.exit(0 if ok else 1)
