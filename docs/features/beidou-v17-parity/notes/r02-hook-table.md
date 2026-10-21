# r02 钩子 / 补丁表（参考端 → 当前端）

> 对象：`E:\MXD\BeiDou-ClientV17.7\ijl15.dll`（2,023,936 B，PE32，image base `0x10000000`）与同目录 `BeiDou.exe`
> 方法：IDA Pro 9.4 静态反编译 + 定点反汇编（脚本见 `notes/scripts/`）
> 用途：把参考端「运行时 patch exe 调用点」的做法翻译成当前端的「直接改源码」，并为每项功能留下可复核的 EA 证据
> 约定：本文 `0x100xxxxx` 为 dll 内绝对 VA；`0x4xxxxx` / `0x7xxxxx` 等为 `BeiDou.exe` 内 RVA（exe image base `0x400000`）

## 0. 通用结论：参考端如何注入

- 所有功能由 `ijl15.dll` 在启动早期挂到 exe 上，核心手段是**改写 exe 里 `call rel32` 的目标**：
  1. 校验站点首字节是否为 `0xE8`（`call rel32`），不是则跳过并记 `SKIP` 日志；
  2. 校验 `rel32` 解析出的原目标是否等于预期（防止版本漂移后误改）；
  3. `Memory::WriteByte(site, 0xE8)` + `Memory::WriteInt(site+1, newTarget - site - 5)`；
  4. 回读校验，成功记 `OK   %s @ 0x%08X: 0x%08X -> 0x%08X`。
- 写内存前按 `Memory::UseVirtuProtect` 决定是否 `VirtualProtect`，对应日志 `Attach* begin UseVirtuProtect=...`。
- **对当前端的含义**：不需要任何运行期 patch。参考端的每个「站点」在当前端都是一处源码调用点（封包读写 / 文本格式化），直接替换实现即可；站点表的价值是**穷举改动面**，避免漏改。

---

## 1. BuffTimer（buff 图标剩余时间）

### 1.1 符号与职责

| EA | 符号 | 大小 | 职责 |
| --- | --- | --- | --- |
| `0x100ADA50` | `anonymous_namespace_::FormatBuffTime` | — | 毫秒 → `1h2m` / `3m4s` / `5s` |
| `0x100AD3D0` | `anonymous_namespace_::DrawBuffTimeLabelOnCanvas` | — | 在 32×32 画布上画黄色正文 + 黑色描边 |
| `0x100AD870` | `anonymous_namespace_::EnsureBuffTimerFonts` | — | 惰性创建计时字体（Tahoma 10px） |
| `0x100AEF00` | `anonymous_namespace_::RebuildBuffShadowCanvas` | — | 把图标 + 文本合成到影子画布 |
| `0x100AF340` | `anonymous_namespace_::UpdateBuffShadowIfNeeded` | 683 | 按 `pieIndex` / 文本变化决定是否重建，并维护 `g_buffShadowStates` |

### 1.2 行为证据

- 入口条件（`UpdateBuffShadowIfNeeded`）：
  ```c
  if (stat != nullptr && stat->tLeft > 0) {
      nID = stat->nID;
      if (nID != 5221006 && nID != 35001002) {   // 两个 buff 跳过倒计时
          FormatBuffTime(...);
          pieIndex = clamp(stat->tLeft / stat->tLeftUnit, 0, 15);  // tLeftUnit==0 → 0
          ...  // 查 g_buffShadowStates，文本或 pieIndex 变化才重建
      }
  }
  ```
- 秒数：`(ms + 999) / 1000`，再保证 ≥ 1（反汇编为 `add eax, 3E7h` + 取整 + `cmovg`）。
- 分档：`>= 3600 → "%dh%dm"`、`>= 60 → "%dm%ds"`、否则 `"%ds"`。
- 绘制：文本色 `0xFFFFE060`，描边 `0xFF000000`、1px、8 方向；画布 32×32，`x = (32 - textWidth) / 2`，`y = 21`。
- 缓存：`BuffShadowState { TemporaryStat* stat; int lastPieIndex; std::wstring lastLabel; }`，按下标与 `stat` 指针线性查找。

### 1.3 当前端落地

