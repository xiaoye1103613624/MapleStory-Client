# 选角卷轴展开"格子裸露"取证与分析（2026-11-16）

## 现象（用户视频）

视频：`C:/Users/11036/AppData/Local/PixPin/Temp/XiaoYe_2026-11-16_22-27-41.mp4`（4.68s / 281帧 / 816×462 / 60fps）。

用户描述：选择角色时属性 UI 短暂没有羊皮纸背景，只有属性值和属性名，"像多出很多一格一格的画面"。抽帧联系表存 `tools/_charselect_out/video_contact.png`、`video_scroll_zoom.png`（临时产物）。

量化：卷轴区域米黄像素数在 f93~f125（约 1.55~2.08s，持续 ~0.53s）为 2055→3065，f126+ 恢复到完整 5700。即展开动画期间约 0.5 秒只有属性格子浮在蓝天/森林背景上。

## 根因

**非资源加载失败**：`charInfo1` 是整张纹理，若未加载会整块消失，不会只显示属性格子。

真实原因：卷轴展开动画时序。`CharSelect/scroll/0` 首帧 frame0（高 30，delay 250ms）只是卷起的木杆、无纸面；但代码从第一帧就把完整内容板（charInfo1 底板贴图 + 所有属性文字）绘制出来，纸面尚未展开 → 属性格子裸露在背景上。切换选中角色时 `update_selected_character()` 置 `charinfo_scroll_playing=true` 重播动画，正是触发时机。

## IMG 帧数据（orange-wz 实时读取，权威）

根：`E:\MXD\BeiDou-Client-xy\Data\UI\Login.img`；key = 128B DEFAULT_KEY（`tools/_gms_key_128.b64`），IV `TSPHKw==`，`exclusive:false`，会话末 `unload_all`。详见姊妹文档：
`docs/features/charselect-detail-scroll-rank-align/evidence/img-nodes-2026-11-16.md`。

`CharSelect/scroll/0` 四帧（origin 全 (0,0)，delay 250/50/50/无）：

| 帧 | 高 | 木杆 | 米黄纸面（帧内 y） | 说明 |
|---|---|---|---|---|
| frame0 | 30 | y1~13 | 仅零星 y2~12/y18 | 卷起态，无展开纸面 |
| frame1 | 161 | y1~13 | y13~143 | 纸面完整展开 |
| frame2 | 160 | y1~13 | y13~142 | 微收 |
| frame3 | 158 | y1~13 | y13~140 | 末帧定格 |

规律：**纸面底边 ≈ 帧高 − 18**（161−18=143、160−18=142、158−18=140，逐帧吻合）；木杆顶 y1。

## 几何映射推导（可见高度公式）

代码定位：
- 卷轴帧顶 = `detail_pos.y − CS_SCROLL_CONTENT_OFF.y(25)`（origin(0,0)），故帧内 y ↔ 屏幕 `detail_pos.y − 25 + y`。
- 内容板顶 `ci_pos = detail_pos + charinfo.get_origin()`，origin 补偿后内容板顶 ≈ detail_pos，即内容板顶 = 帧内 y25。
- 内容板 `charInfo1` 高 151，原代码 `Range(0,36)` 裁掉底部自带"排行榜"行 → 有效内容高度 = 151−36 = 115。

内容可见高度 = 纸面覆盖内容板的纵向长度 = 纸面底(帧高−18) − 内容板顶(25) = **帧高 − 43**：

```
content_h = 贴图高 − 36 = 151 − 36 = 115
vis = clamp(帧高 − 43, 0, content_h)
```

| 帧 | 帧高 | vis | 效果 |
|---|---|---|---|
| frame0 | 30 | 0 | 内容全隐藏，只剩木杆 |
| frame1 | 161 | 115（封顶） | 内容全显示 |
| frame2 | 160 | 115 | 全显示 |
| frame3 | 158 | 115 | 全显示 |

定格态（未播放，画末帧 frame3）强制 vis=content_h，保证与旧实现逐字节一致。

## 文字渐显判定

各值行 y（相对 detail_pos，baseline 语义）：职业 −4、等级 14、人气 13、力量/智力 31、敏捷/运气 49、排行 93。字形底 ≈ y+4。文字仅在 `vis>0 && y+4<=vis` 时绘制，避免半截文字浮在纸面外。定格态 vis=115，最大 y+4=97<=115，全部绘制（零回归）。
