# r01 解析摘要：MapleStoryT vs 192 素材 vs 当前端 HUD

## 工具状态

| 层 | 状态 |
|---|---|
| IDA 13339 | ✅ `MapleStoryT.i64` 已挂；`hexrays_ready=true`；`auto_analysis_ready=false`（本轮以 PE/字符串为主） |
| REA MCP | ❌ `user-rea` STATUS 报错，未用 |
| orange-wz | 本轮未对 192 分卷 UI.wz 做 load（ResMan 加密分卷，明文扫不到 `StatusBar*`） |

## 已证实

1. **MapleStoryT ≠ 192 客户端，也 ≠ 083 Angel**  
   - PE32 / x86；串：`UI/StatusBar2.img/mainBar/...`、`UI/UIWindow2.img/*`、`UI/UIWindow4.img/*`（含 Android/StarForce/AdventureLogbook）  
   - 无 `StatusBar.img` / `StatusBar3` / `ResMan` / `libcef` 明文契约  
   - 有 `XignCode`、`MapleStory.WebBrowserUI`、`TMS Client` 痕迹  

2. **192 整合客户端是现代 ResMan 端**  
   - `Data\{Base,UI,...}\*_NNN.wz` 分卷；`libcef.dll` / CEF pak；SDO 登录；`FileVersion 1,0,0,124`  
   - 与 MapleStoryT 的 IMG 路径契约、打包格式、运行时均不对齐  

3. **当前仓库 HUD 已具备「双皮肤骨架」**  
   - `UiAssetResolve::is_classic_hud()` → `UIStatusBar::init_classic()`（083 `StatusBar.img`）  
   - 否则 `init_modern()`（`StatusBar3`/`mainBar`：status / EXPBar / menu / quickSlot / submenu）  
   - 仓库既有裁决：`怀旧岛192` 不作本线功能/资源权威参照（见 `feature-gap-inventory-2026-11`）  

## 差异与推荐

| 目标 | 结论 |
|---|---|
| 给 MapleStoryT 直接换 192 整包素材 | ❌ 不合适，无法功能闭环 |
| 把 192「整端功能」移植进当前仓库 | ❌ 不可行（协议/系统面跨代） |
| 把 192 **底栏位图**当皮肤迁到当前端 | ⚠️ 仅皮肤级可行，需解包+节点映射；成本高于用 StatusBar3/StatusBar2 |
| 083 ↔ 另一套底栏皮肤可切换 | ✅ 可行；应复用现有 classic/modern，第三套「192 原节点」不建议一期做 |

## 局限

- MapleStoryT 自动分析未完成；`StatusBar2` 路径可能大量走 StringPool，本轮仅抓到 1 条明文路径。  
- 192 UI 分卷未成功明文枚举 StatusBar 节点名；「192 HUD 节点树」仍待 orange-wz 专用 key/解包后补证。  
