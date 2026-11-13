# Wave4 · 多分辨率收口

## 已有链路

- 登录恒 1280×720；进图 `window.game_*`（`ResolutionTiers.h`）
- `UIStateGame` 广播 `update_screen`；QS 翻转重建 StatusBar
- `UIElement::makeactive` 对非 None 策略再跑 `update_screen`（开窗夹回）

## 验收要点

1. 默认 1280：底栏/快捷窗像素（S 按 V4 规则）
2. 切 800/1024/1366/1920：无重叠、整组居中或短 QS 右上、UI 不溢出
3. 退登录恢复 1280，选角链不漂

自动化：`tools/resolution/run_tier_matrix.py`（需服务端在线）。

## 构建 / 部署（2026-11-13）

- Live 指纹（skillbook 修复后）：`5164032` / `19:14:49`
- 幽灵 PID 6108/34476/56016：仍 Access denied（无窗，不挡矩阵）
- 进图矩阵：`0,1,5` → 3/3；`2,3,4,6,7,8` → 6/6；合计 **9/9 pass**
- 修复：`notes/setfield-skillbook-fix-2026-11-13.md`（masterlevel + 双帧 skill 段）
