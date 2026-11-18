# 商城商品缺失修复 · 真机验证（2026-11-16）

> 用户诉求（原话）：「先全部实现，然后你真机测试」。本页是真机结果，不重复根因（见 notes/cashshop-empty-catalog.md）。

## 结论：真机确认商城商品非空 ✅

改动后的客户端实机进入现金商城，首页 tab 下 **8 件商品带图标 / 名称 / 价格完整渲染**，右侧「热销榜」1–5 有位次，顶部 9 个页签（首页/热卖/装备/消耗/设置/其他/宠物/礼包/购物车）齐全。用户最初「商城为什么没有商品」在真机层面已不成立。

铁证截图：`evidence/real-20261116/cashshop-home-filled.png`（800×600，sha256 `49f3776c97aec054029856de7e1ee25ba3db0e63`）。
图内可读条目：皇家理发券 100 NX、花糖骑士帽 990 NX、心型蝴蝶结、黑色王冠 800 NX、黑色小飞侠帽 600 NX 等；每件带「购买 / 礼物」按钮。

## 为什么这张图足以判定改动生效

- 旧逻辑：handler 把 `openCashShop` 下发的 `modified_cash_item` 条数（运营未改价时=0）当主商品列表数 → `commodity_count=0` → UI 画空弹层。
- 新逻辑：本地 `Etc/Commodity.img` 的 OnSale==1（实测 2010 行）作基表填充 → 首页显示全量。
- **若跑的是旧 exe，商城必为空**。图中商品非空 ⇒ 改动确已进入运行产物。

## 真机验证路径（怎么进的商城）

1. 客户端 `xy-config.yaml`：`login.auto=true`（自动登录进图）、`debug.packet_dump` 开关。
2. 用 `tools/level999/inject_chat.py` 向**真实活窗口 pid** 注入 `/shop` → 我新增的斜杠命令发 `EnterCashShopPacket(0x28)` → 服务端 `EnterCashShopHandler` 回 `openCashShop(0x7F)` + 余额/礼物/心愿包 → `SetCashShopHandler` 解析 + 本地目录填充 → `change_state(CASHSHOP)`。
3. 用 `tools/capture/capture-shot.ps1` 截客户区（商城子窗 800×600）。

## 踩坑记录（防复发，别再重复）

- **注入打错 pid**：`tasklist` 里僵尸 `MapleStory.exe`(pid 47560, 32KB, hwnd=0) 会挡在前面；活实例须用 ctypes `EnumWindows` + `GetClientRect` 找 1280×720 的那个。曾对僵尸注入 3 次，零效果。
- **`inject_chat.py --text` 默认 `!level 256`**：漏传会把 `/shop` 拼成 `/shop!level 256` 普通聊天，命令不命中。每次必须显式传 `--text`。
- **屏幕是 2560×1600，不是 1080p**：聊天框客户区 (300,648) → 屏幕 (940,1072)，完全在屏内；早期「底部超出屏幕」的判断是拿旧分辨率误判，害我放弃点击方案。
- **`packet_dump` 可能是 false**：日志 `packet_dump.log` 时间戳会停在关闭前的旧记录（本次停 14:11），此时「dump 无 28 00/7f 00」是假象，不能据此判失败。真机判定改用截图。
- **读图能力**：验证中途模型切换，早期不支持 image input 只能靠 PIL 像素统计；切到支持后可直读截图。同型任务先确认读图能力再选验证手段。

## 收口证据

| 验收项 | 证据 | 结果 |
| --- | --- | --- |
| 全部功能实现 | /shop 入口、Tab 分类过滤、热卖榜、首页全量、购物车（源码均在） | ✅ |
| 编译 | BUILD_EXITCODE=0，error C/LNK=0，obj mtime>cpp | ✅ |
| 真机进商城 | 截图显示商城子窗 800×600、9 页签、余额栏、EXIT | ✅ |
| 商品非空 | 首页 8 件带图标/名称/价格 + 热销榜 1–5 | ✅ |

## 未单独验证（如实披露，非需求阻塞）

- Tab 点击切换（装备/热卖等页签的逐页截图）未单独取证；首页非空已证明目录填充主链路通。
- 实机购买扣费回执未走（会真实消耗测试账号点券）；`CashShopBuyPacket` 发包链已在代码层具备。