- 源码：`IO/UITypes/UIBuffList.h/.cpp`（`BuffIcon::format_time` / `refresh_label` / `draw`），开关 `ui.buff_timer`（`Configuration.h/.cpp`）。
- 差异：不做影子画布缓存（直接每帧画 9 次文本）；暂不复刻 `5221006` / `35001002` 白名单；字体用 `Text::Font::A11M` 近似，`YELLOW` 与 `0xFFFFE060` 有轻微色差。
- 详见 `docs/features/buff-timer/README.md`。

---

## 2. MesoUncap（金币超上限）

### 2.1 总览

- 挂载函数：`?AttachMesoUncapMod@@YAXXZ` @ `0x1009EE00`，大小 1540 B，一次性执行（`g_attached_2` 守卫）。
- 补丁项数量：循环上界 `[ebp+var_210] = 0x20`，即 **32 项**；表项结构 16 B：`{ uint32 addr; uint32 expectedTarget; void* newTarget; const char* label; }`。
- 站点分布：**解码 7 项**（替换 exe 的 int32 读取）、**格式化 22 项**（替换数字/文本格式化）、**编码 3 项**（替换 int32 写入）。
- 站点原目标只有 4 种：
  | expectedTarget | 含义 |
  | --- | --- |
  | `0x406629` | exe 内 `Decode4`：从封包读 1 个 int32 |
  | `0x4065A6` | exe 内 `Encode4`：向封包写 1 个 int32 |
  | `0x428226` | 数字 → 文本（整数格式化，用于拼接 UI 文本） |
  | `0x988690` | 另一处数字格式化入口（用于 `FormatNumber_*` 类面板） |

### 2.2 8 字节编解码语义（最关键）

**解码**（`MesoDecode8_*`，80–111 B）：

```c
lo = Decode4();            // 先读低 32 位
hi = Decode4();            // 再读高 32 位
u64 v = ((u64)hi << 32) | lo;
g_lXxx = v;                // 64 位真实值，仅用于显示
return (hi != 0 || lo > 0x7FFFFFFF) ? 0x7FFFFFFF : lo;   // 玩法值仍钳到 INT_MAX
```

- 参考端**没有**把 64 位值当玩法数值使用：内部 `int` 依旧封顶 `0x7FFFFFFF`，64 位只服务于 UI 显示。
- 该结论与「插件放开金币上限」的直觉相反——**上限没放开，只是显示放开了**。当前端若要让玩法真的超过 21 亿，需要服务端与 `int64` 存储同时改造，属于 P3 范围，不与本项混做。

**编码**（`MesoEncode8_*`，101–106 B）：

```c
if (value == 哨兵) value = 真实 64 位值（可能取负）;   // 见下表
Encode4((int)(value & 0xFFFFFFFF));   // 先写低 32 位
Encode4((int)(value >> 32));          // 再写高 32 位
```

| 函数 | EA | 哨兵 | 替换为 |
| --- | --- | --- | --- |
| `MesoEncode8_StorageWithdraw` | `0x1009F7C0` | `value == 0x7FFFFFFF`（全部取出） | `g_lStorageMeso` |
| `MesoEncode8_StorageDeposit` | `0x1009F750` | `value == 0x80000001`（全部存入） | `-g_lMeso`（代码为取负 + 进位） |
| `MesoEncode8_TradeSetMeso` | `0x1009F830` | `value == 0x7FFFFFFF` | `g_lMeso` |

> 三个哨兵都只在「真实 64 位值 > INT_MAX」时才替换，否则沿用原值。

### 2.3 32 个补丁点全表

> `站点`为 `BeiDou.exe` RVA（`0x400000` 基址下的绝对地址去掉基址），`原目标`为期望的 exe 函数 RVA。

