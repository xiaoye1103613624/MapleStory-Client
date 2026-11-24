# 16 项需求 · 全量现状盘点（2026-11-18，点到面）

## 盘点结论：7 已闭环 / 5 资源待导入 / 4 真缺失（其中 1 高风险）

| # | 需求 | 状态 | 铁证（逐行号/路径） |
|---|---|---|---|
| 1 | 商城窗口 | ✅ 已闭环 | MEMORY 商城九症状交付 + docs cashshop-* 多文档 |
| 2 | 聊天表情包 | ✅ 本轮实现 | 第22轮双端闭环，PLAN/README 在 chat-emoticon-native-port-2026-11/ |
| 3 | 掉落特效 | ✅ 已闭环（本轮确认） | `DropAuras.h:2-20` 头注明写素材来自本需求包 Effect.wz、grade 语义对齐；`Drop.cpp:35-47` 消费渲染；`MapDrops.cpp:45` init；服务端 `PacketCreator.java:1954/1985` 两处下发 grade |
| 4 | 伤害统计 | ✅ 已闭环 | 第16轮 UIDamageRank 实现 |
| 5 | 动漫皮肤 | ⏳ 资源待导入 | 96 img GMS 标准头，SOP 已验证 |
| 6 | 111款阿尔泰皮肤 | ⏳ 资源待导入 | 112 img + 双份资源 |
| 7 | 卡片名称 | ⚠️ 资源待导入+key 特例 | Ins.img 非标准头 `73f851bc…`，实为明文 XML |
| 8 | 每日签到 | ✅ 已闭环（本会话实现） | 双端原生移植见 docs/features/daily-checkin-native-port-2026-11/README.md，独立表 checkin_state+降级，opcode 0x3717/0x3718，28 格窗+@daily；VS三证+mvn=0；真机未验收 |
| 9 | 美容院 | ❌ 真缺失 | 客户端/服务端零痕迹 |
| 10 | 融合外观 | ❌ 真缺失 | 客户端/服务端零痕迹 |
| 11 | 伤害皮肤 | ✅ 已闭环 | 9-10 文件 + DAMAGE_SKIN(0xF7) + .dskin_bak |
| 12 | 适用于083时装 | ⏳ 资源待导入 | 15 PKG1 wz 容器，1.08GB 武器包须分片 |
| 13 | NameTag 修复 | ⏳ 资源待导入 | 单 img GMS 标准头 |
| 14 | 装备染色 ColorPrism | ✅ 双端实现闭环（2026-11-19） | 绘制时分流架构（TintCache/TintContext/draw_raw_ex/五档注册），服务端全7action+全图广播 mvn=0，VS三证；装备tintfx渲染档+真机验收为后续小项；详见 docs/features/colorprism-dye-native-port-2026-11/README.md |
| 15 | 装备悬停对比 | ✅ 已闭环 | EquipTooltip（第21轮判定） |
| 16 | 装备加技能 | ❌ 真缺失·高风险 | 原生 Equip.h 无 skill 字段、服务端 Equip.java 无 skill 字段；参考包是往 Detours 对象 +0x139 placement-new EquipSkill，原生无对应物；要做须改装备编码格式（牵连 critical memory「装备槽位不变量」） |

## 元信号
连续两轮（#15、#3）"以为缺失、实则已实现"——MEMORY 中"商城九症状/装备槽位扩展⑥"记录的历史移植已覆盖多个需求包。**任何功能动手前必须先双端 grep 验证现状**，禁止照包就改。

## 真缺失功能的硬前置
| 功能 | 前置 |
|---|---|
| #8 签到 | 资源入库(UI WZ) + opcode 重排 + 3 列 SQL(只落不执行) + 奖励表配置 |
| #9 美容院 | WZ 资源自备(包内无 wz) + DDL + opcode |
| #10 融合外观 | 服务端 Equip 加 anvilItemId 列 + 发包改装备编码 + opcode 0x4F 复用 |
| #14 染色 | 18 列 DDL + 客户端改色引擎(Detours→原生重写) + opcode |
| #16 装备加技能 | **改装备编码格式（高风险，critical memory 警告）** + 资料残缺(zip 无法列出) |

## 建议下一步
- 低风险优先：#8 签到（方案现成、UI 美术已有，只需逻辑+SQL 落盘）
- 资源类可并行：#5/6/7/12/13（需用户授权动 live 资源）
- 高风险后置：#16（需专门评审装备编码改动）
