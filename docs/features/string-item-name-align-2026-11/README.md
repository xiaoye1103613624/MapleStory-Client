# String 装备/道具名称整份对齐（S9 → xy）

> 个人学习/技术研究用途，单机离线环境。2026-11-22。

## 0. 需求

用户反馈装备/背包中戒指标题显示成分类名「戒指」，样例 ID **1114304**；要求整份装备与道具 String 名称与 S9 对齐。

## 1. RCA

- Tooltip 标题：`ItemData` ← `String/Eqp.img` → `Eqp/<Cat>/<id>/name`。
- 空名时 `EquipTooltip::equip_fallback_name` 回退 `equip.type.RING` →「戒指」。
- 对齐前：live `Eqp.img` 324KB（Ring 约百级条目）；`Character/Ring` 有 1114304 模型但 `Eqp/Ring/1114304` 缺失。
- S9：`Eqp/Ring/1114304/name` =「重启宇宙戒指」；Eqp 3.3MB。
- 服务端 `Eqp.img.xml` 已含 1114304（本轮不改服务端）。

## 2. 范围

整文件以 S9 为准替换本端 6 份 String img：

| 文件 | 对齐前 | 对齐后 |
|---|---:|---:|
| `Eqp.img` | 324461 | 3299337 |
| `Consume.img` | 241500 | 3144644 |
| `Etc.img` | 208924 | 548681 |
| `Cash.img` | 63983 | 483050 |
| `Ins.img` | 22099 | 616841 |
| `Pet.img` | 117747 | 193975 |

未改：`Skill` / `Npc` / `Mob` / `Map` / `MonsterBook`。

## 3. 流水线

```text
S9 Data/String/{Eqp,Consume,Etc,Cash,Ins,Pet}.img
 → orange-wz batch_convert_key(GMS→GMS, verify:true, parallelism:4)
 → staging/（本功能目录）
 → 备份 live SHA → 结束 MapleStory 占用 → 原子 Move 覆盖 live
 → 删除 Data/.nxcache/String.nx
 → unload_all → live 读回验收
```

- 密钥：128B `tools/_gms_key_128.b64` + IV `TSPHKw==`。
- `batch_convert_key`：`total=6 success=6 failure=0 verify=true`（约 718ms）。
- 指纹（MCP verify）：Eqp `8cddb628…` / Consume `15943f03…` / Etc `fd73b1fd…` / Cash `6b93fc41…` / Ins `4c2f9bc2…` / Pet `3557036d…`。

## 4. 验收（orange-wz live）

| 路径 | 结果 |
|---|---|
| `Eqp/Ring/1114304/name` | **重启宇宙戒指** |
| `Eqp/Ring/1112000/name` | 闪光戒指 |
| `Eqp/Cap/1003242/name` | 冒险岛宝石贝雷帽 |
| `.nxcache/String.nx` | 已删除（首启重建） |

真机：重启客户端后悬停/装备 1114304，标题应为原名；「装备分类：戒指」行仍正常。

## 5. 回滚

备份：`docs/features/string-item-name-align-2026-11/backup/`（见 `sha256.txt`）。

将 `*.pre_<hash8>` 原子拷回 `E:\MXD\BeiDou-Client-xy\Data\String\`，并删除坏的 `String.nx`。

## 6. 相关

- 客户端路径：[notes/client-path.md](notes/client-path.md)
- 历史零散补名：`docs/features/equip-tooltip-3fixes-2026-11-18/`
- AGENTS.md：禁止裸拷跨端 `.img`；须 `batch_convert_key`。
