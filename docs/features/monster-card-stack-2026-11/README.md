# 怪物卡片可拾取多张 / 去"唯一"标记（C 方案）实现文档

> 日期：2026-11-17。仓库：客户端 `E:\project\MapleStory-Client`；服务端 `E:\project\BeiDou-Server_s9\gms-server`。
> 味道：🔴 华为味（RCA + 蓝军自攻击），全程可见 PUA 输出。交付留痕见 Mnemon 文档 `0887c747-9d23-417a-ad59-157af8ed045f`。
> 本版仅改资源节点，**未改服务端 Java 代码、未改数据库配置**。

## 1. 需求与结论

用户：怪物卡片"只能拾取一张"，问是否 img 节点设置；拍板选 C（让卡片可拾取多张 + 堆叠 + 去"唯一"标记），客户端部署目录 = `Data/Item/Consume/` 下 img。

**根因（RCA，三项独立层）**：
1. WZ **数据节点** `spec/consumeOnPickup=1` → 拾取即消耗入图鉴、不落背包（`ItemInformationProvider.java:1766` / `Character.java:2057-2062,2113,2193`）。**非图标节点 `icon`/`iconRaw`**。
2. `info/only=1` → 客户端 `Data/ItemData.cpp:77 unique = src["only"].get_bool()` → tooltip 显示"唯一"（用户截图的"唯一"即此）。
3. 真实堆叠上限由服务端 `ItemInformationProvider.getSlotMax:376-402` 决定（USE 无 slotMax 节点 → `item_slot_max` 配置，0 则回退 100），**不读 `only`**。

**关键取证（反转）**：DB `game_config` 中 `monster_card_auto_use=false`（update_time 2026-11-15，服务端在跑，配置层已生效 → 卡片本就进背包）、`item_slot_max=0`（→ slotMax=100）。即"能堆 100 + 进背包"在服务端早已放开，`only=1` 只剩 tooltip"唯一"观感层。因此 C 的实际动作是把两端 `only` 置 0，消除"唯一"标记并对齐数据结构；堆叠能力本就具备。

## 2. 改动清单

| 端 | 文件 | 改动 | 验证 | 备份 |
| --- | --- | --- | --- | --- |
| 客户端 live | `E:/MXD/BeiDou-Client-xy/Data/Item/Consume/0238.img` | 343 张卡片 `info/only` 1→0（orange-wz `mutate_nodes` set_value + save_as staging + 原子替换） | live 读回 343/343 only=0，distinct=[0] | `0238.img.bak_before_only0`（SHA `00f961fd…`） |
| 服务端 dump | `wz/Item.wz/Consume/0238.img.xml` | 343 处 `<int name="only" value="1"/>` → `value="0"`（文本替换） | grep 0 命中 value=1、343 命中 value=0 | `0238.img.xml.bak_before_only0` |

- live img SHA：改前 `00f961fd87b2378e122e3f5ff1d10c953cd0327e235fae54f9b589a7bee575cc` → 改后 `6c8b8ce2611960ac85f4a06e0e8048e75578dc636b831b1c6d108a8ac3439f14`（619498 B）。
- 服务端读哪份：`WZFiles.java:32-46` + `RuntimePaths.java:54-59`，语言 3 → `wz-zh-CN/Item.wz` 为空 → 回退 base `wz/Item.wz`（`wz-zh-CN/Item.wz/Consume/` 无 0238，已核）。客户端 `Data/` 与服务端 `wz/` 各自独立，故两端都改。

## 3. orange-wz 门禁执行记录

- 端点 `127.0.0.1:10012/mcp`，key = 128B `WzAESConstant.DEFAULT_KEY` base64（`tools/_gms_key_128.b64`），iv `TSPHKw==`（AGENTS.md 2026-11-09 规格）。
- 实测命令（只读/写入均经 MCP，未手改二进制）：
  - `load_files(paths=[0238.img], exclusive=false)`
  - `mutate_nodes(operations=[{rootPath, nodePath:"<id>/info/only", op:"set_value", value:"0"}]×343, continueOnError=false)` → `updated=343/343`
  - `save_as(rootPath=live, nodePath="", filePath=<staging .img>, unloadAfterSave=true, clearCache=true)` → staging 独立路径，**未回写 loaded 根**
  - `unload_all` → `load_files(staging)` 读回验证 → `unload_all` → `copy2(live, backup)` → `os.replace(staging, live)` → `load_files(live)` 读回验证 → `unload_all`
- 踩坑（已闭环）：首次 staging 用 `.staging_only0` 后缀，orange-wz `load_files` 仅认 `.wz/.img/.xml` → 报 GBK「不支持该目录或文件」（`ERR` 命中），脚本在原子替换前 assert 中断，**live 未被破坏**（SHA 仍基线）。改用 `.img` 后缀独立目录 `Data/Item/Consume/_card_stage/0238.img` 后一次通过。

## 4. 验证门禁

- [x] orange-wz 实测 live 卡片节点（spec/consumeOnPickup=1、only=1→0、monsterBook=1）
- [x] 客户端 live 343/343 `only=0`，distinct=[0]，SHA 更新
- [x] 服务端 dump 343 处 only=0，0 处残留 only=1
- [x] 两端各留改前备份，可回滚
- [ ] **真机验收（未做，交用户）**：重进游戏，确认 (a) 拾取卡片进背包且同格叠加（上限 100）；(b) tooltip 不再显示"唯一"；(c) 手动使用仍入图鉴。USER.md 要求：真机未验收须如实声明，不拿"资源改了"冒充"功能验证过"。

## 5. 生效条件 / 遗留

- 客户端：img 随启动加载，重进游戏即生效。
- 服务端：dump 改动需**重启服务端**才载入内存（当前进程已加载旧树）；但堆叠/进背包的实际判定来自 DB 配置（已生效），dump 的 `only` 服务端本就不用于堆叠，重启只为数据一致性，非功能前提。
- 若要回滚：`copy2(backup, live)` 覆盖客户端 img；服务端 dump 用 `.bak_before_only0` 还原。
- 临时脚本已删除；无临时调试日志残留。
