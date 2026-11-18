# 高版本装备白值 XML 缺失 — 1003242 tooltip 无 inc 属性根因与修复

> 日期：2026-11-18　触发：用户问"引入高版本装备/套装后，服务端是不是没同步 xml？客户端 tooltip 怎么没有 inc 属性（如 1003242）"

## 1. 结论速览

| 问题 | 结论 |
|---|---|
| 服务端没同步 xml？ | **部分正确**。String 名称 XML（`Eqp.img.xml`）和套装（`SetItemInfo.img.xml` + DB `xy_set_item`）都已同步；**缺的是 `Character.wz/Cap/01003242.img.xml` 装备白值 XML** |
| tooltip 为什么没 inc？ | 服务端缺白值 XML → `getEquipStats()` 返回 null → 发包 stats 全 0 → 客户端 tooltip 的 `equip.get_stat(es) > 0` 判断全部跳过 |
| 套装数据缺吗？ | **不缺**。运行库 `xy_set_item`（61 行）含 set 32"冒险岛宝石套装"，1003242 是其成员，complete_count=8 |
| 修复 | 已补 `wz/Character.wz/Cap/01003242.img.xml`（客户端 Character.nx 权威白值）；**XML 是 jar 外部资源，重启服务端即生效，无需重新打包** |

## 2. 证据链

### 2.1 各端数据有无对照
| 端 | 1003242 | 证据 |
|---|---|---|
| s9 String 名称 | ✅ | `wz/String.wz/Eqp.img.xml:1560` "冒险岛宝石贝雷帽"（`equip-tooltip-3fixes` 任务已补，wz 与 wz-zh-CN 双份） |
| s9 套装底稿 | ✅ | `wz/Etc.wz/SetItemInfo.img.xml:1281-1291` set 32，completeCount=8，ItemID/1=1003242 |
| s9 套装 DB | ✅ | MySQL 实测 `beidou.xy_set_item` 61 行，set_id=32 冒險島寶石套裝已入库 |
| **s9 装备白值 XML** | ❌ **缺失（根因）** | `wz/Character.wz/Cap/01003242.img.xml` 不存在；Cap 目录最高 id 01003073（无 010032*） |
| 客户端模型 img | ✅ | `Data/Character/Cap/01003242.img`（9.32KB） |
| 客户端白值（nx） | ✅ | `Data/.nxcache/Character.nx` 读到完整 info（见 §2.2） |

### 2.2 1003242 权威白值（客户端 Character.nx，nx_inspect 实测）
```
incSTR=3  incDEX=3  incINT=3  incLUK=3
incPDD=57 incMDD=57
reqLevel=45  setItemID=32  tuc=8  cash=0  tradeBlock=1
```
192 文本导出（`冒险岛代码文本版_192.1`）只有 ID/名称/描述、无 inc 字段，不可作为属性依据。

### 2.3 服务端加载链路（缺 XML → 发包全 0）
- `org/gms/provider/wz/WZFiles.java:14,26`：`CHARACTER("Character")`、`DIRECTORY="wz"`。
- `ItemInformationProvider.java:140-145`：构造时 `equipData = XMLWZFile(wz/Character.wz)`。
- `ItemInformationProvider.java:586-620` `getEquipStats()`：`item==null || info==null → return null`（:592-597）；否则遍历 `info/inc*` 存 map。
- `ItemInformationProvider.java:1242-1268` `getEquipById()`：`stats!=null` 才写 STR/DEX/INT/LUK/PAD/PDD/MAD/MDD 到 Equip → 发包。

### 2.4 客户端 tooltip 链路（0 值被跳过）
- `Net/Handlers/Helpers/ItemParser.cpp:80-84`：从包读 15 个 short 到 Equip stats。
- `Character/Inventory/Equip.cpp:69-72` `get_stat()` 返回包内 stats。
- `IO/Components/EquipTooltip.cpp:967-987`：遍历 STR→JUMP，仅 `get_stat(es) > 0` 才画行；`EquipStat.cpp:34-54` 只定义 15 字段。
- 次因：tooltip 只渲染固定 15 字段，`incAllStat`/百分比/星力/潜能即使有数据也不会作为 inc 行显示。

## 3. 修复（已做）

**补文件**：`E:\project\BeiDou-Server_s9\gms-server\wz\Character.wz\Cap\01003242.img.xml`
- 内容含完整 `info/inc*`（§2.2 权威值）+ icon canvas + islot/vslot/req*/tuc/setItemID。
- 已用 `xml.dom.minidom` 校验语法有效、inc 字段齐全。
- `getEquipStats()` 遍历所有 `inc` 前缀字段（:600），所以 STR/DEX/INT/LUK/PDD/MDD 都会被读入并发包。

**部署**：该 XML 在 jar 外部（`WZFiles.DIRECTORY="wz"`，jar 内 0 个 Character xml），**无需重新打包，重启服务端即生效**。

## 4. 历史背景（为什么会漏）
`docs/features/equip-tooltip-3fixes-2026-11-18/README.md`（问题3）当时明确了"服务端 XML 同步"需求，但只补了 **String 名称 XML**（解决"物品id不存在"报错），**未补 Character 白值 XML**。所以现在名称能显示、inc 属性不显示——两阶段同步不完整。

## 5. 后续（如需完整高版本装备支持）
1. 其他高版本装备（如 1000040/1003364，同批移植的）大概率也缺 Character 白值 XML → 用同样的 nx_inspect → XML 方式补齐，或批量从客户端 `.nxcache/Character.nx` 导出所有 `0100xxxx` 的 info 生成 XML。
2. 若要显示 `incAllStat`/百分比/星力/潜能，需扩展 `EquipStat` 字段集 + `ItemParser` 包布局 + `EquipTooltip` 渲染分支（不是补 XML 能解决的）。
3. 套装文本已齐（set 32 在 DB），hover 应能看到"冒险岛宝石套装"档位——如未显示再抓 0x176 确认。

## 6. 待用户真机验证
1. 重启服务端（加载新 XML）后 `!item 1003242` 或 GM 发装备，hover 看是否显示"力量+3/敏捷+3/智力+3/幸运+3/物理防御+57/魔法防御+57"。
2. 确认套装文本"冒险岛宝石套装"是否显示（set 32 已在 DB）。
3. 若 inc 仍不显示，抓包看服务端下发的 Equip stats 是否非 0（可能需清 `equipStatsCache`）。
