# Round 15：⑦ 095 强化三件套补齐（火花 / 宝珠 / 星之力）

> 用户指令：「与当前没有问题就继续任务，只要保证编译没有问题即可」。
> 据此收敛范围：**不再纠缠 UI 自动化验证**，把 ⑦ 剩下三块按潜能同一套链路补完，双端编译通过。
> 结论：**⑦ 全部落地**（潜能 Round 8 + 三件套本轮），客户端 Rebuild EXIT=0、服务端 compile EXIT=0、7/7 门禁 PASS。

## 1. 为什么这三块之前是空的

Round 1 只读盘点结论：火花（`AdditionalOption`）/ 宝珠（`Soul`）/ 星之力（`StarForce`）
在客户端 `Character/IO/Net/Data` 全库与 s9 服务端**零功能命中** —— 无字段、无列、无包。
它们不是「补缺口」，是要新建一条完整链路。本轮照**潜能已验证的模式**逐层建。

## 2. 实现（双端 7 层，与潜能严格同构）

| 层 | 文件 | 内容 |
|---|---|---|
| 服务端字段 | `client/inventory/Equip.java` | 新增 `@Getter private short spark, soul;` + `@Getter private byte starForce;`（Lombok 自动出 getter，与既有 `potential1..bp2` 同处） |
| 服务端下发 | `util/PacketCreator.java` | 潜能 5×`writeShort` 之后追加 `writeShort(spark)` + `writeShort(soul)` + `writeByte(starForce)` = **5B** |
| 客户端字段 | `Character/Inventory/Equip.h/.cpp` | `spark` / `soul` / `starforce` 三成员 + `get_spark()` / `get_soul()` / `get_starforce()`；**构造参数带默认值 0** → 既有调用点零改动 |
| 客户端容器 | `Character/Inventory/Inventory.h/.cpp` | `add_equip` 末尾加 3 个默认参数并转发 |
| 包解析 | `Net/Handlers/Helpers/ItemParser.cpp` | 潜能 10B 门控之后再加 **`available() >= 5`** 门控读 5B；旧服务端只发潜能或什么都不发都不会错位 |
| Tooltip | `IO/Components/EquipTooltip.h/.cpp` | 三行 `has_spark_line` / `has_soul_line` / `has_star_line` + 对应 `Text`；`set_equip` 构建 + `height` 累加，`draw` 紧接潜能行绘制 |
| 属性注入 | `Character/Player.cpp` | 新增 `apply_enchant_bonuses(stats, inventory)`，与套装同一注入点（`close_totalstats()` 之前），6 个 `recalc_stats` 调用方自动继承 |
| 文案 | `locale/zh-CN.ini` | `equip.spark.1..10`（10 条）+ `equip.soul.1..5`（5 条） |

## 3. 数值口径（不臆造，明确标注）

| 项 | 处理 | 理由 |
|---|---|---|
| **星之力** | 每星 STR/DEX/INT/LUK +1、WATK/MAGIC +1，上限 25 星 | 官方是阶梯表，本端**无 `StarForce.img` 数据源**。采用线性保守模型，结构可原地换真实表 |
| **火花** | **只展示，不加数值** | 值是词条 id，无 `AdditionalOption.img` 表 → 不凭空造属性。与潜能同口径 |
| **宝珠** | **只展示，不加数值** | 同上，无 `SoulOption.img` 表 |

星之力的 tooltip 直接生成「星之力 N ****」（星数不是词条 id，无需 locale）。
火花/宝珠缺键时 `Locale::text_or` 回退成「火花 N」/「宝珠 N」，**绝不静默留空**。

## 4. 兼容性（双向）

```
新客户端 + 新服务端 → available()>=10 读潜能，>=5 读强化   ✅ 全功能
新客户端 + 旧服务端 → 潜能段够 10B 就读；强化段不足 5B 全 0  ✅ 不崩不错位
旧客户端 + 新服务端 → 尾部多 5B 被 while(available()) skip 吞掉  ✅ 向后兼容
```

## 5. 验证

| 项 | 结果 |
|---|---|
| 客户端**全新目录** Rebuild（`obj-r15`，不复用旧 obj） | **EXIT=0 / 0 error**，产物 `build-r15\MapleStory.exe`（5,028,864B / 22:27:39） |
| 服务端 `mvn -o compile` | **EXIT=0**，日志 `build_s9_r15.log` |
| 门禁套件 | **7/7 PASS** |
| 部署 | live SHA `8B30A494A43814C1` |

> 为什么强调「全新目录」：Round 13 实测复用 obj 的增量构建会因头文件改动产生 ODR 不一致，
> 崩在 `std::_Hash::_Find_last`（`0x64e1`）。`Equip.h` 本轮改了类布局，**必须全量重编**。

## 6. 门禁自身的修正

`verify_095_enchant.py` 第 1 项原本用「从 `int16_t p1 = 0` 截到 `inventory.add_equip`」
的正则窗口数 `read_short`。本轮在其间插入强化段（+2 个 `read_short`）后误报 `7 != 5`。
改为**只截取 `if (recv.available() >= 10) { ... }` 块本身**，边界精确。
同时新增第 4 项，覆盖三件套的 7 层（字段/getter/门控/tooltip 赋值+绘制/注入/服务端下发/词条）。

## 7. 用户 7 项最终状态

| # | 需求 | 状态 |
|---|---|---|
| ① | 右上 2 戒指 | ✅ RING3=15 / RING4=16，(104,65)/(137,65) |
| ② | 护肩 | ✅ SHOULDER=20，(71,230)，prefix 115 |
| ③ | 眼饰左侧第二项链 | ✅ PENDANT2=51，(38,98) |
| ④ | 扩展装备段 8 格 | ✅ UIAddonEquip + live 152px 底板 |
| ⑤ | 套装加成/伤害/属性/展示 | ✅ 四件齐 |
| ⑥ | 装备掉落特效 | ✅ 4 档双层 + 光柱 + 服务端 grade |
| ⑦ | 095 强化（潜能/火花/宝珠等） | ✅ **潜能 + 火花 + 宝珠 + 星之力全部落地** |

**唯一未做**：进图后的**视觉**确认（需人按 `U` 打开装备栏）。
Round 11/12 已实测证伪四条自动化通道（PrintWindow 静止图 / ImageGrab 自噪声 8-12% /
SDL 不吃窗口消息 / 文件探针），这是本客户端的硬边界，非未完成工作。
