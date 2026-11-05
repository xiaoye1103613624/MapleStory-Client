# 条件分支裁决：原生 classic「角色装备条件 / 头发颜色设置页」

日期：2026-11-05（第四轮复核重写）
裁决状态：**推翻第三轮「显式预留」结论；按原生逻辑实现 step4 外观 / 发色阶段**
证据优先级：REA / IDA 反编译 → 运行时部署 img 位图像素锚点 → 已实机验证的同页锚点 →
（用户截图仅最后观感参考，不作坐标来源）

## 1. 推翻理由

第三轮以「运行时 `charSet` / `avatarSel` / 各 `Bt*` 只有位图尺寸、无 `pos` 定位向量」
判定「无节点位置信息 ⇒ 显式预留」。本轮复核认定这是**方法错误**：

1. v83 经典建角 UI 的控件坐标本就硬编码在代码/脚本里，img 只提供位图；
   `pos` 向量不是位置信息的唯一形式。职业团页就是范例：IDA `sub_6157C9` 给出
   `CCtrlButton::Create(1003,530,422)`，而 `RaceSelect/normal/text/0` 位图内的白槽
   `(412,111)` 加说明板 blit `(118,311)` 恰等于 `(530,422)` —— 位图烘焙槽位就是位置证据。
2. 本轮导出 `charSet` 并逐像素量测，得到米黄内容区 `x≈8..211, y=100..273`
   （204x174）、标题带 `y≈45..90`、底部小板 `x≈22..200, y≈323..377`；
   9 行 `avatarSel` 200x17 × 18px 行距 = 162px，恰好落在内容区内。
   `BtLeft/BtRight` 15x16、`BtCheck` 50x24、`BtYes/BtNo` 81x41 与板内区域吻合。
3. `scroll/0,1` 帧**带 `delay`**（展开 250/50/50、收起 50/50/1000），
   `dice/0..3` 带各自 origin —— 上一轮「动画无时序、需自定」也不成立。
4. REA 深挖（`native-evidence.md` §5）证明 step4 复用既有窗口单例、不新建窗口类，
   主 exe 对该面板节点名 ASCII/UTF-16LE 均 0 命中，即**面板落点本来就不在 exe 立即数里**；
   「没找到第二个 OnCreate 立即数」是原生架构结论，不是停止取证的借口。

故第三轮「证据不足、交用户裁决」不成立，本轮直接实现。

## 2. 原生行为与状态顺序

| 问题 | 裁决 | 依据 |
| --- | --- | --- |
| 原生是否存在外观 / 发色阶段 | **存在**（step4） | `CLogin::ChangeStep` step4 分支初始化 `CAvatar::Init(...)` 与 9 类外观池；`sub_5FCF8A`；运行时 `charSet` / `avatarSel/0..8` / `dice` / `scroll` / `statTb` / `BtCheck` / `BtLeft` / `BtRight` 资产齐全 |
| 与命名态的先后 | **命名 → 外观/发色 → `CreateCharPacket`** | 用户缺陷 3/6 原文「输入名字后角色与名字消失及外观/发色设置页缺失」；4 张原生建角截图（`C:\Users\11036\Desktop\正确客户端截图`）均为命名页，说明命名页是进入该流程的首屏 |
| 命名确认后角色/名字是否保留 | **保留** | 同上；第三轮已用 `confirmed_name` + `newCharLook.draw()` 修复，本轮外观阶段继续保留 |
| 命名页三项是否移动 | **不动** | 用户裁决「骑士团页的角色占位、亭子、名字输入框正确，保持不动并作为对照基准」 |
| 面板屏幕绝对落点 | 无原生证据，取几何避让推导 `(323,150)` | 见 `native-step4-layout.md` §4/§5，明确标为低置信度决策点 |

**流程推断标注**：IDA 只钉到 step4 存在，没有钉死它与命名态的严格先后；
上述顺序来自缺陷描述与截图观感，属流程级推断，不是 exe 立即数结论。

## 3. 实现方案（本轮落地）

不新建 UI 类型、不动 modern 路径、不动协议与 `CreateCharPacket` 字段：

1. `init_classic()` 仍以命名态为首屏（`genderSelected=charLookSelected=true`），
   吊牌 / 确定 / 取消 / 输入槽 / 角色脚底锚点全部保持第三轮实机验证值。
