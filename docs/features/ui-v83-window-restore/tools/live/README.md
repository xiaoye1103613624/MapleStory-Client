# 真机自证驱动（`E:/project` 归档）

用于「构建 → 部署 → 自己登录进图 → 开指定窗 → PrintWindow 抓帧 → 跑 `layout.py check`」这条链。
本机 `E:/project` 里的通用件 `_drive.py` / `_postkey.py` / `_cap2.py` / `_rclick.py` / `_quit.py`
**不在仓库**（它们绑的是这台机器的路径与窗口标题），本目录只收功能相关的驱动脚本。

## 跑法

```bash
# 1) 启动 + 登录 + 选角 + 进世界，每步抓帧，日志落 E:/project/_r27_login.txt
set R27_USER=<本机 xy-config.yaml 里的账号> && set R27_PASS=<口令>
python live/login.py

# 2) 按键开指定窗并抓帧（抢不到前台就中止，不发任何输入）
python live/open_window.py i k            # -> E:/project/_r27_win_01_i.png / _r27_win_02_k.png

# 3) 背包展开态：模板匹配反推窗口左上，点 BtFull，再抓一帧
python live/expand.py

# 4) 门禁对账（在仓库根执行）
python docs/features/ui-v83-window-restore/tools/layout.py check \
    docs/features/ui-v83-window-restore/evidence/r27-oracle/ItemFull.json <抓到的帧>
```

## 三条硬前提

1. **口令不入库**：`login.py` 只读 `R27_USER` / `R27_PASS`，缺任一直接退出。远端是公开仓库。
2. **打键前必须断言前台**（项目记忆 `live-client-verification-gotchas` 第 6/8 条）：
   未聚焦打键会烧快捷槽道具，且 `360` 的隐形钩窗会长期抢前台。`_drive.py` 里有 `foreground()`；
   抢不回前台时把该项转成用户手动 20 秒，别硬试。
3. **窗口句柄动态取**：`_drive.find_hwnd()` 按标题含 `XiaoYE` 匹配。写死 hwnd 会在下次启动后
   锚到别的窗（实测 agree=0.053 那种「看起来像没修好」全是它造成的）。

## 真机量到的坐标与操作前置（2026-11-03）

这一节全是**实测**，不是设计意图。改动前请先按现帧复量，别照抄旧值——`login.py` 的
密码/连接坐标就曾经长期是错的（旧值 `(582,330)` 其实压在账号行上）。

| 事项 | 实测值 / 前置动作 | 后果（不做会怎样） |
| --- | --- | --- |
| 登录页三个点击点（client 相对） | 账号 `(588,302)` / 密码 `(642,354)` / 连接 `(774,317)` | 密码打进账号行、连接点了个空 |
| 世界选择面板 | 面板**默认折叠**，要先按一下 `Enter` 展开，`BtGoWorld` 才落到可点位置 | 点不到进世界按钮，卡在选角 |
| 进世界之后 | **先点一下窗口**（拿激活）再打键 | 热键不生效，`open_window.py` 抓到的还是裸画面 |
| 帧 ↔ view 坐标 | PrintWindow 帧 = 客户区 **+ 边框 (8,31)**；窗口 1296×759 / 客户区 1280×720 ⇒ 1:1 平移 | 差 8/31 的相位，逐格对账全错 |
| 窗口落位来源 | 读运行目录 `Settings` 的 `ui.position.*`（如 `skill: (560,99)` ⇒ 帧左缘 568） | 靠猜窗口左上，锚点定不出来 |
| 新手号（Lv.1） | 背包三页**全空** ⇒ 「hover 落在压住的那颗图标」这类像素判据在背包上做不出来，改在**技能窗 / 键盘设定窗**验 | 拿空格子测命中，读数全是噪声 |
| Release 构建 | `MapleStory.h:75` 的 `LOG` 只在 `_DEBUG` 下走 `std::cout` ⇒ **Release 里是空** | 把日志当运行期判据，读到永远为空 |
| 前台断言失败时 | `_drive.foreground()` 抢不回就**中止**，一个输入都不发（实测 `FOREGROUND LOST: hwnd=… still held by …`） | 未聚焦打键会烧快捷槽道具 |
