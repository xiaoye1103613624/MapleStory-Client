# R27-B4 真机复验：UserList 五格页签 / 装备窗落位 / 背包回归

部署件：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` md5 `a53de1c830864bad00934aa2a2d993e7`，4,452,352 B，2026-11-02 11:20。
上一件 `d780ce77…`（10:36）里**没有**本轮四条提交，所以这次复验才是它们的第一次真机取证。

构建：`MSBuild MapleStory.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64 -p:PlatformToolset=v142 -m`
→ `EXITCODE=0`，`: error` 0 条（`: warning` 2434 条，均为既有 C4100/C4456 一类）。
前置门禁 `python tools/gen_opcode_matrix.py --check` → `GATE OK: opcode matrix matches both sources (s9 44d0d5d9ca)`。
必须整解决方案 Rebuild 的原因：`UIUserList.h` 删掉了成员 `boss_sprites`，公共类成员布局变了（见项目记忆 msbuild-header-layout-stale-objs）。

链接曾被运行中的客户端锁住（`LNK1104 …MapleStory.exe`），按门禁 14 首次询问后由用户选定「关掉 PID 50704」；
本次实际执行时 50704 已自行退出，用 `[System.IO.File]::Open(...,'None')` 断言到 `UNLOCKED` 才开编 —— 没有盲杀。

## 一、驱动链与一个必须记下的坑

`_ms_probe/run_login.py`（口令只从 `xy-config.yaml` 读，只打印长度）→ 客户端 `login.auto` 直接把角色送进世界，
`open_window.py` / `click_img.py`（PrintWindow 整窗帧，坐标=窗口相对）→ `layout.py check`。

**默认按键表不是 `UIKeyConfig.h` 里的 `alternate_keys`。** 真机键盘设定窗（`live-a53de1c8-keyconfig-keyset.png`）读出来是：

| 键 | 动作 | 键 | 动作 |
| --- | --- | --- | --- |
| E | 装备窗 | R | 好友 |
| I | 道具 | K | 技能 |
| O | 寻找组队 | P | 组队 |
| `[` | 界面 | `]` | 快捷键 |

`alternate_keys` 只是键盘设定窗「备用方案」那一栏的预置，运行时映射由 `UI::add_keymapping`（服务端下发的 keyset）灌进
`Keyboard::assign`。所以 `live/open_window.py u` 打的是空键，`period` 同理 —— 按错键会抓出一张「什么都没开」的帧，
长得和「修坏了」一模一样。这类假阴性本轮已出现过一次（agree=0.053 那次是句柄写死），记在这里。

## 二、判据达成（`live-a53de1c8-check.txt` 原样输出）

### UserList（提交 `4a1a011`）—— 6/6 不合格 0

| 探针 | oracle | 实测 | Δ |
| --- | --- | --- | --- |
| 锚 Tab0「好友」 | (4,23) agree 1.000 | 同 | 0 |
| Tab1 组员 | (31,-1) 1.000 | (31,-1) 1.000 | 0 |
| Tab2 家族 | (62,-1) 0.987 | (62,-2) 0.987 | 1 |
| Tab3 家族联盟 | (92,0) 1.000 | (92,0) 1.000 | 0 |
| Tab4 拒绝列表 | (145,-1) 0.986 | (144,-2) 0.986 | 1 |
| BtClose | (284,-19) 1.000 | (284,-19) 1.000 | 0 |
| BtAddGroup | (64,54) 1.000 | (64,54) 1.000 | 0 |

肉眼同图可证的两条：五格页签**一行排开不再叠在窗角**；底图只有一张（改前是 backgrnd/backgrnd2/backgrnd3 三张同尺寸
98.8% 不透明层叠着画，后画的整张盖掉先画的）。Tab3/Tab4 的 Δ=1 是页签美术自身 1px 的锯齿差，agree 与 oracle 完全一致。

### Equip（提交 `1a72b41`）—— 5/7，2 条不合格**不是本轮改的东西**

过：`panel_border_bbox (1,1) 173x302 Δ=0`、`grid_top_edge y=32 rows=[32,33,34] Δ=0`、
`grid_bottom_edge y=265 rows=[265,267..271] Δ=0`、`pet_panel_bbox (181,124) 167x148 Δ=0`、
`pet_panel_tmpl (171,123) Δ=0`（agree 0.766 > 原生自测 0.631，模板本身在原生帧上也只有 0.631，这是宠物面板美术与
抖动底叠加的固有上限）。

不合格两条 = `pet_slot_pink_bbox (72,234) 31x31` 与 `pet_slot_wide_bbox (6,234) 64x31` 全 MISSING。
原生帧上这两格确实是实色粉：均值 `(250,181,183)` 与 `(205,152,165)`（`_native_equip_bottom` 裁片可复核）。
这两格是**程序画的底**，不在 `Equip/backgrnd` 里 —— 全仓 grep 没有任何给格子刷粉/灰的代码，说明它属于「装备槽可不可用状态」
那条待办（与并发线的 `081fd07` 灰显取证同源，色值也对得上：`081fd07` 记的是 ≈RGB(213,156,169)）。
**结论：装备窗的落位/命中/图标三条已真机定案；粉格与灰显是同一条未实现的着色线，不在 `1a72b41` 的范围内，不算改坏。**

抓帧注意：宠物面板必须先点「宠物装备」展开，否则 `pet_panel_*` 会去匹配旁边叠着的背包白格，报出 Δ=44 的假阳性
（本轮第一次跑就是这样）。

### Item 折叠态（回归）—— 4/4 不合格 0

`slot_rows [50,84,118,152,186,220] Δ=0`、`meso_digits (8,266) 167x14 Δ=0`、金币末位 `x+w −1` 仍是已定案的字体光栅差。
本轮没碰 `UIItemInventory`，此项只用于确认 `UIEquipInventory` 的改动没有外溢。

## 三、跑不了的

- **MonsterBook（提交 `a6ac01e`）**：`MONSTERCOLLECTION` 在本包默认 keyset 里**没有绑定**，`UIStateGame.cpp:427` 是唯一入口，
  只能靠 NPC 手册对话打开。真机自证到此断在取证面，改法本身是删掉两行多余的 `UIBook` emplace，逻辑面无分支。
  → 需要用户开一次手册确认「只弹一个窗」。
- 装备槽取槽（`slot_by_position` 重写）是命中逻辑，PrintWindow 帧量不出来，只能靠鼠标悬停/穿戴实测。
