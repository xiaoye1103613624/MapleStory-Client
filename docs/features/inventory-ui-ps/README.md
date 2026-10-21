# 冒险岛展开背包 UI（Photoshop 复刻 + 定制）

## 交付物

| 文件 | 说明 |
| --- | --- |
| `MapleStory_Inventory_Full_Extended.psd` | 分层 PSD（header / slots×2 / footer / tabs / 关闭） |
| `MapleStory_Inventory_Full_Extended.png` | 整图导出 |
| `source/` | 自 `UI.wz` 导出的原版切片 |
| `layers/` | 合成用中间图层 PNG |
| `compose_meta.txt` | 尺寸与裁切参数 |

## 资源来源

- WZ：`E:\project\sdlMS\build\Data2\UI.wz`（GMS）
- 节点：`UIWindow.img/Item/FullBackgrnd`（原 603×289，去右缘 1px 灰边后 601 宽）
- 导出工具：`_tools/DumpItemUi`（MapleLib）

## 定制

1. **格子纵向 ×2**：格区 `y=[45,257)` 高度 212px，向下再贴一份 → 12 行
2. **金币栏加宽 +160px**：白框 `118→278`（`x=24..302`），**内部不放数字**；`mesos` 标签右移
3. **窗体宽度不变**（避免右侧拉伸产生竖条叠影）；去掉 WZ 导出多余灰边；PSD 不再叠 BtCashshop

## 最终尺寸

**601 × 501** px，72 DPI，RGB

## 重做

```bat
dotnet run --project _tools\DumpItemUi -c Release -- E:\project\sdlMS\build\Data2\UI.wz docs\features\inventory-ui-ps\source
python _tools\compose_inventory_ui.py
```

## 范围外

未改客户端逻辑、未写回 `UI.wz`。
