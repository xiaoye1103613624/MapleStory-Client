# R01 · ijl15.dll 模块测绘（实证版）

> 日期：2026-10-21 · 目标：`E:\MXD\BeiDou-ClientV17.7\ijl15.dll`（2,023,936 B，PE32 / x86）
> SHA256：`a3bc8a81b019724d1b340d01d8908e6e7069419bd96fdbf58e57ce88967a8281`
> 方法：IDA Pro 9.4 headless 建库 → 全量符号/字符串/交叉引用导出 → 定点反编译
> 结论：**插件未做符号剥离（保留完整 MSVC 修饰名），纯逆向可行性高，风险显著低于预期。**

---

## 0. 关键发现（改变工期判断）

1. 插件里保留了**完整 C++ 修饰名**：`?AttachBeautyShopMod@@YAXXZ`、`ModRegistry::RegisterBuiltins`、`CUIDailyCheckin@DailyCheckin`、`SetItemMod::AppendTierStatLine` 等。不是 stripped 二进制。
2. 存在统一模块框架：`ModRegistry`（注册表） + `LazyCompatInit`（延迟挂载） + `CompatModule{name, onAttach, onTick}`（68 字节结构）。
3. 插件按 `config.ini` 驱动（`enableBuffTimer`、`enableEquipGrowthTip`、`enableNativeAdventurerDice` …），功能可单独开关，**便于逐项对照验证**。
4. 段表含 `.detourc` / `.detourd` → 使用 **Microsoft Detours** 做 hook，非手写 trampoline。
5. 字符串中直接暴露了部分 exe 侧 hook 地址，可直接用于 r02：
   - `[invexpand] hooked CUIItem::OnButtonClicked @ 0x0081D39F (btn 2007)`
   - `[quicklogin] AttachAllowCashTradeMod OK (NOP @4F3FB8/4F3FC4)`
   - `[bounce] DoActiveSkill cave @ 0x0096792A -> DoBoundJump for 1050/1054`
   - `#1 iteminfo @0x005CA71C` … `#9 MorphPack @0x0068487C`（GameData guard 校验点）

---

## 1. 统计

| 指标 | 值 |
|---|---|
| 函数 | 3,384 |
| 字符串 | 2,339 |
| 字符串交叉引用 | 17,801 |
| 代码段 | `.text` 0x10001000–0x1012F000 |
| 注册模块 | 25 |
| 注册表函数 | `ModRegistry::RegisterBuiltins` @ `0x10011410`（6,079 B，Hex-Rays 放弃，转反汇编解析） |

---

## 2. 模块注册表（实证，按注册顺序）

`RegisterBuiltins` 逐个构造 `CompatModule{ name, onAttach(std::function), onTick }`，`onAttach` 是一个唯一 hash 的 lambda。
下表 hash 为 IDA 解析出的 lambda 标识，`onAttach 实际调用` 取自该 lambda 反编译结果。

