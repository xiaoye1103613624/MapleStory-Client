# -*- coding: utf-8 -*-
"""报告补 §10：左侧木牌栏补齐（用户 m02128「先做你可以改的」）。
原则：旧判词是修复前历史取证链，不抹掉，标注「修复前」+ 指向 §10；不签项其余不动。
每个 replace 断言唯一命中，全内存构建后一次写盘，保 UTF-8 + 原换行符。
"""
import os

ROOT = r"E:\project\MapleStory-Client"
P = os.path.join(ROOT, "docs", "features",
                 "login-flow-angel-calibration-2026-11", "TERM_VERIFY_REPORT.md")

SECTION10 = r"""

---

## 10. 左侧木牌栏补齐（用户 m02128「先做你可以改的」）

### 10.1 授权与自定验收信号（修复前先钉死，不事后放宽）

- 用户 m02125 给「我能直接接着干的」清单（照 `UIWorldSelect.cpp:158-163,306-324` 与 `UICommonCreation.cpp:492-534` 现成模式补选角色页整栏、职业团页 3 块 + 重建 + 真机逐块 NCC），m02128「先做你可以改的」= 明确开工授权。
- **自定验收信号**：选角色页 strip `0.4192 → ≥0.85`、职业团页 `0.5156 → ≥0.85`，4 个 y 块各 `>0.5`，帧 `real_gl_frame`。四条全部满足才签。

### 10.2 实现（4 个 TU，不新建文件 / 不改 vcxproj）

- **照抄现成模式**：`UICommonCreation.cpp:492-534` 的 classic 侧栏（该页 strip 已验证 0.9575，4 块齐）与 `UIWorldSelect.cpp:158-163,306-324` 三钮真资产。`LocalTwoSpriteButton` 用本地 `BtViewAll_{normal,pressed}.png`（各 5205 B，`docs/features/login-flow-ui-ida-align/resources/creation/`），失败回退 `MapleButton(Login["WorldSelect"]["BtViewAll"],…)`。
- **position 语义坑（照抄会画飞）**：最终落点 = `UIElement::position + 按钮局部坐标`（`MapleButton.cpp:76-83 draw` / `:91-105 bounds`；`Sprite.cpp:27-31`）。三页 position 不同：`UIWorldSelect=(0,0)` / `UICharSelect` step2 `=(375,1514)` / `UIRaceSelect` step3 `=(240,40)` / `UICommonCreation` classic `=(0,0)`。→ 装饰 Sprite 用 `draw(Point(0,0), inter)` 画屏幕绝对，按钮局部坐标 = `绝对 − position`（照 `UICharSelect.cpp:315 return screen - position` 已示范写法）。
- **step 帧号 / 数量用数值定死，不肉眼估**：`scripts/pick_step_sprite.py` ROI(0,30,160,92) 零均值 NCC：目标职业团 vs 本端建角(step/3) = 0.9096、目标职业团 vs 目标建角 = 0.9774、目标选角色 vs 本端建角 = 0.8337 → **两页统一 `Common/step/3`**（[`verdicts/step_sprite_pick.json`](evidence/term-verify-v2/verdicts/step_sprite_pick.json)）。`scripts/settle_plank_counts.py` 逐行暗像素投影：目标选角色/职业团页各 4 块 `[[36,85],[99,148],[377,399],[427,449]]`；修复前本端两页 `[]`（mean_dark 0.0459 / 0.1123，全空）（[`verdicts/settle_plank_counts.json`](evidence/term-verify-v2/verdicts/settle_plank_counts.json)）。orange-wz 128B GMS128 key 实读 `Common/step/2`、`/3`、`Common/selectWorld` 均 150×57 ARGB8888 有 PNG（[`scripts/pick_step_sprite.py`](evidence/term-verify-v2/scripts/pick_step_sprite.py)）。
- **落地**：`UICharSelect.h/.cpp` 加 `cs_step/cs_world_box/cs_world_name/cs_channel`，构造 step/3 (0,33) + selectWorld (0,96) + 世界名大写 + `"CH."+(wch+1)`，draw 传 (0,0)；`BtCharacter` → BtViewAll 本地 PNG（`LocalTwoSpriteButton`，atlas `CSBT`）于 `(0,377)−position`；新增专用枚举 `BtHome` → `Common["BtStart"]` 于 `(0,427)−position`。`UIRaceSelect.h/.cpp` 加 `rs_*` 四成员，`BtPreview` → BtViewAll（atlas `RSBT`）于 `(0,377)−position`、`BtStart` → `(0,427)−position`，classic `button_pressed(BtPreview)` → `ViewAllCharPacket().dispatch()`。

### 10.3 蓝军自检两轮（自我纠错留证，不藏）

- **自检 #1**：`UIRaceSelect.cpp` 装饰绘制被插在 `if(classic_text_gl.is_valid())` 单语句后且多缩进 2 格，功能上无条件执行但读起来像受 if 保护 → 改成同级 3-tab 对齐 + 注「无条件绘制」。
- **自检 #2（真实语义回归）**：第一版把 `UICharSelect` 的 `BtPreview` 贴图换成「回到首页」`Common/BtStart`(0,427)，但 `button_pressed(BtPreview)` 仍是「回选世界页 `set_step(1)`」且 ESC `send_key` 也调 `button_pressed(Buttons::BtPreview)`（`UICharSelect.cpp:835`）→ 贴图/行为/ESC 语义不一致，违反用户 m01638「点击后页面变化要清楚」。
- **自我纠错（避免过度修正）**：初拟给 `UIRaceSelect` 也插专用 `BtViewAll` 枚举是**半误判**——classic/modern 两 init 互斥、classic 下 `BtPreview` 本就空闲；且插在 `CLASS0` 前会 shift `CLASS0`(6→7)，而 modern 分支 `order[new_index+N-Buttons::CLASS0]`（`UIRaceSelect.cpp:812-838`）、`for(size_t i=0;i<=Buttons::CLASS0;i++)`(:364) 按绝对值依赖，会炸。→ 收窄**只改 UICharSelect**（其 `CHARACTER_SLOT0` 全是相对引用 `+i`/`-SLOT0`/`>=SLOT0`，grep 无硬编码 9，插入安全）。
- **收窄补丁 [`scripts/patch_sidebar2.py`](evidence/term-verify-v2/scripts/patch_sidebar2.py)**：新增 `BtHome`（仍 `< CHARACTER_SLOT0` 故 `send_cursor` 第一遍响应）→ `Sound(SCROLLUP).play(); Session::get().reconnect(); set_step(0); remove(CHARSELECT/LOGINWAIT/LOGINNOTICE); emplace<UILogin>();`（与 `UIWorldSelect.cpp:1350-1363` BtStart 一致）；恢复 `BtPreview` 原贴图/原逻辑/ESC。第一版见 [`scripts/patch_sidebar.py`](evidence/term-verify-v2/scripts/patch_sidebar.py)。
- **cosmetic**：[`scripts/fix_cosmetic_indent.py`](evidence/term-verify-v2/scripts/fix_cosmetic_indent.py) 补 3-tab 前导缩进（patch_sidebar2 的 `locate` 把 anchor 前导 tab 吃进去致 `lead=""`）；C++ 空白不敏感，纯可读性，已随最终构建验证。

### 10.4 构建口径（当前验收 exe）

- 最终确定性二进制 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` = sha256 `b10df425d42b4c12751e59f337376c7726761fe7b1fa5e0fe9b4f2649890fdeb` / 2026-11-14 03:54:11 / 5 109 760 B，`build_calibration.log` `BUILD_EXITCODE=0`、vcvarsall x64、零 error（`'vswhere.exe' is not recognized` 无害告警），构建命令 `MSYS_NO_PATHCONV=1 cmd.exe /c "E:\project\MapleStory-Client\build_calibration.bat"`；构建前 `taskkill /IM MapleStory.exe` 释放 exe 锁（保留 java 8484 服务端 pid 37592）。
- 三次构建收敛：第一版 4 文件补丁 → 收窄修正二次构建 → cosmetic 后最终构建；**§10.5 验收帧全部跑在最终 `b10df425…` 上**（`build_calibration.log` mtime 03:54:12 与 exe 03:54:11 相邻，验收脚本紧随其后）。仍是全量工作树产物，含别会话并发改动，非「只编本修复」的纯净二进制（§6.4 口径不变）。

### 10.5 真机逐块 NCC 验收（SIDEBAR_FIX_VERIFIED）

- 取帧链路沿用 §1：单实例 + `packet_dump.log` 起始 offset + `force_game_top` + 屏幕 DC `BitBlt`；登录双字段 (admin/admin) → 连接 (880,300) → 世界 (485,210) → CH.1 (490,350) → 到选择的世界去 (820,518) → 2.2 s settle 抓 [`shots/sidebar_fix_1_charselect.png`](evidence/term-verify-v2/shots/sidebar_fix_1_charselect.png) → BtNew (865,263) → 2.0 s 抓 [`shots/sidebar_fix_2_raceselect.png`](evidence/term-verify-v2/shots/sidebar_fix_2_raceselect.png)。脚本 [`scripts/verify_sidebar_fix.py`](evidence/term-verify-v2/scripts/verify_sidebar_fix.py)，证据 [`verdicts/sidebar_fix.json`](evidence/term-verify-v2/verdicts/sidebar_fix.json)。
- 对 `targets_desktop/desktop_{char_select,racecreate_race}.png` 算 ROI(0,0,160,470) 全条带零均值 NCC + 4 个 y 块：

| 页 | 修复前 strip | 修复后 strip | 4 块逐块（标题 / 世界条 / 查看所有角色 / 回到首页） | >0.5 块数 | 判定 |
| --- | --- | --- | --- | --- | --- |
| 选角色 UICharSelect | 0.4192 | **0.9313** | 0.8110 / 0.8039 / 0.9594 / 0.7164 | 4/4 | PASS |
| 职业团 UIRaceSelect | 0.5156 | **0.9508** | 1.0000 / 0.8246 / 0.9971 / 0.6543 | 4/4 | PASS |

- 两帧 `real_gl_frame=true`（选角色 mean 143.99 / std 77.6 / uniq 1369；职业团 mean 134.87 / std 76.7 / uniq 1113）；`config head3` 前后均 `b'# M'`；`verdict.all_pass=true` / `detail=SIDEBAR_FIX_VERIFIED`。
- **目视双证**：[`shots/sidefix_cmp_char.png`](evidence/term-verify-v2/shots/sidefix_cmp_char.png) / [`shots/sidefix_cmp_race.png`](evidence/term-verify-v2/shots/sidefix_cmp_race.png)（左本端 | 右目标，ROI(0,0,160,470) 3× 放大，[`scripts/make_sidebar_compare.py`](evidence/term-verify-v2/scripts/make_sidebar_compare.py)）经 `read_image` 一次一张确认四块木牌形制/位置逐一对上，无多图错配。

### 10.6 本节判词（candidate / 完成分离）

> **自定验收信号四条全部满足 → 选角色页 / 职业团页左侧木牌栏补齐签 `SIDEBAR_FIX_VERIFIED`**（数值 + 目视双证闭环）。**只签这一个子项**：§6 其余不签项一律未动——登录页 MSTitle 品牌/版本残留（§2.2.1）、角色属性板内容不一致（§2.3）、创建链路两端 0.8466 / 0.7186（§9.5，修复前历史帧）、CELL_H=32、18 px 行距、纯净校准二进制。影响面：`BtHome` 新增按钮点击逻辑与 `UIWorldSelect` BtStart 一致、`BtPreview`/ESC 原语义保留、未动 `CLASS0` 绝对值依赖、未新建文件、未改 vcxproj。体感是输入，源码行 + 数值 + 目视三证才是交付。
"""