2. 增设 `classic_look_stage` 状态位：`send_naming_result(false)`（名字查重通过）不再立即发包，
   改为进入外观阶段并保留 `confirmed_name`；把原发包逻辑抽到 `create_new_character()`，
   由外观阶段的 `BtYes` 触发。
3. 外观阶段绘制：`charSet` 面板 + 9 行 `avatarSel`（`disabled` 深棕底色表示该行
   当前无可切换项，与 `enable_char_look()` 的池大小判断一致）+ 每行左右切换箭头 +
   8 格发色色板 + 骰子 4 帧 + `statTb` + 已确认名字与角色。
   全部节点按 R6 加 `if (node)` / `is_valid()` / `try_maple_btn()` 兜底；
   `dice` 用手动帧播放（~80ms/帧、定格末帧），避开循环动画 `update()` 永不返回 true 的陷阱。
   **R5 取舍**：`scroll/0,1` 帧序列带 delay 已在节点层钉死，但其宽高 242x30..214 与
   `charSet` 225x377 不匹配，同锚点叠加会重影，本轮核验后不绘制（NO-FIX，非遗漏）。
4. 补齐 classic 侧此前未创建的 18 个外观切换按钮、`BtGenderMale/Female`、
   `BtHairSelect0..7`、`BtDice`；`enable_char_look()` / `disable_char_look()` 加判空，
   避免第三轮指出的空 `unique_ptr` 解引用。
5. `BtNo` 在外观阶段退回命名态（恢复输入焦点），命名态仍按原生回职业团页；
   `BtYes`/`BtNo` 位置由 `set_classic_yes_no(1/2)` 切换，补齐此前未赋值的
   `classic_yes_avatar` / `classic_no_avatar`。
6. 外观池继续用 `load_classic_char_look()` 的 9 类数据、`randomize_look()` 与
   既有 `button_pressed` 的 `BtSkinLeft..BtWeaponRight` / `BtHairSelect*` / `BtDice` 分支。

## 4. 方案取舍

| 方案 | 取舍 |
| --- | --- |
| 放开既有 GMS 三态「性别 → 外观 → 命名」 | 否决。首屏会变成运行时 img 无资产的性别面板，且 `getAvatarSelPos` 那套 `(489,97)` 会盖住北斗角色 `(627,430)`；违反用户「命名页三项保持不动」裁决 |
| 直接照抄 `getAvatarSelPos` / `getBtYesPos(1)` | 否决。它们是 v83 `CustomizeChar` 坐标，与北斗改版后的吊牌 `(721,155)` 不在同一布局；只复用 18px 行距与「确定/取消落在底部小板」的相对关系 |
| 命名后追加外观阶段（本轮采用） | 同时闭合缺陷 3 与 6，命名页首屏与三项锚点零改动；`CreateCharPacket` 字段与 `UIKeySelect` 时序不变 |

## 5. 影响面与回归界限

- **不改**：modern 路径；三职业命名页已验证的吊牌、确定/取消、输入槽、角色脚底；
  `UIWorldSelect` 木牌数量与位置；`LoginBackground::set_scene()` 的 origin 补偿语义；
  `CreateCharPacket` 字段与 `NameCharPacket` 时序；服务端；不提交 git。
- **本轮改动**：`IO/UITypes/UICommonCreation.{h,cpp}`、`IO/UITypes/UIRaceSelect.cpp`（仅在发现偏差时）、
  本目录证据与文档。

## 6. 仍缺落点 / 后续决策点

1. `charSet` 绝对落点 `(323,150)` 为低置信度几何推导；若取得原生 step4 截图或带 `pos`
   的资源包，应优先按那份证据重校（本次不因该缺口回退到「预留」）。
2. `BtGenderMale/Female`、`BtHairSelect0..7` 运行时无专属资产，用 `BtCheck` / `BtLeft`
   位图兜底；若补齐资源应换回原生资产。
3. `statTb` 落点、`charAlert` 显示时机无 xref；本轮 `charAlert` 不绘制。
4. 三职业 appearance 页叠图实机确认：受频道页 `world_selected` 门闩影响可能跑不到，
   跑不到时如实记为未跑项。
