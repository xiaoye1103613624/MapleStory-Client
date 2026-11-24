# 装备 tooltip 全词条补齐 + 083布局对齐 + 095布局还原 + 造数据验证（2026-11-20）

> 个人学习/技术研究，单机离线。参考端截图/BWIKI 只作布局与数值参考，**不作本端验收证据**。真机未验收如实声明。

## 0. 需求与决策（用户逐条）
1. 缺口都补齐；2. 083 词条上下间距对齐，长标签(金锤子/剪刀等)不居中冒号防溢出；3. 095 布局按095逻辑实现，需对接服务端一并改；4. desc 确认；5. 补品级副标题(SS级/传授)；6. admin 造数据验证 083+095 全词条。

用户抉择：q1=统一行距15px+块间6px；q2=标签≥6汉字自动左对齐；q3=用我给的网址+截图参考；q4=授权改服务端；造数据=改服务端+DB加列全造；潜能文案=ItemOption.xml+BWIKI交叉。

## 1. 逐条落地与行号证据

### 1.1 083 布局对齐（已构建）
- 长标签不居中：`EquipTooltip.cpp:105` build 内新增 `utf8_han_count()` 判定，标签≥6汉字(`LONG_LABEL_HAN=6`)时 `centered=false`（左对齐 BODY_LEFT_X，冒号紧随），短标签维持冒号中轴。覆盖金锤子(11字)/所有者/交易词条等，防溢出 264px 框宽。
- 统一间距常量：`EquipTooltip.cpp:242-244` 新增 `LINE_STEP=15 / BLOCK_GAP=6 / DESC_GAP=13`；主窗(`:1380+`)、对比窗(`:1606+`)所有正文行距、块间、描述前留白全部改用常量，消除原 15/6+15/13 散落魔数。构建段 height 补偿同步（pot_height/extra_height +6→BLOCK_GAP、+15→LINE_STEP，主窗与对比窗对称）。

### 1.2 095 布局还原（已构建）
- **品级副标题**：`EquipTooltip.h` 新增 `rank_line/rank_line_preview + has_rank_line*`；`EquipTooltip.cpp` 主窗/对比窗构建侧由 `equip.get_potrank()` 映射 `equip.rank.*`（POT_NONE 不显示，颜色跟随名称品质 namecolor），绘制侧插在名称区 potflag 之后、trade 之前（参考图顺序：名称→品级→潜能等级→交易→owner）。
- **潜在/附加潜能圆点标题**：`EquipTooltip.h` 新增 `pot_title/bonus_title(+_preview)`；ctor 从 `equip.potential.title`(绿●)/`equip.bonus.title`(紫●) 创建；绘制侧把 5 条潜能分两组——i=0..2 归「●潜在属性」、i=3..4 归「●附加潜能」，各带圆点标题行；构建侧 height 补偿同步加标题行。
- **强化三件套** spark/soul/star 布局沿用现有（每行 LINE_STEP，块间 BLOCK_GAP）。

### 1.3 缺口补齐（已写入）
- 潜能/火花/宝珠文案表：`locale/zh-CN.ini` + `en-US.ini` 各写入 80 条（potline 1-50 / spark 1-20 / soul 1-10），UTF-8 无 BOM strict 校验通过。
  - 数据源：现有 12/10/5 条体系扩展 + BWIKI `/maplestory/潜能` 公式表（属性%/全属性%/BOSS伤害/暴击/无视防御等）+ 095 `ItemOption.img.xml` id→属性类型 + 高版本参考截图词条样例。生成脚本 `tools/gen_equip_option_locale.py`，落盘 `tools/_eqploc.json`。
  - **注意 id 体系**：服务端魔方用 `r.nextInt(50)+1`（`CashOperationHandler:98-100`），DB 存 1..50；这是本端简化 id 空间，**与 095 原生 6 位 ItemOption id 不同**，文案按 1..50 生成。