REPLACES = [
    # 1. 顶部归档说明追加 §10 增补行
    ("（当前 exe `607f789811db25ed`）。",
     "（当前 exe `607f789811db25ed`）。\n>\n> **2026-11-14 03:5x 增补 §10**：用户 m02128「先做你可以改的」授权后，补齐选角色页 / 职业团页左侧木牌栏并真机逐块 NCC 验收 `SIDEBAR_FIX_VERIFIED`（选角色 0.4192→0.9313 / 职业团 0.5156→0.9508）；§9.1 / §9.6 的「整栏没实现」判词是修复前历史取证链，已在 §10 闭环。"),
    # 2. §0 追加当前验收 exe 条目（放在末条前）
    ("- **本报告是证据链 candidate**，不是「登录四屏像素级修复完成」盖章。四层分别签，不签的列残留。",
     "- **§10 验收二进制（最新）**：`E:\\MXD\\BeiDou-Client-xy\\MapleStory.exe` = sha256 前缀 `b10df425d42b4c127` / 2026-11-14 03:54:11 / 5 109 760 B，`BUILD_EXITCODE=0`；选角色页 / 职业团页侧栏补齐后的验收帧全部来自它（§10.4）。\n- **本报告是证据链 candidate**，不是「登录四屏像素级修复完成」盖章。四层分别签，不签的列残留。"),
    # 3. §6 第 1 条：把「整栏缺失」从四处未对齐里析出，标注已修复
    ("1. **「登录四屏像素级修复完成」不签**：登录页品牌/版本残留（§2.2）、角色属性板内容不一致（§2.3）、创建角色链路两端实测未对齐（§2.4 / §9.5）、选角色页左侧木牌整栏缺失（§9.1）四处明确未对齐。",
     "1. **「登录四屏像素级修复完成」不签**：登录页品牌/版本残留（§2.2）、角色属性板内容不一致（§2.3）、创建角色链路两端实测未对齐（§2.4 / §9.5）三处明确未对齐；~~选角色页 / 职业团页左侧木牌整栏缺失~~（§9.1 修复前判词）**已于 §10 补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`**（选角色 0.4192→0.9313 / 职业团 0.5156→0.9508，4/4 块过 0.5）。"),
    # 4. §8 结论表 UI 行（顺手同步「创建角色页无目标帧」旧口径）
    ("| UI | **部分闭环**：选世界收起 0.976、频道格 CELL_W=94 当前 HEAD 复测闭环、登录页结构同位（投影 0.957/0.891）但品牌内容残留、详情板 215=215 但内容不一致、创建角色页本端到手无目标帧、进图状态不可比 | 强（当前 HEAD 真 GL 帧） |",
     "| UI | **部分闭环**：选世界收起 0.976、频道格 CELL_W=94 复测闭环、**左侧木牌栏选角色/职业团页补齐并真机逐块验收（§10，0.9313 / 0.9508）**；仍残留登录页品牌/版本、详情板内容不一致、创建链路两端 0.8466 / 0.7186 未对齐、进图状态不可比 | 强（当前 HEAD 真 GL 帧） |"),
    # 5. §8 一句话答案末尾追加 m02128 答复
    ("口径不拿来当修复。",
     "口径不拿来当修复。**另补 m02128「先做你可以改的」：选角色页 / 职业团页左侧木牌栏已补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`，4/4 块过 0.5（§10）。**"),
    # 6. §9.1 表后加修复前/后注记（在 §9.2 标题前）
    ("### 9.2 点击逻辑表（全部源码行锚）",
     "> **注（§10 更新）**：上表选角色 0.4192 / 职业团 0.5156 是**修复前**帧的取证数值，作为历史证据链保留；用户 m02128 授权补齐后，同一 ROI、同一目标帧、同一零均值 NCC 算法复测为 0.9313 / 0.9508（4/4 块过 0.5，`SIDEBAR_FIX_VERIFIED`），见 §10.5。下方「未实现的是选角色页与职业团页」判词相应作废。\n\n### 9.2 点击逻辑表（全部源码行锚）"),
    # 7. §9.5 判词注明是修复前历史帧
    ("均不签像素对齐。",
     "均不签像素对齐。**注**：本节两端对账帧均为 §10 侧栏补齐前的历史帧，全图 NCC 不因侧栏补齐重算；左侧栏这一子项单独在 §10 验收。"),
    # 8. §9.6 判词里的「没实现/缺 3 块」标注修复前
    ("选角色页整栏本端没实现（只有主区木牌与 BtCharacter/BtPreview）、职业团页(UIRaceSelect)侧栏 3 块也缺",
     "选角色页整栏 / 职业团页侧栏 3 块（§9.1 修复前判词）已于 §10 补齐并真机逐块验收 `SIDEBAR_FIX_VERIFIED`（0.9313 / 0.9508）"),
]


def main():
    with open(P, "r", encoding="utf-8", newline="") as f:
        raw = f.read()
    crlf = "\r\n" in raw
    work = raw.replace("\r\n", "\n")
    for old, new in REPLACES:
        cnt = work.count(old)
        assert cnt == 1, "anchor count=%d for %r" % (cnt, old[:70])
        work = work.replace(old, new, 1)
    # 追加 §10 章节（去重保护）
    assert "## 10. 左侧木牌栏补齐" not in work, "section 10 already present"
    work = work.rstrip("\n") + "\n" + SECTION10.lstrip("\n")
    out = work.replace("\n", "\r\n") if crlf else work
    assert out != raw, "NO CHANGE"
    with open(P, "w", encoding="utf-8", newline="") as f:
        f.write(out)
    print("PATCHED", os.path.relpath(P, ROOT), len(REPLACES), "replaces + section10")
    print("lines", out.count("\n") + 1, "bytes", len(out.encode("utf-8")))


if __name__ == "__main__":
    main()
