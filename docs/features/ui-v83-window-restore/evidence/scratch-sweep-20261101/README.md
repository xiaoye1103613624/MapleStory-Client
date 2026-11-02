# 本机临时件清理（2026-11-01，按「是否仍被引用」判据）

`E:/project` 根目录长期堆着历轮自证用的 `_` 前缀截图/探针/日志。本轮把它们三档分类，
只删**无任何引用**的那一档，并把清单和判据脚本留在仓库里，下次能复算。

判据与实现：[`../../tools/scratch_sweep.py`](../../tools/scratch_sweep.py)
（`classify` 分类 / `verify <dropped.txt>` 删除后自检）。引用语料 = 本仓库工作树 +
项目/用户记忆 + 全局 skill；已判 KEEP 的文本脚本再做传递闭包，防止「文档引用脚本、
脚本引用图」的链条被从中间剪断。目录一律不参与删除（见文末）。

## 结果

| 档 | 数量 | 体积 | 处置 |
| --- | --- | --- | --- |
| keep（精确文件名被引用，含活体驱动链 5 个） | 125 | 13.8 MB | 留在 `E:/project` |
| review（仅裸词干被提到，无扩展名级引用） | 21 | 1.2 MB | **保守也留下** |
| drop（无任何引用） | **1529** | **168.9 MB** | 已删除 |
| dirs | 12 | 见下 | 未动 |

删除与自检的原始输出（逐字）：

```text
deleted=1529 missing=0 reclaimed_MB=168.9
remaining=163 files=151 dirs=12

dropped=1529 still_referenced=0
verify_exit=0
```

删后再跑一次 `classify`，此时只剩我自己的 6 个清理助手脚本落在 drop 档（0.0 MB），
一并删掉；仓库工具因此可反复跑而不会误伤。

## 留下的两档为什么不再细分

`review` 这 21 个是文档里以 `_r20_icon`、`_cap2`（无 `.py`）这种裸写法提到的，
按「引用即保留」的字面判据它们不算被引用，但裸词干撞名与真引用的区分收益很低、
误删代价是一次不可复核的取证。因此**这一档本轮不删**，等下一次真要腾空间时人工过一眼。
清单见 [`review_bare_only.txt`](review_bare_only.txt)。

## 已知副作用（3 个活脚本的引用断链）

`classify` 的闭包只认「被保留脚本精确引用的文件」，反向不成立，所以删完后仍有 3 个
**留下的**脚本指向**删掉的**图：

```text
DANGLING _cap2.ps1    -> _pid.txt              （写出的中间文件，重跑会自动重建）
DANGLING _cap_hud.ps1 -> _cap_our_hud.png      （写出的截图，同上）
DANGLING _r20_job2.py -> _r20_comp6.png, _r20_nat11.png   （读取的输入图，已不在 ⇒ 要重跑须重新取证）
surviving_scripts=45 with_dangling_refs=3
```

前两个是输出路径，无害；只有 `_r20_job2.py` 是输入丢失，它属于 R20 那一轮的比对脚本，
结论早已落进仓库文档，需要复算时得重新抓那两张图。

驱动链冒烟（只读，不发键）：

```text
hwnd=26545976 window=1296x759@(632,385) client_origin=(640,416) cursor=(1254,1360) fg=3410776 fg_ok=False
```

即客户端实例仍在（hwnd 与清理前同一个），`fg_ok=False` 是前台被别的窗口占着，
打键类自证前仍需按项目记忆先断言前台。

## 未动的 12 个目录（等拍板）

```text
_ida_beidou   files=2     MB=174.3   IDA 数据库（.i64）—— 不是临时件
_sdl3dl       files=106   MB=73.4    9 月的 SDL3 下载/构建残留
_ms_probe     files=763   MB=30.6    11-01 仍在写
_rea_work     files=2     MB=8.1
_nat          files=9     MB=2.7
_audit_out / _audit_tmp / _ida_ui_out / _uiinv / _w6_pngs / __pycache__  < 1 MB
_t11          空
```

合计约 290 MB，其中 `_ida_beidou` 删了要重跑整轮反编译分析，绝不随批删。

## 追加（同日第二轮）：目录处置结果与判据补查

上一段是**待拍板快照**。补查发现两个必须先修的漏洞：

1. `classify` 的语料白名单里没有 `.vcxproj/.props/.targets`，所以「构建是否引用」根本没被查到 ⇒ 本轮改为
   对全仓（含构建配置、排除 `build*`/`.vs`）做词边界 grep 复核；
2. 早先的子串 grep 会把 `_native-samples` 误算成 `_nat` 的引用 ⇒ 改用 `[/\"'\` (=（(]$d([/\"'\`)])` 词边界，
   `_nat` 的 40 条「引用」实际全是 `_native-samples` 撞名，真引用为 0。

处置（`_nat` 里 9 张原生帧先按 md5 与 `r27-oracle/frames/` 比对，8 张是已归档帧的副本，
唯一不重复的 `bottom_btns.png` 先入库为
[`../r27-oracle/frames/native_bottom_btns.png`](../r27-oracle/frames/native_bottom_btns.png) 再删）：

```text
rmdir _nat           freed=   2.7 MB   （8/9 与归档帧同 md5，第 9 张已入库）
rmdir _sdl3dl        freed=  73.4 MB   （构建实际取 includes/SDL3，仓库自带；这里是下载缓存）
rmdir _rea_work      freed=   8.1 MB   （BeiDou_copy.exe + 探针脚本，无引用）
rmdir _t11           freed=   0.0 MB   （空目录）
rmdir _uiinv         freed=   0.0 MB
rmdir _audit_out     freed=   0.3 MB
rmdir _audit_tmp     freed=   0.2 MB
rmdir _ida_ui_out    freed=   0.5 MB
rmdir __pycache__    freed=   0.1 MB
freed_total_MB=85.3
kept  _ida_beidou   174.3 MB   BeiDou.exe.i64：IDA 数据库，删了要重跑整轮自动分析
kept  _ms_probe      30.6 MB   port-095 opcode 文档生成脚本的工作目录（README 明写「须在 E:/project/_ms_probe/ 运行」）
kept  _w6_pngs        0.1 MB   evidence/w6-stats/coords.md:11 仍指向它
```

**并发现场**：清理时 `E:/project/_audit/` 正在被写（21:16–21:25，`equip/inventory/minimap/skill/stats/henesys.png`），
仓库同时有一处不属于本会话的 `IO/UITypes/UIMapSearchPanel.cpp` +255 行未提交改动 ⇒ 另一个会话正在并行做
UI 取证与实现。本轮只提交文档，未 stage 那个源文件，也没碰 `_audit/`。

