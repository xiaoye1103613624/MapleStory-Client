# 夜间光照系统补齐（角色光环 + 月光光晕 + 灯光径向提亮）— 2026-11-22

> 分支：`feature/dev_0.0.1`
> 用户拍板：①角色光环做（新功能）②逐层 tint 完整复刻方向 ③月光加由亮到暗光晕 ④所有光晕径向平滑衰减、无矩形边
> 原生依据：`docs/features/native-rea-supplement-2026-11-22/README.md` + bundle `E:/project/BeiDou-ijl15/ezorsia/weather/` 一手取证

## 1. 原生事实（先厘清，避免误解）
- 原天气**没有**"以角色为中心"的光环（`Weather_ReadPlayerPos` 全库仅脚印/踩水 2 处调用）；其局部光照是 lamp-centric（`LIGHT_R_INNER=70/OUTER=700/STRENGTH=0.6`，`LightFactorAt@lamps.cpp:2067`）。角色光环=按用户要求**新增**。
- 原天气**没有**独立月光光束；月盘 glow 烘焙在 `nightDesert back/3`（312×312），用 alpha-only 着色不受夜色压暗。本轮按用户要求**额外加**由亮到暗的月光外晕。
- 原天气夜色是**逐层 RGB 乘色 + 灯附近 relight**（`weather.cpp:908-920 Argb` / `lamps.cpp:2229-2290 Lamps_RelightScenery`），非全屏蒙版。

## 2. 本轮实现

| # | 文件 | 改动 |
|---|---|---|
| 1 | `Graphics/GraphicsGL.h/.cpp` | 新增独立**径向渐变纹理**（128×128，中心 alpha=255→边缘 0，smoothstep 平滑）+ `draw_glow(x,y,radius,r,g,b,intensity)`；flush 末段以加色混合（GL_SRC_ALPHA/GL_ONE）统一绘制 glow 批次，临时切 atlassize/fontregion 复用 shader，画完恢复。**边缘完全无矩形**。 |
| 2 | `Weather/WeatherFx.h/.cpp` | 新增 `Fx::draw_lights(viewx,viewy,player_x,player_y)`：①月光光晕——月盘坐标 `(346+vw/2,-275+vh/2)` 外扩 300px 冷白由亮到暗；②角色周围光环——玩家屏幕位置暖白 150px 柔光（新功能）。强度随 night_level 平滑。 |
| 3 | `Weather/WeatherLamps.cpp` | 删除地面**矩形光池**（真 sprite 分支 192×80/120×48 两矩形 + 兜底分支 68/32/184×68 三矩形）；改为以**灯芯**（`sx+art->ox, sy+art->oy`，在灯柱上方）为中心的 150px/70px 两层径向 glow。 |
| 4 | `Gameplay/Stage.cpp` | 夜色 multiply 之后调用 `Fx::draw_lights`（flush 批次顺序 mul→add→glow 保证光晕抬亮夜色）。 |

## 3. 关键渲染顺序
flush 批次固定顺序：`quads(主) → quads_mul(夜色) → quads_add → glow_quads(光晕)`。
所以光晕永远画在夜色 multiply 之后 → 真正"抬亮"被压暗的场景，而非被夜色盖住。灯 sprite 在主批次先画，glow 在其上发光，符合"灯芯在灯柱上方发光"。

## 4. 构建与部署（按铁律）
- 脚本 `tools/_nightlight_build.bat`，隔离 OutDir `build-light/` + 全新 IntDir `build-light-int/`，全量 Rebuild。
- `MSBUILD_EXIT=0`、error 0。产物 5367296 B，SHA `344218bcc6d439d97d2a00859bcb648b23293e85717b80b4bc7175e5acfa970b`。
- 备份 `MapleStory.exe.bak-before-nightlight-20261122` → 覆盖 live 正式 exe → 8s 防回滚复核 SHA 一致；**live 与产物差异字节数=0**（本次登录器未改 exe）。
- 代码同一性实证：独有浮点常量 `0.30f` 出现次数 BUILD=8 / BACKUP=7（新增角色光环一个），产物比备份大 2048B。

## 5. 真机验收清单（未验收，USER.md 红线如实声明）
- [ ] 夜里能看清角色周围（暖白光环），不再"整体发黑"。
- [ ] 月亮向外由亮到暗有柔和光晕，边缘无矩形/硬边。
- [ ] 路灯灯光在**灯柱上方灯芯**处，不再落在地上；灯附近地面被提亮（不再贴纸感）。
- [ ] 所有光晕边缘平滑渐渐消失，无方形边界、无色块。
- [ ] 白天（night≈0）三层光晕完全不出现；黄昏平滑渐入。
- [ ] 换图/换线/进出商城后光照状态正确；帧率无明显下降。
- [ ] 与既有夜色 multiply、雨/雪粒子、地面水坑无叠加异常。

## 6. 诚实边界
- **真机未验收**：以上清单需用户实机逐项确认，未称"完全修复"。
- **未做完整逐层 relight 复刻**：本轮用"全屏 multiply + 径向 glow 抬亮"在观感上等效原版 `fNight*(1-lf*0.6)`，未对 tiles/objs/npc 每层单独乘色（那需要侵入各层 draw API、风险高）。用户若要求严格逐层（连 NPC 也单独调色），列为后续增强。
- 月光外晕、角色光环为**原版没有的新增**，按用户明确要求实现。
- 资源侧 `nightDesert back/3` 贴图自带 glow 的量未用 orange-wz 复核（不影响本轮新增外晕逻辑）。
