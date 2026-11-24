# 世界地图搜索：输入内容落框 + 结果入灰框 + 消除重叠（2026-11-22 21:41 反馈）

> 状态：本端截图确认 + 行号级根因 + 全量构建 + 部署 live + 短启无闪退；真机观感待用户截图验收。
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` SHA256 `855e630de5aff9f069ca9a46a3494a7564b82267f3c66b27c1c1443981b53bcc`（5368320 B，23:32:33）

## 1. 反馈与证据
用户 21:41 截图 `XiaoYe_2026-11-22_21-41-02.png`：①输入内容"猪"显示在输入框下方而非框内；②结果列表位置偏左上、不在灰框内；③结果项文字重叠。
来源：截图 21:41 晚于当时 live（约 f9ec71fe 20:49，含我早期搜索改动但非最新）→ 本端证据。

## 2. 根因（行号级 + .nx 实测坐标）
backgrnd(147x537) 实测：输入圆角框 y≈24..64（中心≈44）、x≈8..100；结果灰框从 y≈68 起、x≈8..140。
1. **输入不在框内**：`UIWorldMap.cpp` 输入 bounds y=25 是基线，`set_text_shift(0,10)` 使墨迹顶≈25（槽顶），未落槽中心≈44。
2. **结果偏左上**：`RESULT_AREA_TOP=86` 相对灰框 y=68 太靠下/左，进口 x=10 也需对齐。
3. **结果重叠**：放大图量得图标行距=15px（`NATIVE_ROW_H=15`），但中文 A11M 文字实际行高≈13-14 且 `label.draw(rowpos+14,0)` 传的是基线而非墨迹顶 → 文字上下叠。

## 3. 修复（一次成型）
- `UIWorldMap.cpp`：`set_text_shift(0,25)` 让输入墨迹落到输入槽中心 y≈44。
- `UIWorldMap.h`：`NATIVE_ROW_H 15→18`（中文行距留余量）、`RESULT_AREA_TOP 86→70`（结果从灰框 y≈68 内起，往右上）。
- `UIWorldMap.cpp draw`：结果图标垂直居中于行 `(NATIVE_ROW_H - icon.h())/2`；文字用 `label.y_for_ink_top(rowpos.y+(NATIVE_ROW_H-label.height())/2)` 让墨迹在行内垂直居中 → 消除重叠。
- hit-test 矩形随 NATIVE_ROW_H 自动同步。

## 4. 构建/部署
- 并行会话持续锁 PDB，`/p:IntDir=<独立>\obj\ + /p:OutDir=<独立>\` 隔离 → `MSBUILD_RC=0`。
- 覆盖 live 后实时核对 SHA == 产物；短启 `_ab_launch.ps1 -Wait 12` → `ALIVE`。
- 真机观感（输入落框、结果入灰框、无重叠）待用户截图，须先核对 SHA `855e630d…`。

## 5. 关联
- 上轮：`docs/features/mapinfo-dropgrid-rebuild-2026-11-22/README.md`。
- 192 原生搜索轨：`docs/features/worldmap-192-search/README.md`。
- 回退点：`build-searchui-verify/MapleStory.exe`。
