# 字段类型白名单（禁止按 Angel 改回）

> 来源：用户拍板「除我改的字段类型」+ git 提交 `58fcf369`（level/stat int32）/ `547a7bd1`（meso long）+ 工作区 Job/Tooltip 注释。

| 项 | 必须保持 | 文件 |
|---|---|---|
| MapleStat 基值 | `int32_t`（非 `uint16_t`） | `Character/CharStats.h`、`Net/Login.h` |
| Buff 数值 | `int32_t`（非 `int16_t`） | `Character/Buff.h`、`ActiveBuffs.h` |
| WATK/MAGIC 上限 | `2147483647` | `Character/StatCaps.h` |
| 子弹 watk | `int32_t` | `Data/BulletData.h` |
| EXP | `int64_t` | CharStats / handlers |
| meso | `int64_t` + `read_long`/`write_long` | `UITrade`/`UIDelivery` + Duey/Storage/Trade |
| Job 名返回 | `std::string` 按值 | `Job.h`、`CharStats.h`（禁 `const std::string&` 悬空） |
| Tooltip 行 | `BodyLine`；图标底 `ColorBox`；fill α≈0.88 | `EquipTooltip`/`ItemTooltip`/`TextTooltip` |

实现期自检：任何 PR/改动若把上表收窄回 083 原生 short/int，一律拒绝。