| # | 站点 | 原目标 | 替换为（dll 符号） | 语义 / label |
| --- | --- | --- | --- | --- |
| 01 | `0x4E2CFC` | `0x406629` | `MesoDecode8_Character` | `Decode_CharacterData_Money`：角色数据包金币 |
| 02 | `0x4E31F6` | `0x406629` | `MesoDecode8_Character` | `Decode_StatChanged_MESO`：`StatChanged` 的 MESO 字段 |
| 03 | `0x799DE1` | `0x406629` | `MesoDecode8_Fredrick` | `Decode_Fredrick_Meso`：弗雷德里克取回金额 |
| 04 | `0x79B058` | `0x988690` | `MesoFormatNumber_Fredrick` | `FormatNumber_Fredrick` |
| 05 | `0x79B092` | `0x428226` | `MesoFormat_Fredrick` | `Format_Fredrick_Meso` |
| 06 | `0x7C5E40` | `0x406629` | `MesoDecode8_Storage` | `Decode_Storage_OpenMeso`：打开仓库时的金币 |
| 07 | `0x7C20A2` | `0x406629` | `MesoDecode8_Trade` | `Decode_Trade_SetMeso`：交易窗设置金币 |
| 08 | `0x518859` | `0x406629` | `MesoDecode8_HiredMerchant` | `Decode_HM_VisitorMeso`：雇佣商人访客金币 |
| 09 | `0x518FBC` | `0x406629` | `MesoDecode8_HiredMerchant` | `Decode_HM_OwnerMeso`：雇佣商人主人金币 |
| 10 | `0x81DCA4` | `0x428226` | `MesoFormat_Character` | `Format_CUIItem_Meso_1` |
| 11 | `0x81DD4E` | `0x428226` | `MesoFormat_Character` | `Format_CUIItem_Meso_2` |
| 12 | `0x7C77A5` | `0x7C8064` | `MesoWrapFmt_StorageMeso_Naked` | `Wrap_StorageUI_StorageMeso`：裸跳板，转 `FormatNumber_StorageUI` |
| 13 | `0x7C77C8` | `0x7C8064` | `MesoWrapFmt_CharOnStorage_Naked` | `Wrap_StorageUI_CharMeso`：裸跳板 |
| 14 | `0x7C8088` | `0x988690` | `MesoFormatNumber_Active` | `FormatNumber_StorageUI` |
| 15 | `0x7C80C2` | `0x428226` | `MesoFormat_Active` | `Format_StorageUI_Meso_1` |
| 16 | `0x7C8186` | `0x428226` | `MesoFormat_Active` | `Format_StorageUI_Meso_2` |
| 17 | `0x7C2E42` | `0x428226` | `MesoFormat_Trade0` | `Format_Trade_Meso0_1` |
| 18 | `0x7C2F00` | `0x428226` | `MesoFormat_Trade0` | `Format_Trade_Meso0_2` |
| 19 | `0x7C314F` | `0x428226` | `MesoFormat_Trade1` | `Format_Trade_Meso1_1` |
| 20 | `0x7C3238` | `0x428226` | `MesoFormat_Trade1` | `Format_Trade_Meso1_2` |
| 21 | `0x6FBA12` | `0x428226` | `MesoFormat_Hm` | `Format_HM_Meso_1` |
| 22 | `0x6F0388` | `0x988690` | `MesoFormatNumber_Character` | `FormatNumber_PersonalShop_Meso` |
| 23 | `0x6F03BE` | `0x428226` | `MesoFormat_Character` | `Format_PersonalShop_Meso` |
| 24 | `0x755E97` | `0x988690` | `MesoFormatNumber_Character` | `FormatNumber_CShopDlg_Meso` |
| 25 | `0x755ED1` | `0x428226` | `MesoFormat_Character` | `Format_CShopDlg_Meso_FuseFmt` |
| 26 | `0x7562C5` | `0x428226` | `MesoFormat_Character` | `Format_CShopDlg_Meso_FuseA` |
| 27 | `0x756364` | `0x428226` | `MesoFormat_Character` | `Format_CShopDlg_Meso_FuseB` |
| 28 | `0x5B885E` | `0x428226` | `MesoFormat_Character` | `Format_Settle_Meso_1` |
| 29 | `0x5B8B5F` | `0x428226` | `MesoFormat_Character` | `Format_Settle_Meso_2` |
| 30 | `0x7C8909` | `0x4065A6` | `MesoEncode8_StorageWithdraw` | `Encode_Storage_Withdraw` |
| 31 | `0x7C8A14` | `0x4065A6` | `MesoEncode8_StorageDeposit` | `Encode_Storage_Deposit` |
| 32 | `0x7C3952` | `0x4065A6` | `MesoEncode8_TradeSetMeso` | `Encode_Trade_SetMeso` |

