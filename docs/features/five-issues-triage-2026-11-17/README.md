# 五现象全链路归因 + 修复（2026-11-17 22:20~23:06）

> 用户一口气报 5 个现象 + 切图掉落视频。按"以点到面、一次改完"要求，逐条 RCA + IDA/IMG 取证。
> 味道：🔴 华为 RCA。构建 `error C`=0，部署实时 SHA `4c136e9f...` 三处一致。

## 逐条结论表

| # | 现象 | 归因（行号级） | 处置 |
| --- | --- | --- | --- |
| ① | 0 血不能双击用药 | **服务端** `UseItemHandler.java:48-50` `if(!chr.isAlive()){enableActions();return;}` 直接拒绝所有 USE_ITEM；`UseDeathItemHandler.java:32-34` 只设 itemEffect 不扣血复活——s9 服务端**没实现死亡态复活药** | 客户端无法修（包对了服务端也拒）。**需服务端决策**，未擅改 |
| ② | 卷轴双击用药水音效 | `Audio.cpp:128-146 Sound::Sound(itemid)` 查不到卷轴 prefix `02040000` → 回退 `02000000`。orange-wz 实证 `Sound/Item.img` 有 `02000000` 无 `0204xxxx`——**资源缺失** | 资源层问题，未擅改（改 fallback 会误伤所有缺音效道具；需 IDA 确证原生卷轴音效再补资源） |
| ③ | NPC 双击没反应 | IDA `CUserLocal::HandleLButtonDblClk@0x94fbbf` 原生双击只处理气球/留言板/宠物/召唤兽/角色信息，**不处理 NPC**；本端 NPC 单击已实现 `MapNpcs.cpp:97 TalkToNPCPacket` | **原生即如此**，双击 NPC 无特殊行为是对的；单击对话已实现。未加自创双击（重蹈软键盘覆辙） |
| ④ | 切图时 NPC 掉落（视频） | 真 bug：`clear()` 只清活跃对象、**漏清 spawns 待生成队列**（`MapDrops.cpp:126`/`MapNpcs.cpp:63`/`MapMobs.cpp:121`）；且 `SetFieldHandlers.cpp:221-223` clear() 在 fadeout 注册时同步执行，到 `:155` load() 之间多个 `poll_events()` 让旧包再入队，load() 不清队列 → 淡入后旧包在新 foothold 实例化，NPC 旧 fh 找不到落脚点被 `Physics.cpp:106-109` GRAVFORCE 拉下 / drop 重播下落动画 | **已修**：三容器加 `clear_spawns()`（queue swap），clear() 内联；`SetFieldHandlers.cpp:155` load() 后、poll 前二次清队列，与入口 clear() 闭环 |
| ⑤ | 怪物移出平台边缘掉落 | control 怪有 `TURNATEDGES`+`FootholdTree.cpp:134-143 limitx` 边缘夹取闭环，正常不掉；非控制怪 `Mob.cpp:389-393` 纯 movepath 回放无本地物理（`MovePath.cpp:83-92` 无边界校验，忠实回放服务端坐标） | **正常不会掉落**；例外=服务端 movepath 带越界坐标、`hmobile=false` 边界窗口。无需改客户端 |

## ④ 修复细节（唯一实际代码改动）

- `Gameplay/MapleMap/MapDrops.{h,cpp}` / `MapNpcs.{h,cpp}` / `MapMobs.{h,cpp}`：新增 `clear_spawns()`（`std::queue<T>().swap(spawns)`），`clear()` 调用之。
- `Net/Handlers/SetFieldHandlers.cpp:160-167`：fadeout 回调内 `Stage::load()` 成功后、紧随的 `poll_events()` 之前，调三个容器的 `clear_spawns()`。封堵 clear()→load() 异步窗口期残留包。
- 工作区另有 4 个历史 M 改动（UIItemInventory 背包翻页 / UIStateGame 冷却栏+聊天层级 / Stage 宏 / Mob 死亡动画），属基线，本轮构建一并编入，未覆盖。

## 验证与边界（诚实声明）

