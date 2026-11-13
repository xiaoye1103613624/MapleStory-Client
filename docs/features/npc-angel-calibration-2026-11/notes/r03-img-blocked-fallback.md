# 第九面 NPC — IMG 门禁阻塞与降级链

> 取证日期 2026-11-09 · 与第七面商城 / 第八面技能同构，本面独立记录。
> 性质：门禁状态说明，不含实现修改。

## §0 门禁实测

`mcp_connector_status` 当前仅 1 条连接 `json-photoshop`（已连接 / Host 注册 128 工具），**orange-wz 未连接** ⇒ 本面 `REA→IDA→IMG` 三联门禁的 IMG 环节阻塞，与第七面 `cashshop-angel-calibration-2026-11/notes/r03-img-blocked-fallback.md`、第八面 `skill-angel-calibration-2026-11/notes/r03-img-blocked-fallback.md` 同一根因（MCP 连接器未注册，非脚本/端点故障）。

| 门禁 | 状态 | 说明 |
| --- | --- | --- |
| REA / IDA Angel 083 13339 | ✅ | 71/71 反编译、71 具名符号在盘 |
| REA / IDA BeiDou 13337 | ✅ | 70/71 EA + 1-EA 32B 字节审计 |
| **IMG orange-wz** | **⚠️ 阻塞** | 连接器未注册，无 `load_files` / `get_node_tree_json` 能力 |

## §1 替代链（只读取证，不改源码）

1. **`.nx` 资源 + `nx_inspect.py`**：`Npc/%07d.img`、`UIWindow.img/UtilDlgEx*`、`UIWindow.img/Skill`（skillPoint 面板）、`MapleTV.img`、`String/Npc.img`、`Quest/QuestInfo.img`、`Effect/MapEff.img/NPCSummon|NPCReturn`、`UI/Basic.img/DCMark` 节点存在性/尺寸/子节点用本地工具核查。
2. **正确客户端截图库** `C:\Users\11036\Desktop\正确客户端截图\` 作体感输入：`技能展开宏指令.png` 等既有 oracle；本面相关 `ui-v83-window-restore/` npctalk 截图几何、`beidou-v17-parity/notes/ui_layout_dump/skill_backgrnd*.png`。
3. **源码常量**：`IO/UITypes/UINpcTalk.h:35-347` 布局常量（`MAX_HEIGHT=248 / TEXT_X=166 / TEXT_WIDTH=310 / AVATAR_W=419 / AVATAR_H=306 / BTN_*_INSET` 等）、`Gameplay/MapleMap/Npc.cpp:35-38` `kNpcPathFlushMs=1000` / `kNpcWalkForce=0.08`、`Npc.h` MapleTV 双屏 240×90 / 240×180。
4. **083 IDB 一手 SP_ 字符串节点**（`angel-npc-facts.md`）：`SP_3942/3943 UI_MapleTV.img/TVON|OFF`、`SP_3955/3956 UI_MapleTV.img/TVCHAT1|2`、`SP_3190 UI_UIWindow.img/QuestIcon/D`、`SP_3191..3194 UI_UIWindow.img/UtilDlgEx/List0..3`、`SP_3196 Etc/ScriptInfo.img`、`SP_3232 Quest/QuestInfo.img`、`SP_4419/4420 Effect/MapEff.img/NPCSummon|NPCReturn`、`SP_1665 UI_Basic.img/DCMark`、`SP_1666 String/Npc.img`、`SP_955 NPC/%07d.img`。
5. **既有布局转储** `beidou-v17-parity/notes/ui_layout_dump/_skill_nodes.txt` / `_skillbands.{py,txt}`；`ui-v83-window-restore/` npctalk 参考帧。

## §2 F 样式维 12 项降级清单（全部 ⏳，P2 待 orange-wz 恢复）

| # | 校准点 | 083 一手 / 源码 | 本面可判定 | IMG 恢复后要补 |
| --- | --- | --- | --- | --- |
| F1 | MapleTV 上下屏贴图 | `SP_3942/3943 TVON/OFF` + `tv_basic/tv_chat` Texture | 机制 ✅ / 像素 ⏳ | `MapleTV.img` 双屏尺寸、台标帧 |
| F2 | TV chat 覆盖层 | `SP_3955/3956 TVCHAT1/2` | 节点名 ✅ | 覆盖层原点/透明度 |
| F3 | 5 行文字排版 | `Npc.cpp:151-198` A11M WHITE 行距 14 首行 y=-2 | 源码 ✅ | 字体 bitmap 与基线 |
| F4 | NPC 名牌 / func 标签 | `Text(A13B,CENTER,YELLOW,NAMETAG)` + offset(0,-4)/(0,18) | 源码 ✅ | 名牌子节点、描边 |
| F5 | UINpcTalk 主体底图 | `MAX_HEIGHT=248 / TEXT_X=166 / TEXT_WIDTH=310` | 常量 ✅ | `UtilDlgEx` 九宫格切片 |
| F6 | BtNext / BtClose 按钮 | `UtilDlgEx["BtNext"]` + `BtClose2` 描边 X | 源码 ✅ | 按钮三态贴图 |
| F7 | 输入框 / Simple 命中框 | `#L<n>#label#l` `SIMPLE_DASH #919191` | 源码 ✅ | 命中框色值、UL ext=8 |
| F8 | AskAvatar 预览窗 | `AVATAR_W=419 / AVATAR_H=306 / AVATAR_LOOK_X=207 / AVATAR_LOOK_Y=245` | 常量 ✅ | `UtilDlgEx_Avatar` 资源（本包疑缺） |
| F9 | 任务图标 | `SP_3190 QUESTICON_D` / `SP_3232 QUEST_QUESTINFOIMG` | 节点名 ✅ | 图标帧、DCMark 叠加 |
| F10 | NPC 召唤/回归特效 | `SP_4419/4420 NPCSUMMON/NPCRETURN` | 节点名 ✅ | 特效帧序列/时长 |
| F11 | skillPoint 升级确认面板 | `window4("Skill")["skillPoint"]` | 源码 ✅ | 本包 WZ 实测 NOT FOUND（见下） |
| F12 | 组合栏 / 宏按钮 | `SP_4346_..._BTMACROOUT`（第八面） + 本面 Shop/Talk 按钮族 | 交叉 ✅ | 按钮贴图对齐 |

