# -*- coding: utf-8 -*-
"""round67 harness: Angel 083 CUIStatusBar::SetButtonBlink 状态机重修验收。
静态守卫（源码级）+ 场景（083 状态机语义级），不依赖 C++ 编译。
验收信号：STATIC GUARDS failed=0 / SCENARIOS bad=0 / PASS。"""
import re, sys, io

H = "IO/UITypes/UIStatusBar.h"
C = "IO/UITypes/UIStatusBar.cpp"
LB = "IO/LoginBackground.cpp"

def read(p):
    with open(p, "r", encoding="utf-8", errors="replace") as f:
        return f.read()

def has(text, pattern, flags=0):
    return re.search(pattern, text, flags) is not None

def main():
    h = read(H)
    c = read(C)
    lb = read(LB)
    guards = []

    def g(name, ok):
        guards.append((name, bool(ok)))

    # ---- A. 头文件：真动画成员 + 083 常量 + 三方法 ----
    g("A1 include Animation.h", has(h, r'#include\s+"\.\./\.\./Graphics/Animation\.h"'))
    g("A2 include cstdint", has(h, r"#include\s+<cstdint>"))
    g("A3 std::array<Animation,4> blink_ani", has(h, r"std::array<Animation,\s*4>\s*blink_ani"))
    g("A4 std::array<int64_t,4> blink_elapsed_ms 初始 -1", has(h, r"std::array<int64_t,\s*4>\s*blink_elapsed_ms\s*=\s*\{\s*-1,\s*-1,\s*-1,\s*-1\s*\}"))
    g("A5 BLINK_TIMEOUT_MS = 90000", has(h, r"BLINK_TIMEOUT_MS\s*=\s*90000"))
    g("A6 BLINK_LEVEL_LIMIT = 10", has(h, r"BLINK_LEVEL_LIMIT\s*=\s*10"))
    g("A7 blink_last_level 成员", has(h, r"int16_t\s+blink_last_level\s*=\s*0"))
    g("A8 start_button_blink 声明", has(h, r"void\s+start_button_blink\(uint16_t\s+id\)"))
    g("A9 stop_button_blink 声明", has(h, r"void\s+stop_button_blink\(uint16_t\s+id\)"))
    g("A10 blink_index 声明", has(h, r"int32_t\s+blink_index\(uint16_t\s+id\)\s*const"))
    g("A11 注释引 083 EA 0x8dfe21", has(h, r"0x8dfe21"))
    g("A12 注释引 OnStatChanged 0xa1fce9", has(h, r"0xa1fce9"))

    # ---- B. 旧自造实现痕迹必须清除 ----
    g("B1 无 blink_frames", not has(h, r"blink_frames") and not has(c, r"blink_frames"))
    g("B2 无 BLINK_DURATION", not has(h, r"BLINK_DURATION") and not has(c, r"BLINK_DURATION"))
    g("B3 无 ColorBox 白块叠层", not has(c, r"ColorBox\s+hl"))
    g("B4 无 7.5Hz 注释", not has(c, r"7\.5Hz"))
    g("B5 无『本包无.*KEY_ANI 资源』误判注释", not has(h, r"本包无[\s\S]{0,40}KEY_ANI") and not has(c, r"本包无[\s\S]{0,40}KEY_ANI"))
    g("B6 无『脉冲高亮』自造描述", not has(h, r"脉冲高亮") and not has(c, r"脉冲高亮"))

    # ---- C. cpp：4 组真 KEY_ANI 动画构造 ----
    for key in ("EquipKey", "InvenKey", "StatKey", "SkillKey"):
        g(f"C1 {key} Animation(sb[...ani])", has(c, rf'Animation\(sb\["{key}"\]\["ani"\]\)'))
    g("C2 4 处开子窗改 stop_button_blink", len(re.findall(r"\bstop_button_blink\(id\)", c)) == 4)
    g("C3 开子窗分支内不再 start_button_blink", not has(c, r"emplace<[\s\S]{0,200}?start_button_blink\(id\)"))

    # ---- D. 状态机方法定义 ----
    g("D1 blink_index 定义 0/1/2/3", has(c, r"int32_t\s+UIStatusBar::blink_index\([\s\S]{0,400}?return 0;[\s\S]{0,200}?return 1;[\s\S]{0,200}?return 2;[\s\S]{0,200}?return 3;"))
    g("D2 start 设 elapsed=0 + reset", has(c, r"void\s+UIStatusBar::start_button_blink[\s\S]{0,400}?blink_elapsed_ms\[idx\]\s*=\s*0;[\s\S]{0,120}?blink_ani\[idx\]\.reset\(\)"))
    g("D3 stop 设 elapsed=-1", has(c, r"void\s+UIStatusBar::stop_button_blink[\s\S]{0,300}?blink_elapsed_ms\[idx\]\s*=\s*-1;"))

    # ---- E. draw 用真帧，不再白块 ----
    g("E1 draw 调 blink_ani[i].draw", has(c, r"blink_ani\[i\]\.draw\(DrawArgument\("))
    g("E2 draw 守卫 blink_elapsed_ms[i] < 0", has(c, r"if \(blink_elapsed_ms\[i\] < 0\)[\s\S]{0,80}?continue;"))

    # ---- F. update：90s 超时 + 动画推进 + 升级启动 ----
    g("F1 elapsed += Constants::TIMESTEP", has(c, r"blink_elapsed_ms\[i\]\s*\+=\s*Constants::TIMESTEP"))
    g("F2 blink_ani[i].update()", has(c, r"blink_ani\[i\]\.update\(\)"))
    g("F3 >= BLINK_TIMEOUT_MS 清除为 -1", has(c, r"blink_elapsed_ms\[i\]\s*>=\s*BLINK_TIMEOUT_MS[\s\S]{0,120}?blink_elapsed_ms\[i\]\s*=\s*-1"))
    g("F4 升级且 <=LEVEL_LIMIT 触发 STAT start", has(c, r"blink_level\s*>\s*blink_last_level[\s\S]{0,120}?blink_level\s*<=\s*BLINK_LEVEL_LIMIT[\s\S]{0,120}?start_button_blink\(Buttons::BT_CLASSIC_STAT\)"))
    g("F5 blink_last_level 每帧同步", has(c, r"blink_last_level\s*=\s*blink_level"))

    # ---- G. 旁证：LoginBackground 线性插值按 Angel 保留 ----
    g("G1 LoginBackground 仍是线性 t（无 smoothstep）", has(lb, r"tween_from\.x\s*\+\s*\(tween_to\.x\s*-\s*tween_from\.x\)\s*\*\s*t") and not has(lb, r"3\.0\s*-\s*2\.0\s*\*\s*t"))

    # ---- H. 注释引 .nx 实测 ----
    g("H1 cpp 注释含 .nx 实测", has(c, r"\.nx 实测") or has(c, r"\.nx[\s\S]{0,20}实测"))
    g("H2 cpp 注释含 SP_1986..1989", has(c, r"SP_1986\.\.1989"))

    bad = [n for n, ok in guards if not ok]
    for n, ok in guards:
        if not ok:
            print("  FAIL", n)
    print(f"STATIC GUARDS checked={len(guards)} failed={len(bad)}")
    if bad:
        sys.exit(1)

    # ================= 场景：083 状态机语义 =================
    TIMESTEP = 8
    TIMEOUT = 90000
    LEVEL_LIMIT = 10
    ANI_FRAME_MS = 500  # .nx 实测 ani/0, ani/1 各 delay 500

    class Blink:
        def __init__(self):
            self.elapsed = [-1, -1, -1, -1]  # 083 v8=-1 未启动
            self.last_level = 0
            self.ani_tick = [0, 0, 0, 0]     # 当前帧累计 ms，模拟 Animation::update 的 delay 递减
            self.ani_frame = [0, 0, 0, 0]    # 0/1 两帧

        def start(self, idx):
            if 0 <= idx < 4:
                self.elapsed[idx] = 0
                self.ani_tick[idx] = 0
                self.ani_frame[idx] = 0      # reset()

        def stop(self, idx):
            if 0 <= idx < 4:
                self.elapsed[idx] = -1

        def open_child(self, idx):
            # 083 OnButtonClicked 2000-2003 / OnChildNotify a3==100：开子窗 = 清除
            self.stop(idx)

        def tick(self, level=None):
            # 083 Update：+90000 超时清除 + 动画帧推进
            for i in range(4):
                if self.elapsed[i] < 0:
                    continue
                self.elapsed[i] += TIMESTEP
                self.ani_tick[i] += TIMESTEP
                if self.ani_tick[i] >= ANI_FRAME_MS:
                    self.ani_tick[i] -= ANI_FRAME_MS
                    self.ani_frame[i] = 1 - self.ani_frame[i]
                if self.elapsed[i] >= TIMEOUT:
                    self.elapsed[i] = -1
            # 083 OnStatChanged：低等级升级 -> STAT 钮启动
            if level is not None:
                if self.last_level > 0 and level > self.last_level and level <= LEVEL_LIMIT:
                    self.start(2)
                self.last_level = level

        def visible(self, idx):
            return self.elapsed[idx] >= 0

    scenarios = []
    def s(name, ok, detail=""):
        scenarios.append((name, bool(ok), detail))

    # 1 初始全灭
    b = Blink()
    s("初态 4 钮全灭", all(not b.visible(i) for i in range(4)))

    # 2 启动 STAT（升级），可见
    b = Blink(); b.tick(level=1); b.tick(level=2)
    s("等级 1->2 升级启动 STAT", b.visible(2) and not b.visible(0) and not b.visible(3))

    # 3 开子窗清除（083 方向，与外部程序相反）
    b = Blink(); b.start(0); b.open_child(0)
    s("开子窗清除 blink（083 语义）", not b.visible(0))

    # 4 90s 超时清除
    b = Blink(); b.start(1)
    for _ in range(TIMEOUT // TIMESTEP - 1):
        b.tick()
    before = b.visible(1)
    b.tick()
    s("+90000ms 超时清除", before and not b.visible(1), f"before={before}")

    # 5 未超时不清除
    b = Blink(); b.start(3)
    for _ in range(1000):
        b.tick()
    s("1000 帧未到 90s 仍显示", b.visible(3))

    # 6 高等级升级不触发
    b = Blink(); b.tick(level=11); b.tick(level=12)
    s("等级 12 升级不触发（>0xA=10）", not b.visible(2))

    # 7 边界等级 10 触发、11 不触发
    b = Blink(); b.tick(level=9); b.tick(level=10)
    s("等级 9->10 边界触发", b.visible(2))
    b2 = Blink(); b2.tick(level=10); b2.tick(level=11)
    s("等级 10->11 边界不触发", not b2.visible(2))

    # 8 1Hz：delay=500ms、timestep=8ms → ceil(500/8)=63 帧翻一帧，两帧 1 循环。
    # Animation::update 用 delay -= timestep，到 0 才翻，故不能用整除 62 帧。
    half = -(-ANI_FRAME_MS // TIMESTEP)  # ceil
    b = Blink(); b.start(0)
    f0 = b.ani_frame[0]
    for _ in range(half):
        b.tick()
    f1 = b.ani_frame[0]
    for _ in range(half):
        b.tick()
    f2 = b.ani_frame[0]
    s("真 KEY_ANI 两帧 500ms 翻转", f0 == 0 and f1 == 1 and f2 == 0, f"frames {f0}->{f1}->{f2} half={half}")

    # 9 stop 后动画不再推进可见
    b = Blink(); b.start(2); b.tick(); b.stop(2); b.tick()
    s("stop 后不再可见", not b.visible(2))

    # 10 四钮独立
    b = Blink(); b.start(0); b.start(2); b.stop(0)
    s("四钮状态独立", not b.visible(0) and b.visible(2) and not b.visible(1) and not b.visible(3))

    # 11 升级只启动 STAT(idx2) 不误伤其它
    b = Blink(); b.start(0); b.tick(level=1); b.tick(level=2)
    s("升级启动 STAT 且保留 EQUIP", b.visible(0) and b.visible(2))

    # 12 多次超时幂等
    b = Blink(); b.start(1)
    for _ in range(TIMEOUT // TIMESTEP + 500):
        b.tick()
    s("超时后长期保持清除", not b.visible(1))

    sb = [(n, d) for n, ok, d in scenarios if not ok]
    for n, ok, d in scenarios:
        if not ok:
            print("  FAIL", n, d)
    print(f"SCENARIOS {len(scenarios)}, bad={len(sb)}")
    if sb:
        sys.exit(1)
    print("PASS: HUD button blink 083 state machine")
    return 0

if __name__ == "__main__":
    sys.exit(main())
