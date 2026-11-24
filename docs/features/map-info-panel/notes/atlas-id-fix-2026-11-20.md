# MapInfo atlas-id 修复（2026-11-20）

## 真因

`UIMapInfoPanel` 全部 `LocalPngTexture::load` 共用同一 `ATLAS_ID`。  
`GraphicsGL::ensure_raw` 对同 id **只上传首次** → chrome 抢占后，页签/格子/行底纹 GPU 仍指向底板裁片。

## 修复

- `ATLAS_BASE` + 每张 PNG `atlas++`（对齐 `UIMapSearchPanel`）
- 怪物行：灰底内居中图标 + 文案 Y 对齐 78px 行
- Tab 激活态 `PRESSED`；掉落图标缩至 ≤30；爆率黄字 `A11B`
- 底栏只画怪物金币区间（无数据 `-`），停画重复 `mesos` 标签
- 运行时同步 v2 `slot` / Tab / `mob_row_bg`

## PS 重绘

本轮 atlas + v2 资源同步后即可验收；**未**触发 Photoshop MCP 重绘。若真机仍缺格栅线/槽位观感，再按 `ui-photoshop.md` 开 `v3`。

## 2026-11-20 二次校准（真机截图）

- 底板禁止随世界地图框高拉伸（`panel_height=HEIGHT`）→ 消除掉落格溢出
- 格点对齐烤格：`SLOT=30 GAP=4 GRID=(31,313)`；Tab `Y=286` 贴蓝灰条
- 怪物文案整组上移一行；爆率改黑色；meso 画在底栏白框
- 搜索结果原点改为白名单区 `(16,52)`，行高 16
