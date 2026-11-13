# 真机验证补充 — Round 10：端到端进图成功

> 承接 `runtime-verification-r9.md`（只验到登录页）。本轮用仓库既有的测试账号约定
> 完成**端到端进图**，拿到包级 + 标题双重铁证。
> 结论：**7 项改动在真实客户端里跑起来了，未崩溃**。槽位/套装/掉落的**视觉确认**
> 仍需人为点开窗口（见 §5）。

## 1. 账号来源（不是猜的）

`docs/features/beidou-v17-parity/notes/r11-baseline-replication.md:44` 逐字写明：

> 黑屏环境既有问题不修；验证靠 s9 日志 + **自动登录回归(admin/admin 进图)** + 用户实机确认

另有 `docs/features/live-accept-2026-11-06-qsi.md:6`「账号：`admin` / 角色 Admin」。
`xy-config.yaml` 里 `login.account: admin` / `login.pass: admin` 本就存在，只有 `auto: false`。

## 2. 启用自动登录（含一次 BOM 事故）

| 步骤 | 结果 |
|---|---|
| 把 `login.auto: false → true` | 成功 |
| **写回后检查编码** | 发现文件被写成 **UTF-8 BOM**（`EF BB BF`）—— 违反 MEMORY 里「config.ini 不带 BOM」铁律 |
| 剥 BOM 重写 | `first3` 从 `b'\xef\xbb\xbf'` 变 `b'# M'`，size 2833，内容完好 |
| 同时打开 `debug.perf_log` / `debug.packet_dump` | 用于取证（详见 §3） |

> 教训：Python `io.open(..., encoding='utf-8')` 写文件时若源串带 BOM 或用了 `utf-8-sig`，会带 BOM 写出。
> 本项目读 yaml 时不剥 BOM 会解析失败 —— 所以改完必须回读前 3 字节验证。

## 3. 包级铁证：真的进图了

`packet_dump.log`（2,376B）关键序列：

| 方向 | 包 | 含义 |
|---|---|---|
| SEND | `01 00 05 00 61 64 6d 69 6e ...` | LOGIN_PASSWORD，账号 `admin` |
| RECV | `0a 00 00 06 00 53 63 61 6e 69 61 ...` | WORLD_LIST，世界名 `Scania` |
| SEND | `0b 00` | 选世界 |
| RECV | `0b 00 00 03 ... 41 64 6d 69 6e` | CHAR_LIST，角色 `Admin`（Lv.132） |
| SEND | `13 00 01 00 00 00 11 00 44 43 2d ...` | 上报机器信息 |
| SEND | `14 00 01 00 00 00` | **进入游戏通道** |
| RECV | `64 00 ...` = **0x64 SET_FIELD** | **切换地图**（含 GBK 地图名/坐标） |
| RECV | `ec 00 0x01..0x23`（35 个）= **0xEC SPAWN_MOB** | **刷怪**（带 mobId + 坐标） |
| RECV | `23 00` / `3f 00` / `5f 00` / `9f 00` | SHOW_STATUS / 技能表 / 快捷栏初始化 |
| RECV | `7a 00 16 00 41 64 6d 69 6e ...` | 「Admin Admin 登陆了游戏」 |

`0x64 SET_FIELD` + 35 个 `0xEC SPAWN_MOB` 是**只有真正进入地图才会收到**的包型。

## 4. 窗口标题铁证

```
XiaoYE MS v0.0.1【1-Admin】【132-暗之黑骑士】【破功值：199999】【FPS:224延迟:--】
        【当前时间：2026-11-12 20:20:59】【在线时长：00 00:00:28】
```

标题出现 `等级-角色名`、`职业`、`在线时长` —— 登录页阶段这三项**不可能**出现。

网络侧：客户端 `Established` 到 `127.0.0.1:7575`（游戏通道），8484（登录服）已走完 TimeWait。

## 5. 帧捕获

| 文件 | 说明 |
|---|---|
| `live-r9-frame.png` | Round 9，登录页（9,537 色） |
| `live-r10-field.png` | 刚进图瞬间（4,736 色，中心 `(103,145,255)`） |
| `live-r10-ingame.png` | 进图后（**8,264 色**，最大单色仅 3%） |

> 注：`PrintWindow(PW_RENDERFULLCONTENT)` 对 GL 渲染的游戏画面可能抓到的是合成层而非
> 游戏世界本身，因此**像素判据统计只用于「非白屏/非单色」的排除性判定**，
> 「是否进图」以 §3 包转储 + §4 标题为准（两者都是不可伪造的运行时事实）。

## 6. 仍未验证的部分（需要人点开窗口）

以下四项需要人为操作 UI，无法由我自动完成（也没有可靠的输入注入通道）：

1. **②③④ 槽位**：打开装备栏 → 看右上 2 戒指 `(104,65)/(137,65)`、眼饰左侧第二项链 `(38,98)`、行7 护肩 `(71,230)`；同键开辅助装备子窗（图腾×4 + 纹章/机器人/心脏/徽章）。
2. **⑤ 套装面板**：悬停属某套装的装备 → 右侧出现 `套装名 (n/N)` + 逐档加成。
3. **⑥ 掉落光环**：打怪掉落**有品质**的装备 → 双击地面掉落物周边出现 front/back 双层光环 + 开场光柱。
4. **⑦ 潜能 5 行**：悬停带潜能的装备 → tip 里「可升级次数」之后出现最多 5 行（附加潜能带 `(B)` 前缀）。

## 7. 复现命令

```powershell
# 1. 确保 xy-config.yaml 里 login.auto: true（且文件无 BOM）
# 2. 启动（勿用 Start-Process，会挂 shell）
cd E:\MXD\BeiDou-Client-xy
cmd /c start "" MapleStory.exe
# 3. 约 25s 后看标题栏出现【等级-角色名】【职业】【在线时长】= 已进图
# 4. 打开装备栏开始 §6 的四项视觉确认
```