| # | 模块名 | onAttach 实际调用 | lambda | 备注 |
|---|---|---|---|---|
| 1 | HpMpAlert | `RegisterHpMpAlertPacketHandler()` | bffcebae | 仅注包处理器 |
| 2 | WindowTitle | `WindowTitle::RegisterPacketHandler()` | fd0d0117 | 参考清单外 |
| 3 | WorldMapInfo | `WorldMapInfo::AttachHooks()` | ed402aaa | 代码内默认 skip（bisect） |
| 4 | DamageRank | `DamageRank::RegisterPacketHandler()` + `AttachHooks()` | 1fe62047 | |
| 5 | DamageSkin | `DamageSkin::RegisterPacketHandler()` | 29bb4222 | 另有 `AttachDamageSkinMod` |
| 6 | BeautyShop | `BeautyShop::RegisterPacketHandler()` | d36298b2 | |
| 7 | DailyCheckin | `DailyCheckin::RegisterPacketHandler()` | 4c22db5d | |
| 8 | MiracleCube | `MiracleCube::RegisterPacketHandler()` | ec664ed5 | |
| 9 | ColoringPrism | `ColoringPrism::RegisterPacketHandler()` | 8b9f1650 | |
| 10 | StorageBag | `StorageBag::RegisterPacketHandler()` | 0019b9e4 | |
| 11 | FusionAnvil | （lambda 被内联进 RegisterBuiltins） | e29cd7eb | 见 `EnsureHooks@FusionAnvil` 0x100AF680 |
| 12 | HigherStorageList | `HigherStorageList::ApplyPatches()` | de58c9d3 | 纯 patch，无包 |
| 13 | PotentialScroll | `PotentialScroll::ApplyPatches()` | ad0f211d | 纯 patch |
| 14 | UserInfoDetail | `UserInfoDetail::RegisterPacketHandler()` + `AttachHooks()` | c81cdc80 | |
| 15 | DropItemAura | （内联） | ffb1df99 | 参考清单外 |
| 16 | SetItem | `SetItem::RegisterPacketHandler()` + `EnsureHooks()` + **`StatDetailExt::AttachHooks()`** | d7970fba | **属性详情随套装一起挂载** |
| 17 | EquipGrowth | `EquipGrowth::RegisterPacketHandler()` + `EnsureHooks()` | b423c992 | 受 `enableGrowthCompanionTip` 开关 |
| 18 | PartyBuffs | `PartyBuffs::RegisterPacketHandler()` + `EnsureHooks()` | 2b6d10b4 | |
| 19 | EquipCompare | `EquipCompare::EnsureHooks()` | 56ce2c40 | 纯 hook |
| 20 | SlotLock | `SlotLock::EnsureHooks()` | 3f73681f | 纯 hook |
| 21 | SideToolbar | `SideToolbar::RegisterPacketHandler()` + `OverheadIcons::AttachHooks()` | ab9800d5 | 头顶图标随侧边栏挂载 |
| 22 | CashShopWindow | `CashShopWindow::RegisterPacketHandler()` | 80692227 | |
| 23 | InvExpand | （内联） | 2332fbe9 | 见 `EnsureHooks@InvExpand` 0x10070600 |
| 24 | Weather | `Weather_RegisterPacketHandlers()` + `AttachWeatherMod()` + `AttachWeatherWindMod()` | 65abd33e | 双包 + 双 attach |
| 25 | NewCharDice | （内联） | f95d94c3 | 见 `ApplyPatches@NewCharDice` 0x1009AF60 |

---

## 3. 不走注册表的挂载入口（DllMain / 独立 attach）

这些功能不在 `RegisterBuiltins` 表内，靠 `DllMain` 或后续阶段直接调用，**移植时容易漏**：

