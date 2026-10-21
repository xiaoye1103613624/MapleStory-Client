# 神秘徽章装备栏 UI 资源

画布尺寸：`187 x 340`（与参考图一致）

## 文件清单

| 文件 | 说明 |
|------|------|
| `panel_base.png` | 完整面板：标题 + 深蓝主区（无「200级可解锁」）+ 6 空槽（无锁） |
| `slot_empty.png` | 单个空槽（浅灰纹理） |
| `lock.png` | 透明底锁图标 |
| `layer_title_bar.png` | 标题栏图层（全画布透明底） |
| `layer_main_slot.png` | 深蓝主区图层 |
| `layer_grid_bg.png` | 浅蓝网格底板 + 四角 rivet |
| `mystic_badge_panel.psd` | 分层 PSD（若生成成功） |

## 合成方式（自下而上）

1. `layer_frame`（外框，含在 `panel_base` 内；PSD 中为 `frame`）
2. `layer_title_bar` — 标题「神秘徽章装备栏」
3. `layer_main_slot` — 深蓝展示区（无解锁文字）
4. `layer_grid_bg` — 浅蓝装备区背景
5. `layer_slots` / 6× `slot_empty` 贴到槽位坐标
6. （可选）`lock.png` 居中叠到需要锁定的槽上

### 槽位坐标（左上角，相对画布）

- 槽 0: (18, 149) 尺寸 48x82
- 槽 1: (68, 149) 尺寸 47x82
- 槽 2: (118, 149) 尺寸 48x82
- 槽 3: (18, 239) 尺寸 48x82
- 槽 4: (68, 239) 尺寸 47x82
- 槽 5: (118, 239) 尺寸 48x82

## 与参考图差异

- 已去除「200级可解锁」文字
- 6 个槽默认空槽，不绘制锁
- 锁单独导出为 `lock.png`
- 角部装饰为简化卷草纹（非位图抠取）

## PSD

- 状态：成功
- 说明：ok

## 生成

```bash
python assets/ui/mystic_badge/generate_mystic_badge_ui.py
```
