# 客户端路径记忆

> 规则来源：全局规范门禁 16「用户给过的客户端路径须落盘」。本文件只记**已确认在用**的路径，
> 每次涉及构建产物同步、真机驱动、原生对照取证前先核对此处，不要重新猜路径。

## 本仓（MapleStory-Client）的运行目录 = 构建 OutDir

| 项 | 值 |
| --- | --- |
| 路径 | `E:\MXD\BeiDou-Client-xy\` |
| 可执行 | `MapleStory.exe`（x64 / Release） |
| 关系 | vcxproj 的 `OutDir` **就是这个目录** —— 构建成功即已部署，无需再手工拷贝 |
| 窗口标题 | 含 `XiaoYe`（真机驱动 `_drive.py find_hwnd` 按此匹配） |
| 配置 | `xy-config.yaml`（`login.auto` 控制选角页自动进图；**凭据只运行时读取，不得写进任何输出或文档**） |
| 运行时观测 | `packet_dump.log`（出站/回程）、`_ui_asset_diag.log`、`atlas_dbg.log` |

exe 身份判定用 **md5 + size + mtime** 三元组；`TimeDateStamp` 在全量 Rebuild 后非确定，不可当依据。

## 原生对照参考端（只读取证，不写）

| 项 | 值 |
| --- | --- |
| 路径 | `E:\MXD\BeiDou-ClientV17.7\` |
| 可执行 | `BeiDou.exe`（32 位，8503296 字节） |
| IDA 会话 | MCP `ida_beidou`（已加载的 gui session，反编译/xref 都走它） |
| 素材 | `Data/UI/*.img`（`StatusBar.img/BtNPT`、`ITC.img/Auction`、`Npt_GuideLine.img` 等） |
| 抓帧 | 窗口类名 `MapleStoryClass`；**PrintWindow 全黑**，只能屏幕抓帧（`_ndrive.py`） |

## 本机其它冒险岛目录（`find` 探到，未在本线使用）

`E:\MXD\BeiDou-ClientV16.1\`、`E:\MXD\BeiDou-Client_S9\`、`E:\MXD\20dalu_079MS\客户端\`、
`E:\project\sdlMS\build\Data.*`、`E:\MXD\265\`。
**动它们之前先问用户** —— 可能是别的版本线或别的会话在用。

## 并行会话占用（本轮踩过的坑）

OutDir 是**共享**的：另一个会话构建并启动客户端后，本会话链接会 `LNK1104`。
处置口径见全局规范「插件同步与进程占用」——**不得擅自结束对方进程**；
先 `tasklist //FI "IMAGENAME eq MapleStory.exe"` 看清 PID 与启动时间，再问用户怎么办。
