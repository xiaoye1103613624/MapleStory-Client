# HD 对接说明 · Daily Gift

## 本版策略

- **运行时**：Geometry `ColorBox` + `Text`（含 `digit_glyphs[0..9]` 拼数字）+ 可选 `Basic.img` 关闭钮  
- **docs**：`resources/` 下已生成 PNG 占位 / 简易 HD 切片，**不写回 UI.wz/nx**  
- 项目暂无通用「磁盘 PNG → Texture」UI 管道；换皮时二选一：  
  - A. 增加本地贴图加载后按本清单替换灰框  
  - B. 经 patch 工具写入 `UI*.img` 节点后再 `UiAssetResolve`

## 建议节点路径（未来写入 WZ/NX 时）

```text
UIWindow2.img/DailyGift/          # 或 UIWindow.img/DailyGift/
  backgrnd                        # window_frame
  banner                          # banner_green
  slot/normal|highlight|hover|pressed|claimed
  claimedMark
  progress/empty|fill|full|cap
  BtClaim/normal|mouseOver|pressed|disabled
  digit/0 .. digit/9              # 格子天数用
  digitDate/0 .. digitDate/9      # 顶栏日期用
```

关闭钮可继续复用：`UI/Basic.img/BtClose3`（或 BtClose2）。

## 数字替换步骤

1. 替换 `resources/digits/digit_N.png`（或导出到上述 `digit/N`）  
2. 客户端将 `digit_glyphs[i]` 从 Text 改为 Texture 数组  
3. `draw_digits` / `draw_day_label` / `draw_date_range` **逻辑不变**（仍按位拼）

## 格子道具

中部空槽对接后期：在 `draw_slot` 空槽坐标放置 `ItemData` 图标即可；**本版刻意不渲染**。

## 证据

- 官服参考：`evidence/official-daily-gift-reference.png`  
- 预览总览：`resources/preview/asset_sheet.png`  
- 数字条：`resources/preview/digits_0_9_strip.png`  
