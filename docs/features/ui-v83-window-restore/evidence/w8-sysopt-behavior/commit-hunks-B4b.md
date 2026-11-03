# R28-B4b 提交 hunk 归属清单（并发工作树，逐 hunk 暂存）

共享工作树里并发会话（R28-B5 字体 + 其它在途）与本功能（R28-B4b 系统设置修复包）混在
同一批 modified 文件里。本笔提交**只 stage 下列 hunk**，其余留给对方。

## Configuration.cpp（hunk 头按提交前 `git diff -U3` 行号）
| hunk | 内容 | 归属 |
| --- | --- | --- |
| @@ -16,6 +16,7 @@ | `+#include "MapleStory.h"` | **并发（R28-B5）**，不 stage |
| @@ -76,6 +77,10 @@ | 启动 段 boot.logo{,.frame_ms} 两行 | 本功能 |
| @@ -97,6 +102,7 @@ | 登录 段 login.auto 一行 | 本功能 |
| @@ -108,10 +114,29 @@ | 系统设置 9 键 + 游戏设置 + 自动喝药 2 键 + 界面补 maplepoints | 本功能 |
| @@ -141,7 +166,15 @@ | 界面位置 familytree/gameopt/sysopt/dailygift/partyrace + 调试 | 本功能 |
| @@ -243,6 +276,7 @@ | `settings.emplace<PosPARTYRACE>()`（键早已定义、注册缺失） | 本功能 |
| @@ -262,6 +296,47 @@ | R28-B5 字体兜底（is_font_path_key/file_present/stale_font_path） | **并发**，不 stage |
| @@ -321,7 +396,8 @@ | load() 里挂 stale_font_path | **并发**，不 stage |
| @@ -481,7 +557,8 @@ | reload 路径挂 stale_font_path | **并发**，不 stage |

## Configuration.h
| hunk | 内容 | 归属 |
| --- | --- | --- |
| @@ -523,15 +523,18 @@ | font.path 默认改思源宋体 | **并发**，不 stage |
| @@ -823,11 +826,14 @@ | SysOptWindowMode 默认 false→true + 镜像注释 | 本功能 |
| @@ -838,10 +844,12 @@ | SysOptMobInfo 默认 0→3（soMobInfo=3） | 本功能 |

## IO/UITypes/UIStatusBar.cpp
| hunk | 内容 | 归属 |
| --- | --- | --- |
| @@ -47,6 +47,9 @@ | +`#include "../../Configuration.h"`（B4-5 阈值读取） | 本功能 |
| @@ -705,10 +708,10 @@ | 商城按钮走 EnterCashShopPacket | **并发**，不 stage |
| @@ -2352,19 +2355,43 @@ | update_classic 低血/低蓝才闪（HP/MP 警报消费端） | 本功能 |

## 整文件归属（工作树里该文件的改动 100% 本功能，直接 stage 整文件）
- IO/Window.cpp（鼠标 warp 回写删除 + 启动镜像 + 重锚定；poll 调用点传 xrel/yrel）
- IO/UITypes/UIOptionMenu.cpp（commit() 先取基准，修恒假比较）
- docs/features/ui-v83-window-restore/PLAN-R28.md（§6.3 结案增补）
- docs/features/ui-v83-window-restore/evidence/w8-sysopt-behavior/README.md（§2.4/§3/§4 结案增补）
- docs/features/ui-v83-window-restore/evidence/w8-sysopt-behavior/README-B4b-cursor-api.md（新）
- docs/features/ui-v83-window-restore/evidence/w8-sysopt-behavior/cursor_api_scan.py（新）

## 不入提交
- locale/*.ini、其余 30+ 个 modified 文件、全部 untracked（_out/、tools/_q_*、临时截图等）：并发会话/历史杂物。
- login.pass：有意不落盘（明文口令只走 xy-config 部署级）。
