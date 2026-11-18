# 商城深挖 · 第④项翻页条改造 真机闭环报告

日期：2026-11-17　方法：🟠阿里（定目标→追过程→拿结果）　目标：goal-82dc531a

## 1. 目标与背景
用户四项全选深挖商城扩展，第④项=翻页条改造（list_slider → MinMS PAGE_BTN 页码条）。
其前提「仅 10 项单页」已被源码证伪：`tools/_nx_commodity_stats.py` 实测 `Commodity.img` 的 `OnSale==1` 共 **2010** 行（见 `notes/cashshop-empty-catalog.md:69`），按 10 项/页 = **201 页**，翻页真机可验。

## 2. 源码改动（对照 MinMS）
`IO/UITypes/UICashShop.h` / `.cpp`，补丁脚本 `tools/_apply_pagination.py`（每处 assert 唯一，备份 `tools/_bak_UICashShop.{h,cpp}.before_pagination`）：

| 位置 | 改动 | MinMS 对照 |
| --- | --- | --- |
| `.h` 方法声明 | 新增 `list_page_count()` / `sync_page_chrome()` / `change_list_page(delta)` | :720-780 |
| `.h` 按钮枚举 | 新增 `BtPagePrev/BtPageNext`（从 `BtBuy+MAX_ITEMS` 起，`BtGift` 顺延，不破坏 `BtBuy+i` 槽位布局） | `BT_PAGE_PREV/NEXT` |
| `.h` 成员 | 新增 `Text page_label` | page_label |
| `.cpp` 构造 | 复用 `UiAssetResolve::window("MonsterBook")["arrowLeft"/"arrowRight"]` 建箭头按钮（本端 `UIMonsterBook.cpp:170-173` 已证资源可用）；`try_maple_btn` 辅助函数建钮 | :183-194（本端无 set_visual_scale，裸用） |
| `.cpp` slider 回调 | 改走统一 `change_list_page(upwards?-1:1)`，滑条与页码双入口共用 offset 路径 | — |
| `.cpp` draw | 页码 label 画在列表下方；`update()` 每帧 `sync_page_chrome()`（Text::change_text 内容不变即 no-op，无性能问题） | :741-759 |
| `.cpp` button_pressed | `BtPagePrev/BtPageNext` → `change_list_page(∓1)` | :760-780 |
| `.cpp` 算法 | `list_page_count=ceil(items/10)`；`sync` 设 label `cur+1/pages` 并按边界 disable 按钮；`change` clamp 后 `update_items()+sync` | :720-780 |

## 3. 编译 / 部署证据（防假通过门禁）
- 首次编译 `error C2664`：`make_unique<MapleButton>(node, Point, "tag")` 参数类型错——MapleButton 直构只收 (node,x,y)，Point+tag 是 `try_maple_btn` 签名。改用 `try_maple_btn`（日志 build_uifour.log:618）。
- 重编：`BUILD_EXITCODE=0`、`errorC=0`；`obj mtime 14:00:34 > cpp mtime 13:59:53`；exe SHA `3f609f70…`（相对旧产物 902c38b7 已变）。**源码真正入链**。
- 部署：`build-pua/MapleStory.exe` → `E:/MXD/BeiDou-Client-xy/MapleStory_pua25.exe`，SHA 一致 `3f609f70…`。

## 4. 真机验证（脚本 `tools/_page_launch.py` + `tools/_page_click_verify.py`）
环境坑：canon（8b03165b）被旧进程占用无法覆盖；直接启 pua25 因 admin 单点登录卡在"请再确认"框（标题无 Admin）→ `_page_launch.py` 一次性优雅退出旧实例（Alt+F4→RETURN 走 UIQuitConfirm）+ 强制清残留 + 启 pua25 + 自动点确认框 + 抢到 Admin + 进商城。DPI：GetClientRect=1280×720 逻辑、ClientToScreen origin=(640,424)、ImageGrab 按 client 区抓。视觉后端全挂，全程 PIL 像素。

| 检查项 | 结果 | 证据 |
| --- | --- | --- |
| 进商城 | 视口 1280×720→800×600，白底 16.6% | evidence/page_shop_full.png |
| 页码条渲染 | 放大图清晰显示 `1/201` 白字 + 灰左箭头 + 橙右箭头 | evidence/page_bar_zoom.png |
| 点 NEXT (client 661,503) | 页码 `1/201`→`2/201`，商品 hash 3458308→3391876（变化） | evidence/page_p2_after_next.png / page_p2_after_next_bar.png |
| 点 PREV (client 631,503) | 页码 `2/201`→`1/201`，商品 hash 3391876→3458308（复原） | evidence/page_p1_after_prev.png |
| 页数据规模 | 201 页 = ceil(2010/10)，与目录实测吻合 | notes/cashshop-empty-catalog.md |

**验收结论：第④项翻页条改造全链路闭环——编码、编译（errorC=0/obj 新/SHA 变）、部署、真机点击（页码与商品双变、可逆）全部通过。**

## 5. 复盘四步法
1. **回顾目标**：用户要四项深挖真机验证，本项=翻页条。
2. **评估结果**：完成且超预期——页码条 + 滑条双入口统一 offset，201 页真实可用。
3. **分析原因**：唯一弯路是 MapleButton 构造 API 用错（照抄 try_maple_btn 意图却用了直构），一次编译即暴露、换辅助函数即过。
4. **沉淀规律**：本端资源复用优先查 `UiAssetResolve::window(...)` 与已有 UI（如 UIMonsterBook）先例；按钮统一走 `try_maple_btn` 而非 `make_unique<MapleButton>`；翻页类 offset 改动必须保留 `BtBuy+i` 槽位枚举不变（BtGift 顺延）避免连带破坏购买/礼物回调。

## 6. 剩余
第①项宠物+特效试穿、第②项 CSInventory/CSLocker 格子、第③项购买回执清购物车仍待编码/验证（goal-82dc531a 保持 active）。