- desc `#c…#` 宏：已确认生效——`Text.h:122-123` 默认 `formatted=true`，`GraphicsGL.cpp:996 case '#'` 解析 `#b/#c/#d/#g/#k/#n/#r`；`EquipTooltip.cpp:1005` desc 构建未关 formatted。**无需改代码**。
- 强化 5B 下发：s9 `PacketCreator:551-553` writeShort(spark)/writeShort(soul)/writeByte(starForce)，客户端 `ItemParser.cpp:150-155` 读取，**数据链路本就通**。

### 1.4 造数据（admin 验证样本，已写入 DB）
服务端改动（`BeiDou-Server_s9`）：
- `Equip.java:840+` 加 `setSpark/setSoul/setStarForce`（getter 由字段 @Getter 提供），`ItemFactory.java:131+` 从 `inventoryequipment` 读新列。
- DB `beidou.inventoryequipment` ALTER 加 3 列：`spark/soul/starForce`（INT NOT NULL DEFAULT 0）。
- Maven `mvn -o compile` EXIT=0 通过。

### 1.5 RCA：造数据加列后漏改 INSERT（2026-11-20 21:26 已修）
- **现象**：Admin 下线 `Error saving chr` → `Column count doesn't match value count at row 1` @ `ItemFactory.saveItemsCommon` equip `executeUpdate`。
- **根因**：表已 32 列（含 spark/soul/starForce），但两处隐式 `INSERT ... VALUES (DEFAULT, 28×?)` 仍只写 29 值；读侧已接新列、写侧未同步（同类历史：level-cap-999 / V1.11.7 潜能列）。
- **修复**：`ItemFactory` 改为显式列名 INSERT（31 `?`，含 spark/soul/starForce），`saveItemsCommon` + `saveItemsMerchant` 共用；Flyway `V1.11.15__add_inventoryequipment_spark_soul_starforce.sql` 幂等收口。
- **生效**：**必须重启服务端**加载新 `ItemFactory.class`（`target/classes` mtime 2026-11-20 21:26）。

造数据样本（角色 Admin id=1）：
- 背包样本 `inventoryitemid=26608`（itemid 1040002 帽子，pos5，owner="OWNER背包帽测试" 长owner验证防溢出）：15 属性全给值、upgradeslots=7、vicious=1、潜能 13/14/5、附加 9/10、spark=9、soul=1、starForce=12。
- 已装备样本 `inventoryitemid=26660`（itemid 1002357 帽子，槽-1，owner="Admin"）：另一套属性、潜能 1/2/3、附加 4/5、spark=10、soul=2、starForce=8。
- 悬停背包 26608 可同时看主窗 + 对比窗（对比数据源 = 已装备同槽 26660）。

## 2. 部署状态（2026-11-20 17:36 实时核对）
- 客户端产物：`tooltipfix_int/MapleStory.exe` SHA `b0d4903e64a6de6fcd6f7a50df8550d74a4cb44753a7c62e5dd6e6f1cd49bece`。
- 具名 exe：`E:\MXD\BeiDou-Client-xy\MapleStory_tooltip_095_20261120.exe`（SHA 一致）。
- live 主 exe：`afcdfac7ddce226da530bfa3c3c45760ffa268cb15965f7ab05e111cd3a2d252`（**不含本次改动**，strings 无 `equip.rank.*`），未覆盖（多客户端进程运行中）。
- live locale：`locale/zh-CN.ini` / `en-US.ini` 已同步（含 rank/标题/80条文案，5 类 key 命中），原文件备份 `.bak_tooltip_20261120`。
- 生成脚本/探针：`tools/gen_equip_option_locale.py`、`tools/apply_equip_option_locale.py`、`tools/_eqploc.json`。

