# r001 · 原生客户端伤害公式与上限取证（只读取证）

> 工具链：REA MCP（provider=`ida`，attached 到 `E:/MXD/BeiDou-ClientV17.7/BeiDou.exe.i64`，imagebase `0x400000`，size `0xa94000`，sha256 `1198fa57…`）+ 直连 IDA MCP（13337）+ 运行时 `Effect.nx` / `UI.nx` 节点巡检。
> 证据文件：同目录 `../evidence/ida_decompile_*.txt|json`。
> 性质：静态反编译观察，不是运行时执行；函数体范围、间接调用、未命名数据均存在覆盖限制。

## 1. 关键结论

1. **「19万9」是原生客户端 `CalcDamage` 的单段硬上限**：`BeiDou.exe` 的 `0x00AFE8A0` 处 8 字节为 double `199999.0`（`00 00 00 00 F8 69 08 41` = `0x410869F800000000`）。
2. 该常量在物理/魔法两条伤害线存储前都参与比较，超过则**替换 ST(0) 为 199999.0** 再转整数；这是 v83 客户端的 line-store 封顶，不是金币或 INT_MAX 上限。
3. 原生伤害公式远比当前 C++ 客户端复杂：含 `PAD/MAD` 1999 上限、命中率随机、暴击、属性克制、技能倍率、怪物防御、Shadow Partner 等分支，且物理与魔法各有一套。当前端 `Mob::calculate_damage` 是简化公式，**未还原**。
4. 运行时资源里只有默认数字图集 `NoRed0/1`、`NoCri0/1`、`NoViolet0/1`、`NoBlue0/1`（`Effect.nx/BasicEff.img`），`UI.nx/UIWindow.img` 顶层**没有** `DamageSkin` 节点 → 伤害皮肤在本部署数据内无资源可接，按规范显式预留。
5. 参考端「长伤害显示」的完整 64 位封包链路处于休眠态（`E:/MXD/自定义功能实现/长伤害显示/最终实现方案.md` §5），因此本轮 long 化属**新增能力**，不是对齐原生行为。

## 2. 二进制证据（逐地址）

| 地址 | 字节 / 反编译观察 | 结论 |
|---|---|---|
| `0x00AFE8A0` | `00 00 00 00 F8 69 08 41` = double `199999.0` | 上限常量本体 |
| `0x0079058F` | `72 08` (`jb +8`)，未跳转时 `dd d8`(fstp) → `dd 05 A0 E8 AF 00`(`fld 199999.0`) → `e8 7A 1A 2D 00`(`call __ftol`) | 物理伤害行：>=199999 时以 199999 替换真实值 |
| `0x00791F94` | `72 08`，同样 `fld 199999.0` → `call __ftol` | 魔法伤害行同一封顶 |
| `0x00790599` | `e8 7A 1A 2D 00` + `8B 4D B8` + `89 01` + `EB 55` | 物理伤害行写回 `anDamage` 槽（`[ebp-0x48]`）；与 `IMPLEMENTATION.md` §5 的 `PDamage_store` 地址完全一致 |
| `0x00791F9E` | `e8 75 00 2D 00` + `8B 4D 40` + `89 01` + `83 45 40 04` | 魔法伤害行写回（`[ebp+0x40]`），随后槽指针 +4 |
| `0x0078E543` | `dd 05 A0 E8 AF 00` (`fld 199999.0`) … | 文档 §5 所述「另一处减法路径」，不是 line funnel，不改造 |

`xrefs_to(0xAFE8A0)` 共 13 处，全部落在伤害计算相关函数：
`sub_78DF87`（物理，size `0x2689`）的 `0x78E543 / 0x790586 / 0x790593`；
`sub_791617`（魔法，size `0x9a5`）的 `0x791F8B / 0x791F98`；
以及 `sub_790782`、`sub_792FA8`、`sub_79309F`、`sub_79345E` 各两处（配套小函数）。

## 3. 反编译片段（可复核）

物理 `sub_78DF87`（`evidence/ida_decompile_PDamage_78DF87_download.json`，行号按 code 文本）：

```c
// 最终封顶与写槽（约 L1580-1590）
if ( v243 <= 1.0 ) v243 = 1.0; else v243 = v328;
if ( v243 >= 199999.0 )
  v243 = 199999.0;
v74 = (__int64)v243;
*v320 = v74;
```

```c
// 特殊随机路径（约 L528）：195000~199999 固定伤害语义（SNIPE）
v59 = (__int64)(199999.0
      - ((v330 - Src) * (double)(v58 % 10000000) * 0.000000100000010000001
       + Src));
```

魔法 `sub_791617`（`evidence/ida_decompile_MDamage_791617.txt`）：

```c
// PAD/MAD 输入钳制
if ( v21 >= 1999 ) v114 = 1999;
v114 += v114 * v22 / 100;
if ( v114 >= 1999 ) v114 = 1999;

// 最终封顶与写槽
if ( v117 <= 1.0 ) v87 = 1.0; else v87 = v117;
if ( v87 >= 199999.0 ) v87 = 199999.0;
v47 = (__int64)v87;
```

中间还可见命中率 `v102 = 5*(dex/10+luk/10)*100/(def*10+255)`、伤害随机区间 `[0.7,1.3]`、暴击 `v120/v113`、属性克制 `sub_790660(..., 1.0 - elemental*0.01)`、`sub_78DEA6`（怪物防御）、Shadow Partner 相关 `9000..9002` 技能循环等分支。当前客户端 `Mob.cpp:725-824` 只有 `accuracy/((1.84+0.07Δ)*avoid+1)`、`wdef*0.6/0.5` 与 `×1.5` 暴击，缺项明显。

## 4. 资源取证（运行时数据为准）

```
python tools/nx_inspect.py list E:/MXD/BeiDou-Client-xy/Data/.nxcache/Effect.nx 2 BasicEff.img
  -> NoBlue0/1, NoCri0/1, NoRed0/1, NoViolet0/1  (均为容器节点)

python tools/nx_inspect.py list E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx 1 UIWindow.img
  -> 无 Damage/Skin 相关顶层节点
```

结论：默认数字图集可用，伤害皮肤资源在本部署包内缺失；`DamageSkin` 只能显式预留，不得凭空造节点。

## 5. 局限与口径

- REA `search_procedures` / `procedure_pseudo_code` 对大函数超时；最终用直连 IDA MCP `decompile` + `get_bytes` + `xrefs_to` 完成取证，属于同一 attached IDB（`BeiDou.exe.i64`）。
- 反编译是 Hex-Rays 重建文本，不等于源码；未解析的间接调用、结构体字段语义未知，不据此臆造字段名。
- 原生客户端的「199999 封顶」与 s9 服务端 `calcDmgMax`/`maxWithCrit` 是两套不同机制；本轮 long 化以服务端实际扣血为准，原生端只作为「上限来源」证据。
- 参考端 V17.7 的 ijl15 长伤害模块零符号、`AttachDamageLongMod()` 未接线；Kaentake 插件文档 `IMPLEMENTATION.md` 描述的是另一套注入 DLL + Cosmic 服务端，地址与本 exe 的 line funnel 一致但封顶常量为 199999（文档称 2.14b，属目标版本差异）。

## 6. 对后续步骤的硬约束

- 客户端唯一 clamp 点保留在 `Mob::next_damage`，默认口径 2^53；不得保留 999999。
- 12 字节/行协议会让 `r806` 的 4 字节/行 13/13 基线失效，必须做双模式对拍。
- 伤害皮肤无运行时资源 → 显式预留；默认数字图集须保留 `if(node)` 守卫。