### 2.4 格式化辅助函数

| EA | 符号 | 行为 |
| --- | --- | --- |
| `0x1009F4D0` | `FormatMesoCommas` | `u64` → 十进制字符串，从右往左每 3 位插一个逗号 |
| `0x1009F8A0` | `MesoFormatNumber_Active` | 取 `g_pActiveFmt`（当前面板的 64 位值），无则回退 `g_lMeso`，写 `zxstr` |
| `0x1009F8F0` | `MesoFormatNumber_Character` | `g_lMeso` → `zxstr` |
| `0x1009F930` | `MesoFormatNumber_Fredrick` | `g_lFredrickMeso` → `zxstr` |
| `0x1009F970` | `MesoFormat_Active` | 同 `MesoFormatNumber_Active`，直接产出最终文本 |
| `0x1009F9B0` / `0x1009F9D0` / `0x1009F9F0` | `MesoFormat_Character` / `_Fredrick` / `_Hm` | 各自全局值 → 文本 |
| `0x1009FA10` / `0x1009FA30` | `MesoFormat_Trade0` / `_Trade1` | 交易窗两槽的金币文本 |
| `0x1009FC00` / `0x1009FC20` | `MesoWrapFmt_*_Naked` | 17 B 裸跳板，仅转发到 `FormatNumber_StorageUI` |

- 全局 64 位值：`g_lMeso`（角色）、`g_lFredrickMeso`、`g_lHmMeso`、`g_lStorageMeso`、`g_lTradeMeso[2]`；对应高位 dword 符号见解码函数反汇编（如 `dword_101D2774`）。
- 所有格式化都走 `FormatAssignGame(zxstr, lo, hi)`（与游戏自带数字格式化同款，带千分位），保证 UI 文本与其他金额风格一致。

### 2.5 当前端落地映射（待实现）

| 参考端语义 | 当前端目标文件 | 说明 |
| --- | --- | --- |
| 角色金币解码（站点 01/02） | `Net/Handlers/PlayerHandlers.cpp:84`、`Helpers/CharacterParser.cpp:26` | 现为 `read_int()`，改读 lo+hi |
| 仓库 / 交易 / 雇佣商人解码（06–09） | 对应 handler + `UI*` 打开包 | 需逐个封包核对字段顺序 |
| 显示格式化（10–29） | `IO/UITypes/UIShop.cpp`、`UIItemInventory.cpp`、`UIStorage.cpp`、`UITrade.cpp`、`UIPersonalShop.cpp`、`UICashShop.cpp` 等 | 已统一走 `int64_t` + `split_number`，只需确认入口值来自 64 位全局 |
| 编码（30–32） | `Net/Packets/*` 的存款/取款/设置金币 | 写 lo 再写 hi，哨兵语义见 §2.2 |

- 服务端必须同步支持 8 字节字段，否则客户端解码会错位（P3 范围，服务端源码 `E:\project\BeiDou-Server`）。
- 当前端**不要**照搬「钳到 INT_MAX」的玩法语义之外的部分：若只做显示对齐，应保持现有 `int64` 存储，仅把封包读取与格式化改成 64 位。

---

## 3. 复现与维护

```powershell
# 反编译指定函数（写入 dec_out.txt）
Set-Content C:\Users\11036\AppData\Local\Temp\opencode\dec_targets.txt -Value '1009EE00','1009F5A0'
& "D:\software\IDA Professional 9.4\ida.exe" -A -S"docs\features\beidou-v17-parity\notes\scripts\ida_decompile.py" `
  "C:\Users\11036\AppData\Local\Temp\opencode\ida_ijl15\ijl15.dll.i64"
```

- 该库 `ijl15.dll.i64` 为持久库，重跑脚本不会丢失符号；Hex-Rays 对部分函数返回 `None`，脚本自动回退反汇编。
- 版本升级后优先复核 §2.3 的 `原目标` 列：参考端自身就靠这 4 个目标值做漂移检测，目标变了说明 exe 结构变了，站点表必须重算。

## 4. 变更记录

| 日期 | 内容 |
| --- | --- |
| 2026-10-21 | 首版：BuffTimer 证据表 + MesoUncap 32 站点全表（7 解码 / 22 格式化 / 3 编码） |
