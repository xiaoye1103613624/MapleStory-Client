# 条件分支裁决：原生 classic 是否有「角色装备条件 / 头发颜色设置页」

日期：2026-11-05
裁决状态：**原生确有该阶段；但运行时 img 未提供定位向量，本轮按证据红线显式预留，不落推导坐标**
证据优先级：REA / IDA 反编译 → 运行时部署 img 节点 →（用户截图仅最后观感参考，不作坐标来源）

## 1. 结论

| 问题 | 裁决 | 主要证据 |
| --- | --- | --- |
| 原生 classic 是否存在独立的外观 / 发色设置阶段？ | **存在**（step4） | `CLogin::ChangeStep` step4 分支；`sub_5FCF8A` 9 类外观池；运行时 `Login.img/NewChar*` 的 `charSet` / `avatarSel/0..8` / `dice` / `statTb` / `BtCheck` / `BtLeft` / `BtRight` 资产齐全 |
| 该阶段在当前客户端为何缺失？ | `init_classic()` 把 `genderSelected` / `charLookSelected` 直接置 `true`，跳过 gender/look 两态；`send_naming_result()` 名字可用后立即发 `CreateCharPacket`，没有外观确认环节 | `IO/UITypes/UICommonCreation.cpp` 的 `init_classic()` / `send_naming_result()` |
| 该阶段放在命名前还是命名后？ | **命名之后、`CreateCharPacket` 之前** | 用户缺陷原文「输入名字后角色与名字消失及外观/发色设置页缺失」；用户裁决「骑士团页的角色占位、亭子、名字输入框正确，保持不动并作为对照基准」→ 首屏必须仍是命名页 |
| 运行时 img 是否提供该面板的精确落点？ | **否** | `evidence/img-node-layout.txt` §2：`charSet` / `avatarSel` / `statTb` / 各 `Bt*` 只有位图尺寸与 `origin=(0,0)`，**无 `pos` / `ctrl` / `offset` 定位向量**；`CustomizeChar` / `RaceSelect_new` / `createLimit` 为 `<absent>`（R6 精简包预期） |
| 本轮是否实现该面板？ | **否，显式预留** | 用户红线「还原/校验必须先以原生界面逻辑与 img 节点位置信息为依据，截图不得作为坐标来源」；节点位置信息缺失时，任何精确坐标都只能是推导值。按 xiaoye 门禁应把证据不足的决策点交用户抉择，而不是把推导值伪装成原生常量 |

## 2. 原生行为证据（REA / IDA）

目标：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（PE x86, imagebase `0x400000`）
既有产物：`docs/features/login-flow-ui-ida-align/evidence/CLogin_ChangeStep_0x5f53c0.txt`
摘要见 `notes/native-evidence.md` §2 / §3。

- `CLogin::ChangeStep`（`0x5F53C0`）：
  - step3（`v13=0`，反编译 `:200-218`）创建 `CUINewCharRaceSelect` —— 职业团选择页。
  - step4（`v14=0`，反编译 `:148-198`）创建 `CAvatar` + `CAvatarLook::Init`，`pOrigin = (-2369-600*step, ...)`、`x=22`，并调用 `sub_5FCF8A(this)` 初始化外观池。
  - step5（`v14=1`）清标志。
  - `CUINewCharRaceSelect` 的 `BtSelect`(id 1003) 点击 → `CLogin::ChangeStep(login, -1)`，即 step3 → step4 是**连续的原生阶段**。
- `sub_5FCF8A`（`0x5FCF8A`, size `0x1b3`）：按性别从 `Male` / `Female` 两组各 **9 类**外观池取值，用 `rand()` 随机化；对应运行时 `avatarSel/0..8` 的 9 行与 `dice` 动画。
- 局限（见 `native-evidence.md` §4）：`search_strings` 只索引 ASCII，搜不到 `charSet` / `NewChar` 不表示原生没有；布局由 WZ/脚本驱动，exe 立即数里没有按钮落点。

**取证边界**：IDA 证据只能钉死「阶段存在 + 状态机顺序 + 外观池数量」，钉不出面板逐像素落点。

## 3. 运行时资源证据（R1，部署数据为准）

完整节点 dump 与命令输出见 `evidence/img-node-layout.txt`、`evidence/runtime-scene-nodes.txt`，本文件不复制命令输出。要点：