- 构建 `BUILD_EXITCODE=0`，`error C`=0；obj `MapDrops.obj 22:59` / `SetFieldHandlers.obj 23:05` > cpp 22:58（入链判据）。
- 部署按 PID 精确杀进程后覆盖；实时 SHA 三处一致 `4c136e9f4750aa1c76b302efdea63e7518ef290a9d9d4635359afa63a654cbd6`。
- **真机未验收**：④ 切图是否还掉、①②③ 需用户按各自路径点验。编译通过 ≠ 功能验收。
- ① 为服务端功能缺失、② 为资源缺失、③ 原生无此功能——**均未擅自改**，如需处理请分别立项（服务端复活逻辑 / 卷轴音效资源 / 确认需求）。

## 视频取证

`evidence/video_frames/f_001..007.png`（ffmpeg 从 XiaoYe_2026-11-17_22-28-59.mp4 抽帧；f_003 切图前，f_004/f_005 切图后见底部掉落物/名字标签位移）。

---

# IDA 深挖复核（2026-11-17 23:25）——用户"不确定的先验证再处理"

> 用户要求 IDA 都开着、不确定的先验证。13337=Angel.i64（`\`Angel.exe`, hexrays_ready），逐条反编译验证。

## 复核结论（推翻/坐实上一轮推测）

| # | 上一轮结论 | IDA 深挖后 | 处置变化 |
| --- | --- | --- | --- |
| ① | 服务端 isAlive 拒绝、客户端改不了 | **坐实**：`SendStatChangeItemUseRequest@0xa092fb` 药水发 opcode **72(0x48 USE_ITEM)**，与本端 `UseItemPacket` 一致；函数内无 HP==0/isDead 分支。问题确在服务端 `UseItemHandler.java:48` | 不变，客户端无法修，需服务端立项 |
| ② | 卷轴音效=资源缺失（推测） | **深挖推翻/升级**：`CDraggableItem::OnDoubleClicked@0x4efd25`（763 行全文已存 evidence）按 prefix 分发 221/219/203/210/212/224/226/227/231/232/524，**无 204 普通 UseItem**；卷轴走 UIEnchantSkill 壳。本端 `UIItemInventory.cpp:819` 卷轴开壳后**漏 break**，继续落到 :853 `UseItemPacket` 误发 USE_ITEM → 服务端 ItemEff → `Sound::Sound(204xxxx)` 查不到 02040000 回退 02000000 药水音 | **新增实质修复**：:824 加 break，卷轴使用由壳内 `apply_scroll:909 ScrollEquipPacket::dispatch_bag_equip` 独占发包 |
| ③ | 原生双击不处理 NPC（半函数推测） | **坐实**：`HandleLButtonDblClk@0x94fbbf` 全文 7225 字符，**NPC 命中 0 次**，只处理 Balloon/MessageBox/Pet/Summoned/角色信息/Avatar；发包 opcode 123/162 均与 NPC 无关 | 不变，原生如此，非 bug |
| ④ | spawns 漏清（已修） | 代码证据充分，无需 IDA | 保持已修 |
| ⑤ | control 有保护/movepath 回放（正常不掉） | 代码证据充分，无需 IDA | 保持结论 |

## ② 修复与验证

- `IO/UITypes/UIItemInventory.cpp:824`：204 卷轴开壳后加 `break`，截断误发 `UseItemPacket`；确认壳内 `UIMiscShells.cpp:891-916 apply_scroll` 有 `ScrollEquipPacket::dispatch_bag_equip` 完整发包路径，加 break 安全。
- 证据：`evidence/angel_item_doubleclick_4efd25.txt`（Angel 道具双击 763 行全文）。
- 构建 `error C`=0，obj 23:23 > cpp 23:20；部署按 PID 精确杀进程，实时 SHA 三处一致 `2d5c24e523fa5ab8310c0a15de12984bccd5440e08d3fd9dfa19d88752f10057`。
- 真机未验收：卷轴双击应只开强化壳、不再播药水音——需用户点验。

## 累计两次部署

1. `4c136e9f...` = ④ spawns 队列清理修复（MapDrops/MapNpcs/MapMobs clear_spawns + SetFieldHandlers load 后二次清）
2. `2d5c24e5...` = ② 卷轴 break（含 ④ 全部，累进）
