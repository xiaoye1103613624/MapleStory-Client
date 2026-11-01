# r201 — 登录链叠图验收 playbook

SoftKey tip→out 已饱和（r198）；PersonalShop CREATE 已接线（r200）。本笔记固定登录叠图 knobs，待当前端截图后调。

## 对照素材

`docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/`

| 截图 | 对应 UI |
| --- | --- |
| 登录界面.png | LoginBackground step0 + UILogin |
| 选择频道页.png / 选中频道页.png | step1 + UIWorldSelect |
| 选择角色页.png | UICharSelect |
| 进入游戏页.png | UIStatusBar（CreateCtrl 已锁） |

## 优先 knobs（勿盲改）

1. `LoginBackground.cpp` step0 `{137,-20}` / step1+ `{28, step_height}`
2. `UILogin.cpp` `signboard_map_pos(137,21)` + `TITLE_WINDOW_OFFSET(58,16)`
3. `UIWorldSelect` pole/scroll map + `CH_SLIDE_DIST`（侧栏 r20 已锁）
4. `UICharSelect` `CS_CHARINFO_OFF(-93,-208)`（`CS_DETAIL_*` 勿动）
5. `UILoginWait` bar `(99,97)`（勿回退 46,98）

## 验收步骤

1. 当前端同分辨率截图（建议 1280×720）放入 `evidence/r20/current_shots/`
2. 与 correct_shots 半透明叠图；仅偏差项改对应 knob
3. `tools/build_smoke.bat` → 部署 `E:\MXD\BeiDou-Client-xy`

产物：本 playbook（无偏移代码变更）