## 3. 真机验证必需的环境操作（否则验不到）
1. **必须用新 exe 启动**：`MapleStory_tooltip_095_20261120.exe`（旧 exe 无本次代码改动；live 主 exe 未覆盖）。截图前先确认进程来自这个 exe。
2. **必须重启服务端**：ItemFactory/Equip 改动 + DB 新列需重启才生效；否则 spark/soul/star 仍读不到。**2026-11-20 21:26 存档 INSERT 修复后同样必须重启**，否则下线仍报 `Column count doesn't match`。
3. 客户端 locale 已同步，无需额外操作。

## 4. 验收清单（用户真机截图，须确认本端）
- 083 原生：15 属性行、需求块、职业行、攻速、可升级次数、金锤子、owner、desc —— 上下间距一致；金锤子/owner 等长标签冒号不居中、不溢出边框。
- 095 新增：名称上方星排、品级副标题（SS级…）、「●潜在属性」3 行、「●附加潜能」2 行、火花/灵魂武器/星之力行 —— 全部出现且文案正确（非"潜能N"占位）；主窗与对比窗对称。
- desc 里 `#c…#` 颜色宏正确上色、不露 `#c`。

## 5. 如实声明
- 本轮**未真机验收**，全部为源码层 + 构建 + DB 数据层证据。
- 参考端截图/BWIKI 只作目标样式与数值公式参考，**非本端验收凭据**。
- 095 客户端 VMP 壳不可静态反编译；布局按参考图 + s9 字段顺序还原，非 095 原生逐像素（子代理 probe-095-jar 破译 095.jar 土制壳确认同源，但 jar 内无布局常量/id文案表）。
- Hindsight 工具本轮 401 未配置 key，无法写入/查询。

## 6. 迁移一致性验证（2026-11-20 21:5x，用户要求"换库部署 SQL 脚本不能缺"）
- 问题：当前库 spark/soul/starForce 列由 dbx 手工 ALTER 加的（COMMENT 简版），需证明"换一个空库只跑 Flyway migration，逻辑照样正常"。
- 三步 migration 链（仓库已有，无需新增）：
  - `V1.0.17__create_inventory.sql` 建 inventoryequipment 24 基础列；
  - `V1.11.7__add_inventoryequipment_potential.sql` 加 5 潜能列（AFTER ringid）；
  - `V1.11.15__add_inventoryequipment_spark_soul_starforce.sql` 加 spark/soul/starForce 三列（information_schema 幂等守卫 + PREPARE/EXECUTE，AFTER bonusPotential2；已由服务端 21:32:14 Flyway 自动执行，flyway_schema_history 117/117 success）。
- 验证手法：建临时库 `beidou_migrate_verify` → 按序执行三个脚本原文 → information_schema 对比列序 → 用 ItemFactory 的 31 列显式 INSERT 插一行 → 读回验串列 → 删临时库。
- 结果：临时库 32 列列名/列序/默认值与当前库**完全一致**；31 列 INSERT 成功（1 row affected）、读回 int=63/watk=24/potential=13,14,5/bonus=9,10/spark=9/soul=1/star=12 **无串列**。
- ItemFactory 侧已对迁移免疫：`INSERT_INVENTORY_EQUIPMENT` 用**显式列名**（`:55-62`，31 占位符一一对应），SELECT 用 `rs.getInt("列名")`（`:140-148`），均不依赖物理列序。
- 唯一差异：三列 COMMENT 当前库是手工 ALTER 简版、migration 是标准版（"火花词条 id，0=无"），纯注释无功能影响。
- 结论：DDL 已进 Flyway 版本管理（V1.11.7 + V1.11.15），dbx 手工 ALTER 只是同步动作、功能不依赖它；dbx DML 仅用于验证数据，不写 migration。

## 7. 相关
- `refs-high-version-layout.md`（参考图布局 + BWIKI 潜能公式）、`itemoption_ids.json`（095 ItemOption 154 id 表）
- `docs/features/equip-tooltip-3issues-2026-11/tooltip-fields-gap-analysis.md`（原始三方对照）
- `README.md`（本批 RCA 前置调查）
