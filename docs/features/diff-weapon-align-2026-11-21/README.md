# 对比差值位置 + 套装武器折叠 + 属性对齐 三问题修复（2026-11-21 14时）

> 个人学习/技术研究，单机离线。逐条行号级结论；真机未验收如实声明。

## 1. 逐条根因与修复

| # | 用户报的问题 | 根因（行号级 + IMG 证据） | 修复位置 |
| --- | --- | --- | --- |
| 1 | 装备对比差值显示到 tooltip 外面（屏幕左侧红框的 `(+8)(+90)` 等） | `EquipTooltip.cpp` 主窗/对比窗 diff 画在 `Point<int16_t>(right, pos.y())`，`right` 是 `BodyLine::draw` 返回的**相对 tip 的 x**（0..FRAME_WIDTH），被当成绝对屏幕坐标 → 画到屏幕左侧 | 两处改 `Point<int16_t>(pos.x()+right, pos.y())`（主窗 `:1760`、对比窗 `:2072`） |
| 2 | 套装列表出现多件武器（红框两行无部位标签的「冒险岛宝石套装(8/8)」） | orange-wz 实读 live `SetItemInfo.img/32`（冒险岛宝石套装）26 件里武器号段含 152/153；`equip_sort_key` 旧只折叠 130-149+170 → 152/153 落 `default:200+cat` 各自成行。且派生名 `set_name_base` 来自服务端文案首行带 `(8/8)`，未剥离 → 武器行显示「冒险岛宝石套装 (8/8)」 | 新增 `is_weapon_cat(cat)`（130-149 / 152-159 / 170），`equip_sort_key`+`part_label` 共用；`derived_member_name` 剥离尾部 `(n/m)` → 武器行显示「冒险岛宝石武器」 |
| 3 | 底部套装属性没对齐（STR/攻击力/MaxHP 数值忽左忽右） | 每个属性是单条 `Text`（标签+值连写），标签宽度不同 → 冒号/值起始 x 各异（实测 x319..338） | `Line` 加 `split_value/value`；属性拆标签(`ATTR_LABEL_W=60`)+值两段 Text，值从固定列起排 → 所有数值对齐一列 |

## 2. 构建与部署

- 脚本 `tools/_diff_weapon_align_build.bat`（Release x64 / v142 / 隔离 IntDir）。`MSBUILD_EXIT=0`，`error C`=0。
- 产物 5,175,296 B，SHA256 `A3E1E815AC07F23AB02ABF9101B314E019B78FBD747C685872A7A3CB524861D0`。
- 按铁律覆盖 live 正式 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`。部署时客户端 PID 51304 锁定 exe，沿用本会话已授权「结束进程再覆盖」。备份 `MapleStory.exe.bak_before_diffweapon_20261121`。
- **部署后 live 实测 SHA = A3E1E815…861D0**（与 build 一致，实时核对）。

## 3. 真机验收清单（待用户，截图须本端）

1. 悬停背包装备时对比差值 `(+N)` 出现在属性行右侧、tooltip 内，不再跑到屏幕左侧。
2. 冒险岛宝石套装：多把武器折叠为一行「冒险岛宝石武器」（X 标记）+ 部位(武器)。
3. 套装属性「标签 + 值」数值对齐成一列。

## 4. 如实声明

- 已部署 live 并核对 SHA，未做真机截图验收；因解锁结束客户端进程（本会话已授权）。