- `charSet` [bitmap] 225x377、`avatarSel/0..8` [bitmap] 200x17、`statTb` 65x77、`BtCheck` 50x24、`BtLeft/BtRight` 15x16、`BtYes/BtNo` 81x41；`charName` 201x224、`charAlert` 187x120、`dice/0..3` 与 `scroll/0,1` 卷轴动画均存在。
- 上述面板节点 **均无 `pos` / `ctrl` / `offset` 定位向量**，`origin=(0,0)`。
- `native_newchar_charname.png` 实测：输入槽深棕木纹，板内约 `(28,102)`、约 150x25；代码 `slot_pos(28,104)` / `newCharNameDim(150,24)` 与节点吻合 → 本轮据此只修基线/字色，不动锚点。
- `native_newchar_slot_zoom.png` 像素采样：中心 RGB ≈ `(165,69,0)`，全图均色 `(135,69,14)` → `Color::Name::BLACK` 在该槽上不可读，是「字色不对」的物理解释。

**关键结论**：`charSet` 及 9 行 `avatarSel` 的落点没有 img 节点位置信息。按 R1 与「禁止凭截图猜布局」，本轮不把这些推导值写进代码。

## 4. 方案取舍

计划中曾考虑三条路径，均未在本轮落地为「新建面板」：

1. **复用 modern 的 gender/look/name 三态首屏** —— 否决。用户裁决骑士团命名页三项保持不动作为基准；`draw()` 的 `!charLookSelected` 分支会访问 classic 下未初始化的 `sprites_lookboard` / `sprites_char_look` / `avatarOrigin`，`enable_char_look()` 会解引用 classic 下未创建的 18 个按钮。爆炸半径远超最小修正。
2. **命名后追加「设定角色」确认态并绘制 charSet 面板** —— 否决。面板落点无节点证据，只能靠位图尺寸 + 截图锚点推导，违反用户红线。
3. **抽出 `create_new_character()` 做二次确认** —— 否决。原生是否「命名后再点一次确定」没有独立证据（IDA 只钉到 step4 存在），改交互流程属无据变更。

**本轮采用**：只修复有确凿代码/资源证据的部分 —— `charNameSelected` 绘制分支补回已确认名字与角色、输入槽基线与字色；外观阶段在源码注释、本文件与 README §11 中**显式预留**，并把决策点交给用户。

预留所需的现成机制（后续若取得带 `pos` 的资源包可直接复用，无需新建架构）：`load_classic_char_look()` 的 9 类外观池、`randomize_look()`、`Buttons::BtSkinLeft..BtWeaponRight` / `BtHairSelect0..7` / `BtDice` 的 `button_pressed` 分支、`enable_char_look()` / `disable_char_look()`、`getAvatarSelPos()`。

## 5. 影响面与回归界限

- **不改**：modern 路径；三职业命名页已验证的吊牌、确定/取消、输入槽、角色脚底锚点（骑士团三项保持原值）；`UIWorldSelect` 木牌数量与位置；`LoginBackground::set_scene()` 的 origin 补偿语义；`CreateCharPacket` 字段与发包时序；服务端。
- **本轮改动**：`UIRaceSelect.cpp`（BtSelect origin）、`UICommonCreation.{h,cpp}`（名字字色/基线/命名态绘制）、`UIStateLogin.{h,cpp}`（VIEWALLCHAR 独占显示与恢复）。

## 6. 待用户裁决 / 后续项

1. **决策点**：是否允许按「位图尺寸 + 已实机验证的吊牌锚点」推导 `charSet` / 9 行 `avatarSel` / `statTb` / `BtCheck` / `BtLeft` / `BtRight` 的落点来实现 step4 面板（推导值、低置信度），或提供带 `pos` 定位向量的 `NewChar*` 资源包后再还原。
2. 若取得带 `pos` 的资源包，按节点向量重校后接入 `scroll/0,1` 卷轴展开动画（242x30 → 250x212）；节点无 `delay`，节奏需自定（参考 `BootSplash::play_logo()`）。
3. 在 IDA 中定位 step4 窗口类（同构特征：`sub_41C06C(&word_AF0E08,-1)` + `sub_403065(14xx)` + `sub_8ABxxx` 辅助，体量 ~14KB）以取得按钮创建立即数。
4. `charAlert`（187x120）与 `statTb`（65x77）的语义未在 IDA 中钉死，未绘制。