| 功能 | 入口符号 | EA |
|---|---|---|
| 解锁 300 级 | `AttachLevel300Mod` / `AttachLevel300ModSafe` | 0x1009D410 / 0x100A5580 |
| 血量突破上限 | `AttachMaxHpMpMod` | 0x1009E280 |
| 金币超上限 | `AttachMesoUncapMod` | 0x1009EE00 |
| 商店列表加长 | `AttachHigherShopListMod` / `ApplyPatches@HigherShopList` | 0x1009CF80 / 0x1009CF60 |
| 玩家商店加长 | `AttachPersonalShopMod` | 0x100A5620 |
| 快速登录 | `AttachQuickLoginMod` | 0x100A5800 |
| 现金道具可交易 | `AttachAllowCashTradeMod` | 0x100A5740 |
| 角色槽位 | `AttachCharSlotsMod` | 0x100A58D0 |
| 弹跳技能 | `AttachBounceSkillMod` | 0x100D59C0 |
| 骑宠预览 | `ApplyPatches@UserInfoMount` | 0x1009AA30 |
| 新角色骰子 | `AttachNewCharDiceMod` | 0x1009B210 |
| 扩展装备栏 | `EnsureHooks@EquipAddon` / `AttachShoulderSlotsFix` | 0x100B5970 / 0x100A7FA0 |
| 美容院 | `AttachBeautyShopMod` | 0x10055040 |
| 收纳背包 | `AttachStorageBagMod` | 0x10076540 |
| 侧边栏 | `AttachSideToolbarMod` | 0x100625A0 |
| 伤害皮肤 | `AttachDamageSkinMod` / `AttachDamageSkinPickerMod` / `AttachUnitDamageRendering` | 0x10034010 / 0x100378D0 / 0x10034170 |
| 套装 | `AttachSetItemMod` / `AttachSetItemUiHooks` | 0x10041950 / 0x10041970 |
| 融合注能 | `AttachFusionAnvilPacketHooks` / `AttachFusionAnvilUiHooks` | 0x100B0510 / 0x100B0570 |
| 染色 | `AttachColoringPrismMod` / `AttachWeaponTintMod` / `AttachItemEffectMod` | 0x100BF240 / 0x100C84E0 / 0x100D4740 |
| 天气 | `AttachWeatherMod` / `AttachWeatherWindMod` / `Lamps_Attach` | 0x1007F0E0 / 0x1008A140 / 0x100968B0 |
| 世界地图 | `AttachMapInfoToolTip` | 0x10029860 |
| GameData 守卫 | `AttachGameDataGuard` | 0x1009FCB0 |
| 崩溃防护（非功能） | `AttachSkillTipCrashGuards` / `AttachEnterGameCanvasNullGuards` / `AttachLoadTrace` | 0x100A0000 / 0x100A04C0 / 0x100A0EB0 |

---

## 4. 与 33 项参考清单的对账

| 参考清单项 | 测绘结果 | 状态 |
|---|---|---|
| 套装系统 | `SetItemMod` + `SetItem::EnsureHooks` + `InitSetItemDatabase`（读 `SetItemInfo.img`） | ✅ 已定位 |
| 伤害皮肤 | `DamageSkin` + Picker + `LoadDamageSkin`（`DamageSkin.img` / `BasicEff/damageSkin`） | ✅ |
| 美容院 | `CBeautyShop` + `BeautyShop_HandleServerPacket` + `BeautyShop_SendUnlockSlot` | ✅ |
| 每日签到 | `CUIDailyCheckin@DailyCheckin`（`DrawLocalizedChrome`/`DrawTooltip`） | ✅ |
| 七彩棱镜染色 | `CUIColorPrism@ColorPrism` + `IsDyeableEquip` + `SetTarget` | ✅ |
| 收纳背包 | `StorageBag` + `CUIBagWindow@BagWindow` | ✅ |
| 窗口现金商城 | `CUICashShop@CashShopWnd` + `CashShopWnd_HandleSync` | ✅ |
| 世界地图 Tip | `WorldMapInfo::AttachHooks` + `AttachMapInfoToolTip` | ✅ |
| 他人装备详情 | `UserInfoDetail::AttachHooks` | ✅ |
| 装备成长 Tip | `EquipGrowth` + `EquipGrowth_OnEquipTipDrawn` | ✅ |
| 融合外观注能 | `CUIFusionAnvil` + `_FusionAnvil_GetTipStamp` | ✅ |
| 魔方结果窗 | `CUIMiracleCube` + `PopupDisabledByConfig` | ✅ |
| 侧边栏工具 | `SideToolbar` + `OverheadIcons` | ✅ |
| 格子锁定 | `SlotLock::EnsureHooks` | ✅ |
| 团队 Buff | `PartyBuffs` | ✅ |
| HP/MP 警报 | `RegisterHpMpAlertPacketHandler` | ✅ |
| 个人伤害统计 | `CUIDamageRank`（27 条 `UIWindow.img/DamageRank/*` 资源路径） | ✅ |
| 扩展装备栏 | `EquipAddon` + `InvExpand` + `ShoulderSlotsFix` + `Pendant2` | ✅ |
| 更高仓库列表 | `HigherStorageList::ApplyPatches` | ✅ |
| 商店列表加长 | `HigherShopList::ApplyPatches` | ✅ |
| 玩家商店加长 | `AttachPersonalShopMod` | ✅ |
| 金币超上限 | `MesoUncap` + `MesoDecode8_*`（Character/Fredrick/HiredMerchant/Storage/Trade 五处解码） | ✅ |
| 血量突破上限 | `AttachMaxHpMpMod` | ✅ |
| 300 级 | `AttachLevel300Mod` / `Safe` | ✅ |
| 角色槽位 | `AttachCharSlotsMod` | ✅ |
| 快速登录 | `AttachQuickLoginMod` | ✅ |
| 装备悬停对比 | `EquipCompare::EnsureHooks` | ✅ |
| Buff 计时 | `g_buffTimerAutoInit` + `enableBuffTimer` | ✅ |
| 潜能卷轴拖拽 | `PotentialScroll::ApplyPatches` | ✅ |
| 天气系统 | `WeatherFx::Inject` + 5 个 Frame 函数（Accum/Move/Puddle/Sway） | ✅ |
| 怪物图鉴 | 未在注册表/attach 名单命中 | ⚠️ 需专项搜（可能并入 GameData guard 或已移除） |
| 属性详情 | `StatDetailExt::AttachHooks`（随 SetItem 挂载） | ✅ **与当前端已实现项对应** |
| 长伤害显示 | 未单独命中（可能在 `AttachUnitDamageRendering` 内） | ⚠️ 需专项 |
| 抽奖系统 | 客户端无对应模块（纯服务端+管理端） | ✅ 与判断一致 |
| 骑宠预览 | `ApplyPatches@UserInfoMount` | ✅ |
| 新角色骰子 | `NewCharDice` + `enableNativeAdventurerDice` | ✅ |
| 弹跳技能 | `AttachBounceSkillMod`（仅 1050/1054） | ✅ |

