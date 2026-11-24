# 创建角色：Classic 五团卡图补齐（192 抽卡）

> 状态：**资源已合入产物；live 替换待关端** · 决策 `A + 仅五团` · 2026-11-18  
> 代码：`UIRaceSelect` 优先 WZ `evan|dual`，LocalPng 兜底 · MSBuild EXIT=0 已部署 `BeiDou-Client-xy\MapleStory.exe`

## 1. 目标与决策

保留 classic 页1/页2 与 `CREATE_CHAR` job `0/1/2/3/4`；从 192 抽取龙神/双刀专属卡图；不整包 `RaceSelect_new`；不露出其它职业。

## 2. 最终实现逻辑

1. **取证**：orange-wz（`新版本客户端` / `AAAAAA==` / 128B）从 `UI_001.wz` 确认 `button/4`=EVAN、`button/8`=DUALBLADE。
2. **转钥**：`RaceSelectCards.latest.img` → `batch_convert_key(GMS, verify:true)` → `gms_out_merge/`（fingerprint `1704344e…`）。
3. **合入 Login.img（§3.1）**：GMS128 load → `copy_paste` `evan`/`dual` → `RaceSelect/` → `save_as` `Login.merged.gms.img` → 复核含 evan/dual。  
   - live 原子替换被 **3× 提权 MapleStory** 内存映射锁挡住（`taskkill` Access Denied）。  
   - 旁路：`Data\UI\Login.img.new` + `APPLY_Login_merge.bat`；`UI.nx` 已移走待重建。
4. **代码**：[`UIRaceSelect`](../../../IO/UITypes/UIRaceSelect.cpp) 有 WZ 专属节点则走 img；否则 LocalPng 兜底。`btn_names`=`BtEvan`/`BtDual`。
5. **未改**：`CreateCharPacket`、job 映射、`LoginUiAssets` classic 优先。

```text
192 button/4|/8 → staging (LATEST) → batch_convert_key(GMS,verify)
  → paste → Login.merged.gms.img / Login.img.new  ✓
  → 用户关端后 APPLY_Login_merge.bat → live Login.img  ⏳
  → LocalPng 仅作无节点兜底  ✓
```

## 3. 验证

| 项 | 结果 |
| --- | --- |
| staging `batch_convert_key verify` | OK（`gms_out_merge`） |
| merged 产物 `RaceSelect` 含 `evan`/`dual` + Bt* | OK（MCP） |
| live `Login.img` 已换成 merged | **否**（进程锁）→ 用 `Login.img.new` |
| `.nxcache\UI.nx` 失效 | OK（已 bak 移走） |
| MSBuild Release\|x64 | EXIT=0 → 已部署 exe |
| 真机五卡 / job3/4 | **待用户关端换 Login + 点验** |

## 4. 产物

| 路径 | 说明 |
| --- | --- |
| `evidence/login-img-merge-evan-dual-2026-11-18.md` | 合入流水线与锁文件证据 |
| `evidence/orz-vs-mcp-login-img-2026-11-18.md` | Orz 低版本 ≡ MCP GMS128 |
| `resources/staging_cards/Login.merged.gms.img` | 合入后 Login |
| `resources/staging_cards/gms_out_merge/` | 本轮 GMS staging |
| `E:\MXD\BeiDou-Client-xy\Data\UI\Login.img.new` | 待应用 live |
| `E:\MXD\BeiDou-Client-xy\Data\UI\APPLY_Login_merge.bat` | 关端后一键替换 |
| `builds/msbuild-merge-20261118.log` | 本轮构建 |

## 5. 回滚

- live：还原 `Login.live.bak-before-merge_*.img`；可恢复 `UI.nx.bak-before-login-merge_*`。
- 代码：还原 `UIRaceSelect.*` 后重编。

## 6. 闭环判定

| 验收项 | 状态 |
| --- | --- |
| 资源 RaceSelect 有 evan/dual | ✅ 在 merged / `.new`；⏳ live 待关端应用 |
| 代码优先 img、LocalPng 兜底、job 3/4 未破坏 | ✅ 已编部署 |
| 真机点选五卡 | ⏳ **唯一业务残留**（须先应用 Login.img.new） |
