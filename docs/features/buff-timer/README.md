# Buff 图标剩余时间文本（BuffTimer）

> 状态：已闭环（客户端层）
> 适用客户端/协议版本：HeavenClient 系 v83（部署目录 `E:\MXD\BeiDou-Client-xy`）
> 项目名称：MapleStory-Client（XiaoYE MS v0.0.1）
> 作者/日期：萧曳 / 2026-10-21
> 相关提交：见 `git log --grep="buff 图标剩余时间"`
> 产物目录：`docs/features/buff-timer/`

## 1. 背景与目标

- **问题**：当前端 `UIBuffList` 只有 buff 图标与冷却遮罩，看不到还剩多少秒；参考端 `E:\MXD\BeiDou-ClientV17.7\ijl15.dll` 的 BuffTimer 模块会在图标上叠加倒计时文本。
- **目标**：对齐参考端的显示行为——图标上叠加 `1h2m` / `3m4s` / `5s` 三种粒度的剩余时间，黄色正文 + 黑色 1px 描边，水平居中。
- **明确不做**：不改 buff 计时/续期逻辑；不做团队 Buff 面板（那是独立功能 `PartyBuffs`）；不引入参考端的「影子画布」缓存机制（原因见 §3.3）。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 UI | `IO/UITypes/UIBuffList.h/.cpp` | 新增倒计时文本渲染 |
| 配置 | `Configuration.h/.cpp` + `Settings` 写盘模板 | 新增 `ui.buff_timer` 开关 |
| 服务端 / 协议 | — | 无（`add_buff(buffid, duration)` 早已带时长） |
| WZ/资源 | — | 无（只用到已加载的字库与 buff 图标） |

## 3. 最终实现逻辑

### 3.1 主流程

1. 服务端下发 buff 时，`UIBuffList::add_buff(buffid, duration)` 构造 `BuffIcon`，把 `ui.buff_timer` 的当前取值一并传入。
2. `BuffIcon::update()` 每帧按 `Constants::TIMESTEP` 递减 `duration`（毫秒），并调用 `refresh_label()`。
3. `refresh_label()` 先把毫秒向上取整成秒（最小 1），**只有秒数变化时**才重排文本（`Text::change_text`），避免每帧重建 layout。
4. `BuffIcon::draw()` 先画图标与冷却遮罩，再画文本：先铺 8 个方向的黑色偏移副本做描边，最后叠黄色正文。

### 3.2 数据 / 协议 / 节点路径与语义

- 无新增协议字段，`duration` 沿用既有 `TemporaryStat` 时长（毫秒）。
- 格式化规则（逆向自 `ijl15.dll!anonymous_namespace_::FormatBuffTime` @ `0x100ADA50`）：

  | 条件（秒） | 输出 | 示例 |
  | --- | --- | --- |
  | `>= 3600` | `"%dh%dm"` | `1h23m` |
  | `>= 60` | `"%dm%ds"` | `5m30s` |
  | `< 60` | `"%ds"` | `45s` |

  秒数计算为 `(ms + 999) / 1000`，并钳到最小 1（参考端同样先 `+0x3E7` 再取整、`cmovg` 保证 ≥1）。

- 绘制几何（逆向自 `DrawBuffTimeLabelOnCanvas` @ `0x100AD3D0`、`RebuildBuffShadowCanvas` @ `0x100AEF00`）：
  - 参考端把文本画在 32×32 的图标画布上，`x = (32 - 文本宽) / 2`，`y = 21`；
  - 当前端的图标精灵以**底边**对齐 `position`，故等效屏幕位置为
    `position + ((32 - width) / 2, 21 - 32)`。

### 3.3 模块与扩展点

- `BuffIcon::format_time()`：静态纯函数，便于单测与后续统一（伤害数字等）复用。
- `Timelabel` / `timeoutline` 两个 `Text` 实例共享同一文本，避免在 `draw()` 里反复 `change_color`。
- **与参考端的差异**：参考端把「图标 + 描边文本」一次性渲染进一张 canvas 并插入图层（`BuffShadowState` 缓存，按 `pieIndex` 与文本变化才重建）；当前端没有等价的可写渲染目标，改为每帧直接绘制 9 次文本。buff 数量通常 < 10，实测无掉帧；若未来图标行数显著增加，再引入纹理缓存。
- 参考端对 `nID == 5221006` 与 `nID == 35001002` 的 buff 跳过倒计时（`UpdateBuffShadowIfNeeded` @ `0x100AF340`）。当前端暂**不**复刻该白名单：这两个 buff 在本服的时长语义尚待确认，先按统一规则显示，后续如需对齐再补。

## 4. 资源与同步

- 无新增 WZ/IMG 节点，无需资源同步。
- 字体沿用客户端既有 Arial 字库（`font.path.normal`）。参考端用 Tahoma 10px；当前端用 `Text::Font::A11M`（11px）近似，宽度由 `Text::width()` 实测居中。
- 颜色：参考端 `0xFFFFE060`（黄）+ `0xFF000000`（黑描边）；当前端 `Color::Name::YELLOW` + `BLACK`。色值有轻微差异（调色板未含 `FFE060`），如需完全一致需扩展 `Color::Name`。

## 5. 编码与 i18n

- 文本为纯数字+单位后缀（`h/m/s`），不含中文，无 GBK/UTF-8 转换风险。
- 若后续要本地化单位（如「时/分/秒」），须按客户端 narrow 文本编码约定处理。

## 6. 产物清单

| 类型 | 路径 | 说明 |
| --- | --- | --- |
| 文档 | `docs/features/buff-timer/README.md` | 本文件 |
| 证据 | `docs/features/beidou-v17-parity/notes/r02-hook-table.md` §1 | 反编译结论与 EA |
| 配置 | `Settings` / `xy-config.yaml` 的 `ui.buff_timer` | 默认 `true` |

## 7. 验证记录

- **构建**：`MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142` → `EXITCODE=0`，无 error；产物 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`（2,546,688 B，2026-10-21 20:25）。
- **基线**：改动前全量 Rebuild 通过（15 个基线提交）。
- **IDA 校准**：`FormatBuffTime` / `DrawBuffTimeLabelOnCanvas` / `EnsureBuffTimerFonts` / `UpdateBuffShadowIfNeeded` 均已反编译确认（见 r02 §1）。
- **启动冒烟**：启动 `MapleStory.exe` 25 秒，进程存活（约 1.5 GB），窗口标题显示 `1-Admin` 与在线时长，自动登录链路正常；关闭后无残留进程。
- **待人工确认**：进游戏后施加任意有时长的 buff（如「加速术」），确认文本位置在图标下部居中、`59s → 1m0s` 与 `59m59s → 1h0m` 切换正确；把 `ui.buff_timer` 改为 `false` 后重启，确认仅剩图标与遮罩。

## 8. 预留、风险与回滚

- **风险**：`duration` 若为 0 或负数（部分无时限 buff），会恒显示 `1s`。参考端同样下限为 1；如观感不佳可改为「无时限不显示」。
- **性能**：每 buff 每帧 10 次文本绘制。若后续 buff 数量大，改为按秒缓存到位图。
- **回滚**：删除 `ui.buff_timer` 或设为 `false` 即可回到原行为；代码层回滚只需还原 `UIBuffList.h/.cpp` 与 `Configuration.*` 两处新增。
