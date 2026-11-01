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
python live/open_window.py i k            # -> E:/project/_r27_win_i.png / _r27_win_k.png

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
