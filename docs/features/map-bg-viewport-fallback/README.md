# 地图背景高分分级代码兜底

> 状态：**已实现**（2026-11-24）  
> 范围：全局 `MapBackgrounds` backgrounds / foregrounds；不改 WZ、无白名单开关。

## 1. 决策摘要

| # | 拍板 |
| --- | --- |
| 1 | 分级代码兜底（不平铺一切 type0） |
| 2 | type0 缺口用半透明淡出复制（非底边 clamp 拉伸） |
| 3 | 全局（室内大视口同样生效） |
| 4 | 不预留 map 白名单 / 关闭开关 |

关联：[`../weather-night-flicker-fix-2026-11-21/README.md`](../weather-night-flicker-fix-2026-11-21/README.md) §4「底部蓝带」——根因同为 type0 不随视口延展；本功能以代码兜底闭合该视觉缺口（不改资源）。

## 2. 问题

多分辨率下视口大于 800×600 设计幅面时：

- `type` 1–7（平铺/滚动）已由 `refresh_viewport` 的 `ceil(视口/cx)+3` 动态平铺覆盖；
- `type=0`（NORMAL）`htile/vtile` 恒 1，单张远中景盖不住 → 露出天空底色或室内黑底（蓝带/露底）。

远近层级仍由每层独立 `rx/ry` 视差驱动，本功能不改视差公式。

## 3. 实现逻辑

文件：[`Gameplay/MapleMap/MapBackgrounds.cpp`](../../../Gameplay/MapleMap/MapBackgrounds.cpp) / [`.h`](../../../Gameplay/MapleMap/MapBackgrounds.h)

```text
Background::draw
  → refresh_viewport（读当前 Constants 视口）
  → 算视差坐标
  → type==NORMAL → draw_normal_with_fade
  → 其它 type → 原有双重平铺循环
```

`draw_normal_with_fade`：

1. 用 `DrawArgument::get_rectangle(origin, dim)` 得屏幕 AABB（与 `Texture` 翻转语义一致）。
2. **水平**：本体为基线；若未盖满 `[0, VWIDTH)`，向左右按 `dim_x` 步进复制，奇数级镜像翻转，`opc *= 0.55^k`，单侧最多 3 级；`opc < 0.05` 停。
3. **竖直**：对每个水平基线实例，未盖满 `[0, VHEIGHT)` 时上下按 `dim_y` 步进淡出复制，不额外镜像，单侧最多 4 级。
4. 画布尺寸 ≤0 时只画本体，避免步进 0。

常量（匿名命名空间）：

| 常量 | 值 |
| --- | --- |
| `kBgFadeRatio` | `0.55f` |
| `kBgMaxHCopies` | `3` |
| `kBgMaxVCopies` | `4` |
| `kBgMinOpc` | `0.05f` |

**刻意不做**：WZ 补资源、Camera VR 收紧、`WeatherBackLayers`（月亮等 type0 天体不应被延成一串）。

铺底色（`drawscreenfill` 黑/天空蓝）保留；淡出透明处继续吃底色。

## 4. 验收要点

- [x] 构建 Release x64 通过（`tools/build_smoke.bat` `EXITCODE=0`）
- [ ] `800×600` / `1280×720` 户外：无明显多余重影（已盖满则只画本体）— 待真机
- [ ] `1920×1080`（及更宽）户外：山脚/天空交界无大块硬缝露底，可见淡出过渡 — 待真机
- [ ] 高分室内：墙/装饰 type0 缺口淡出，非整墙双胞胎平铺 — 待真机
- [ ] 夜间 multiply：淡出层被统一压暗 — 待真机
- [ ] 游戏内切档：次帧覆盖正确 — 待真机

## 5. 回滚

还原 `MapBackgrounds.cpp` / `MapBackgrounds.h` 即可；零协议、零资源、零配置。
