# round74 · UILogin/CUITitle 标题窗按 Angel 083 OnCreate 立即数复核

## 0. 授权与验收
- User said (m11704): "记录一次有可见 PUA 输出的交付，不记录评分。"
- m11705 `<goal_round>` Round 74/90；用户 m09859 硬指令「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复」。
- 门禁 18：REA→IDA→orange-wz(IMG) 只读取证；截图仅备选。本轮 IDA 主路径 + runtime open_merge 探针，0 行为字面量改动。

## 1. 选面
- 全仓「截图 SAD 自造版式」扫描后选登录链路起点 `IO/UITypes/UILogin.cpp:151-157/163/235`：注释自称「截图反推 (58,16) + 按原版 CUITitle::OnCreate 偏移 (67,12)/(67,40) 132x15/120x15 + BeiDou 缺输入框贴图兜底」。

## 2. 083 活 IDB（禁凭记忆）
- `entity_query *CUITitle*` 5 真符号：`OnCreate 0x62054a size 0x75b` / `OnButtonClicked 0x620ca5` / `SetRet 0x620e3c` / `IsRequestValid 0x621156` / `EnableLoginCtrl 0x621175`；`CCtrlEdit::OnCreate 0x4ca25d`。
- `CUITitle_OnCreate.dec.txt` 350 行，7 按钮 (1000..1006) 与 2 CCtrlEdit (1007/1008) 立即数逐字见 evidence JSON。**OnCreate 无窗口屏幕原点立即数**。

## 3. 对照清单（决定性：反例）
| 维度 | 083 立即数 | 当前端 | 判定 |
| --- | --- | --- | --- |
| 7 按钮坐标 | (213,-5)/(37,74)/(144,73)/(226,74)/(14,122)/(114,120)/(214,120) | `UILogin.cpp:133-181` 全同 | ✅ 逐字命中 |
| 账号框 | (67,12) 132x15 @0x6209c0 | `account_pos(67,12)`+兜底 132x15 | ✅ |
| 密码框 | (67,40) 120x15 @0x620a2b | `password_pos(67,40)`+兜底 120x15 | ✅ |
| 输入上限 | v75=12 | `TEXTFIELD_LIMIT=12` | ✅ |
| 窗原点 (58,16) | **无** | 截图 SAD 派生，注释已自认 | ⚠️ 非 083 数字、不冒充 |
| bedou 兜底 | 083 CCtrlEdit 宽高 | `Login.img/Title` 缺贴图属实 | ✅ 真缺口真尺寸 |

→ **这一面是按 Angel 083 立即数实现的反例**（继 round73 之后第二例），不是第 8 例同族病。用户 m09859 授权「不是按 Angel 修的才重修」，本面无需重修行为。

## 4. orange-wz 门禁 + .nx + 运行时探针
- orange-wz：11 节点全部 `IMG 解析失败: Login.img`（65536B GMS key `268a8407a47ee76c` / `TSPHKw==`），结构层 WORKS、内容层失败 → 合规 `.nx` 回退，原文留证。
- `.nx`（09-21 旧缓存）：`Login.img/Title` 13 节点、`MSTitle 397x219`、`mapleID/PW/backgrnd` 缺、`Title_new` NOT FOUND、`Common/backgrnd` 缺 → `bedou=true` 属实。
- round71 教训（旧 .nx 的 SysOpt 299x365 vs 运行时 299x396）→ 本轮补 `_tools/lt_probe/lt_probe.exe` 真 open_merge 探针：`OPEN_MERGE_OK`，运行时与 .nx 完全一致、`Title_new_valid=0`、`Common/frame 1280x720`。**不拿旧快照当运行时权威**。

## 5. 决策点 D-E（交用户裁决，本轮零实施）
- 083 `CUITitle::OnButtonClicked @0x620ca5` 的 1002..1006 走 5 个 StringPool `SP_*` 外链常量；当前端 `open_url()` 用 4 个 `Configuration` URL + `SDL_OpenURL`。属 s9/配置适配，非布局缺陷；校准≠改回原生，不照搬 083 SP_ 常量。

## 6. 零风险补丁
- 7 块纯注释、0 行为字面量：`UILogin.cpp` 5 处（补 083 EA 依据 / 明确 (58,16) 非 083 数字 / bedou 真节点双证 / D-E 口径）、`UILogin.h` 2 处（bedou + 兜底尺寸依据）。
- `git diff HEAD` 校验两源码增删行全是注释行。

## 7. 验证
- harness `tools/test_round74_logintitle_083.py`：静态守卫（083 EA 落注释 / 反例定性 / 0 行为行 / LoginBackground 线性插值保留 / 四文档）+ 场景（7 按钮坐标模型 / 2 输入框 / limit=12 / bedou 真节点 / D-E 4URL vs 5 SP_）。
- `GATE OK (s9 a78ba681a4)`；`tools\build_smoke.bat` `EXITCODE=0` + 覆盖 `UILogin.cpp` + `ERROR_COUNT 0`。

## 8. 提交 scope
- 精确 `git add` 7 文件：2 源码 + harness + evidence JSON + notes + dev-plan README + PLT README。一次性脚本 / IDA 反编译 / 运行时探针 stdout 仅本地证据链留存，不进版本库。零署名原子提交。

## 9. 教训
1. 别为了凑「第 N 例同族病」把按 083 立即数实现的窗硬判成自造——反例也是闭环。
2. 截图 SAD 派生的量（58,16）可以保留，但必须标「083 反编译未给出、不作 083 背书」。
3. 旧 .nx 缓存与运行时可不同（round71 已翻案），结论前必须 open_merge 探针复核。
4. 决策点（D-E）与布局缺陷分开定性，协议/配置适配不擅自「改回原生」。
