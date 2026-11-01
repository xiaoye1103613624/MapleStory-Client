# r19 · W0 基础 UI 实现记录（登录链 / 频道 / 选角 / 底栏）

> 日期：2026-10-22  
> 依据：[`r15-baseline-ui-gap-vs-screenshots.md`](r15-baseline-ui-gap-vs-screenshots.md)、[`r18-wave0-rea-incremental.md`](r18-wave0-rea-incremental.md) §7  
> 目标：对齐 `BeiDou.exe` 原生主链 UI/交互（含正确端截图观感）；**不含** ijl15 扩展。

---

## 已改

| 项 | 文件 | 对照证据 | 改动 |
| --- | --- | --- | --- |
| 世界过滤 | `IO/UITypes/UIWorldSelect.cpp` | REA `sub_624E13` 无 `<2` 过滤 | `channel_count < 2` → `< 1`；点世界时校验按钮已创建 |
| 选角排行 | `Net/Login.h`、`LoginParser.cpp`、`UICharSelect.*` | REA `sub_602ADC` 字串 2976 @(36,99) | `StatsEntry::has_rank`；无排行绘「排行不可用」，有排行绘名次 |
| 底栏快捷格点击 | `IO/UITypes/UIStatusBar.*` | r15/r18：格只画不点；命中区须包住 QS | `send_cursor` 槽命中 → `Stage::send_key`；命中区高 80、上沿 -9 |
| 清理 | `UIStatusBar.cpp` | 调试残留 | 删除 `[DBG-CLK]` 写盘日志 |

## UI / 动画一致性说明

- **频道**：保留两步确认（点频道选中 → 再点同频道或「到选择的世界去」进世界）与 `channel_slide`；与正确截图「选中圈 + Go 钮」一致。参考端 OnButton 点频道即发包属另一路径，本版不改为单点进世界，避免破坏已有滑入动画。
- **选角 charInfo**：坐标已与 `sub_602ADC` / 正确截图对齐；本轮补齐排行带文案。
- **底栏**：E/I/S/K 键按钮此前已接线；本轮补 **快捷格 8 槽点击放技能/用道具**。折叠动画未改。
- **登录背景 / Loading**：路径与防双框已在既有代码；本轮未改偏移（需实机与 `正确客户端截图` 叠图后再调 `LoginBackground::target_offset` / `kBeiDouBarOff`）。

## 验收建议

1. 登录 → 世界牌齐全 → 点世界展开 20 频道 → 选 CH → Go / 再点进入选角。  
2. 选中角色：charInfo 四维正确，排行区显示「排行不可用」或名次。  
3. 进图：点底栏 Equip/Inven/Stat/Skill；点快捷格释放已绑定技能/道具。  
4. 对照桌面 `正确客户端截图` 叠图登录/频道背景，有偏差再开微调提交。

## 未做（显式预留）

- opcode 5 vs 6（CharlistRequest）与 exe `6EC9CE(6)` 差异：保持现网 opcode 5 + s9。  
- 登录/Loading 像素级偏移微调（待实机叠图）。  
- Wave A 社交内容窗。