**结论：35/37 项已定位到具体函数与地址；2 项（怪物图鉴、长伤害显示）需专项搜索。**

---

## 5. 复现方式

```powershell
# 1) 建立持久数据库（只需一次）
$env:REA_ANALYSIS_PROVIDER='ida'
$env:REA_IDA_MCP_CONFIG='C:\Users\11036\AppData\Local\Temp\opencode\ida-mcp-headless.json'
rea analyze 'E:\MXD\BeiDou-ClientV17.7\ijl15.dll' --provider ida --json
# REA 会报 cleanup_incomplete（adapter 已知竞态），但分析结果已在
# %TEMP%\rea-ida-*\ijl15.dll.id0，复制到固定目录后复用

# 2) 导出全量符号/字符串/交叉引用
ida.exe -A -S"notes\scripts\ida_ijl15_inventory.py" "ijl15.dll"

# 3) 定点反编译（把目标 EA 写进 dec_targets.txt，每行一个）
ida.exe -A -S"notes\scripts\ida_decompile.py" "ijl15.dll"

# 4) 生成模块表
powershell -File notes\scripts\gen_module_map.ps1
```

注意事项：
- 该安装**没有 `idat.exe`**，必须用 `ida.exe -A -S...`（autonomous），否则会弹 GUI 卡住。
- Hex-Rays 对 6KB 以上的模板密集函数（`RegisterBuiltins`）会返回 `None`，需回退反汇编。
- REA 的 `analyze` 会返回 `cleanup_incomplete`，属正常，结果有效。

---

## 6. 下一步

1. **r02 hook 表**：反编译各 `EnsureHooks` / `ApplyPatches`，提取 `DetourAttach` 的 exe 侧目标 VA、原始字节、patch 长度。
2. **r03 opcode 表**：反编译 25 个 `RegisterPacketHandler`，取 `RecvOpcode`/`SendOpcode` 比较常量与包体读取顺序。
3. **补齐 2 项缺口**：怪物图鉴、长伤害显示（按字符串 `MonsterBook`、`damage` 相关再搜一轮）。
4. **WZ 节点对账**：把插件引用的 `UI/...img/...` 全量与 `E:\MXD\BeiDou-Client-xy\Data\UI` 实际节点比对，产出缺失清单。
