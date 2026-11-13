# 2026-11-07 续作：item 10「自由市场 MapleTV 没有画面显示」

## 结论

不是 ABI / 字体回归，而是**原生场域大屏贴图从未接入**：`CNpc::SetMapleTVMessage`
对 `info/MapleTV=1` 的 NPC 模板在立绘之上叠两块屏幕贴图，本客户端此前只读
`UI/UIWindow.img/MapleTV`（撰写留言的对话框），全仓 `.cpp/.h` 对
`UI/MapleTV.img` 的引用为 0，因此两块屏幕永远保持立绘里烘焙的深灰色。

本轮按运行时数据 + 原生字符串 + 服务端 WZ 交叉取证后，在 `Npc` 上补齐了
**默认待机画面**（上屏台标 + 下屏轮播广告）。

## 证据（均为只读解析，未改数据）

| 层 | 路径 / 值 | 结论 |
| --- | --- | --- |
| 运行时 NPC 模板 | `Data/.nxcache/Npc.nx` → `9250025.img/info` | `MapleTV=1`、`MapleTVmsgX=65304(short -232)`、`msgY=-379`、`adX=-237`、`adY=-261`、`hideName=1`；`String.wz` 名称 = 自由市场枫叶电视 |
| 运行时立绘 | 同上 `stand/0` 442x570，`origin=(321,569)` | 两块深灰屏幕的像素实测：上屏 x89..328 / y190..279 (240x90)，下屏 x84..323 / y308..487 (240x180) |
| 运行时 UI | `Data/.nxcache/UI.nx` → `MapleTV.img` | `TVbasic/0` 240x90、`TVmedia/{0,1,2}` 240x180×20/24/25 帧(delay 150/250ms)、`TVon` 16 帧、`TVoff` 11 帧、`TVchat/0` 240x90、`TVchat1/2` 268x113；全部 `origin=(0,h-1)` |
| 原生客户端 | `docs/features/native-map-object-re/evidence/beidou_v177_decrypted_strings.tsv:3940-3956` | 明文引用 `UI/MapleTV.img/TVmedia`、`TVmedia/%d`、`TVon`、`TVof`、`TVbasic/0`、`TVchat/0`、`TVchat2` |
| 原生逻辑 | `docs/features/change-quickslot-persist/evidence/cnpc-update-0x677b50.decompiled.txt:115-116` | `if (m_pTemplate->bMapleTV) CNpc::SetMapleTVMessage(this)`；`beidou_exe_utf16_full.txt` 含 `MapleTVadX/msgX/msgY/mMapleTVadY` 字段名 |
| 服务端 v83 WZ | `E:\project\BeiDou-Server_s9\gms-server\wz\Npc.wz\92700*.img.xml` | 一批同类模板均带 `MapleTV=1` 与四个 short 偏移；`String.wz/Npc.img.xml` 9250025 = Free Market Maple TV |
| 服务端 v83 协议 | `PacketCreator.enableTV()/sendTV()/removeTV()` + `MapleTVEffect.broadcastTV()` | 343/341/342 为「开播 / 下发立绘+5 行文案 / 15~60s 后撤下」的世界广播 |

### 偏移换算（与像素实测完全吻合）

立绘以 `origin=(321,569)` 对齐 NPC 锚点 `absp`，WZ 偏移相对 `absp`：

- 上屏左上角 = `(321 + msgX, 569 + msgY)` = `(89, 190)` → 实测深灰区 `(89,190)`，尺寸 240x90 = `TVbasic/0`
- 下屏左上角 = `(321 + adX, 569 + adY)` = `(84, 308)` → 实测深灰区 `(84,308)`，尺寸 240x180 = `TVmedia/%d`

`msgX` 在 `.nx` 里是 `0x0000FF18`(65304)，必须按 **int16** 解释成 -232；
其余三个是 `0xFFFFFFxx`，同样按 int16 还原。s9 的 WZ XML 明确 `msgX` 是
`<short>`，印证这不是巧合。

## 实现

- `Gameplay/MapleMap/Npc.h/.cpp`
  - 构造期检测 `info/MapleTV`，读四个偏移（统一 `static_cast<int16_t>`），
    从 `nl::nx::UI["MapleTV.img"]` 载入 `TVbasic/0` 与 `TVmedia/0`。
  - `draw()` 在立绘之后叠两块屏幕；位图是 bottom-left origin，故绘制锚点
    = `absp + 偏移 + 当前帧 origin`，正好落在深灰区左上角。
  - `update()` 按 WZ 帧 `delay` 推进下屏广告轮播。
  - R6 守卫：`TVbasic` / `TVmedia/0` 缺失时只保留立绘（先判节点再构造
    `Animation`，避免空 `Animation` 越界取 `frames[0]`）；镜像模板（`flip=true`）
    也先只留立绘，因四个偏移需按镜像重算，无原生证据不猜。
- `IO/UITypes/UIChatBar.cpp`
  - item 14 兜底：`base/chatTarget` 缺失时铺一块 `81x20` 深蓝框，避免
    `WHITE` 目标文字压在 `backgrnd` 烘焙的白色输入带上变成「不显示」。
    运行时 `StatusBar.img/base/chatTarget`(81x20) 实测存在，故本包不触发，
    属防御性守卫。

## 验证

- `tools/build_smoke.bat`：`GATE OK` / `EXITCODE=0`，无 error；
  部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe` 已刷新（本轮 10:42）。
- 几何取证：`tools/mapletv-compose-20261107.py` 按上述偏移把运行时位图合成到
  立绘上，`evidence/mapletv-20261107/tv_before_stand.png`（空屏）与
  `tv_after_expected.png`（双屏有画面）可直接对拍；脚本打印的
  `screen LT = msg (89,190) / ad (84,308)` 与 PIL 量出的深色区完全一致。
- 回归截图（pass6，均为新 exe）：`r6_01_macro_three_rows.png` 组合栏三行、
  `r6_02_both_closed.png` 技能窗+组合栏同时关闭 → item 12 闭环。

## 显式预留（未凭猜测实现）

1. **开播态**：`SEND_TV`(341) 的 `CharLook` 角色立绘、`TVchat/0` + `TVchat1/2`
   气泡、5 行文案排版，以及 `ENABLE_TV/REMOVE_TV` 驱动的 `TVon/TVoff`
   过场。当前 `WaveAShellHandlers.cpp` 仍 `skip_tv_charlook`，只把文案塞进
   `UIMapleTV` 提示窗；要做需先 REA/IDA 拿到 `CNpc::SetMapleTVMessage` /
   `CMapleTVMan` 的层叠与排版证据，再定 Plan，避免把广播画面画错位置。
2. **真机确认**：回归自动登录落在「番茄田/彩虹岛」，NPC 9250025 在
   `910000000`（自由市场），自动脚本未走到大屏前。需用户进自由市场站在
   MapleTV 旁确认双屏动画与木框无错位；如有偏移按上面四个 WZ short 复核。
3. **item 11 点击**：pass2/4/5b/6 的自动点击坐标均未命中目标 NPC（截图无
   对话框），属驱动坐标问题，不能据此判「点击功能坏」；用户手点一次即可闭环。