## §3 已实测的资源缺失（沿用第八面结论，本面继续生效）

`IO/UITypes/UISkillBook.cpp:1098-1152 send_spup` 注释：`UIWindow.img/Skill/skillPoint` 在本包 WZ 中经 `nx_inspect` 实测 **NOT FOUND**，`BT_OKAY` 等钮从未创建，`set_skillpoint(true)` 恒 false，点 + 无反应（用户报「升级技能按键没反应」），实现已降级为「资源缺失时直接 `SpendSpPacket(id).dispatch()` 保证技能仍能升级」。`Net/Handlers/NpcInteractionHandlers.cpp` 同样有「缺 `UtilDlgEx`/`UtilDlgEx_Avatar` 资源时 `end_talk`，不要 emplace 隐形窗口，否则脚本一直等回复」的降级。这些都是增量兼容逻辑，**校准不推翻**。

## §4 P2 恢复条件

1. `mcp_connector_status` 出现 orange-wz 已连接 / Host 注册 `load_files` 等工具；
2. 用 orange-wz 加载 `Npc.img` / `MapleTV.img` / `UIWindow.img` / `Effect/MapEff.img` 对应节点，逐项回填 F1–F12 像素证据；
3. 与 `C:\Users\11036\Desktop\正确客户端截图\` 参考帧做 `vision_pixel_diff`；
4. 跑 `tools/build_smoke.bat` 出包验证贴图不炸窗；
5. 更新本面 README §3 F 维 12 项 ⏳ → ✅/⚠️，重跑门禁。

## §5 禁引 095（红线）

- `docs/features/port-095/**`（含 `evidence/opcode-v95/dec/_OnNpcPacket_CNpcPool__*.c`）禁引为 083 一手。
- `docs/features/change-quickslot-persist/notes/npc-action-move-pending.md` 引用的 `Maplestory95.i64`（`CNpc::GenerateMovePath 0x671590` / `CMovePath::Encode 0x666e20` / `Flush 0x668160` / `v95 send op 241`）**只作历史实现注记**；本面 083 权威是 `CNpc::GenerateMovePath @ 0x6d3273` + `COutPacket(197)`（见 `r01 §3`）。
- `CubeRerollPacket.h:182` 类 095 注释污染同样不作功能依据。

恢复前 F 维不伪造通过；IMG 阻塞不影响 A–E / G 维 86 条的只读取证判定。
