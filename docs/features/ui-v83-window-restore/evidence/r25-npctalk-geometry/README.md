# R25 — `UINpcTalk`（NPC 对话框）几何取证

对象：`IO/UITypes/UINpcTalk.{h,cpp}`。素材侧只读取证（`tools/nx_inspect.py` 读 `.nxcache/UI.nx`），
版式侧用原生截图逐列量。复现脚本在 `scripts/`，原始输出在本目录。

## 1. 素材侧：`UtilDlgEx.img` 不提供任何按钮槽位

`UI.nx` 的 node-0 直接就是各 `.img` 名（**没有 `UI` 这一层中间根**，第一版脚本按 `UI/UIWindow.img`
去找就 `TypeError` 了）。`UIWindow.img/UtilDlgEx.img` 实测：

| 节点 | 尺寸 | origin |
| --- | --- | --- |
| `t` / `c` / `s` | 529×28 / 529×20 / 529×58 | (0,0) |
| `it` / `ic` / `is` | 529×28 / 529×20 / 529×58 | (0,0) |
| `bar` | 121×19 | (0,0) |
| `line` | 332×2 | (0,0) |
| `notice` | 311×134 | (0,0) |
| `list0..list3` | 119×14 / 102×18 / 26×14 / 106×14 | (0,0)，仅 `list3` = (−1,0) |
| `dot0` / `dot1` | 6×7 | (0,0) |

9 个 `Bt*` 节点（`_r25_utildlgex.txt` 全量）：

```
BtClose                                            85x19   origin (0,0)   keyFocused 87x20 (1,1)
BtNext / BtNo / BtOK / BtPrev / BtYes              46x18   origin (0,0)   keyFocused 48x20 (1,1)
BtQGiveup / BtQNo / BtQYes                         57x17   origin (0,0)   keyFocused 60x20 (2,2)
```

⇒ **所有 origin 都是 (0,0)**，`MapleButton::bounds(parentpos) = parentpos + position - origin`，
所以 `position` 必须由代码给；不给就全叠在窗口左上角。
⇒ 本包**没有** `BtAllLevel / BtMyLevel / BtQCNo / BtQCYes / BtQAfter / BtQStart`，
`try_add_button` 会跳过它们（不是 bug，是素材代次差异）。

## 2. 底带按钮纵向：`BTN_DY = height + 49`（两帧独立互证）

壳体总高 = `t.height + 正文height + s.height` = `28 + height + 58`。
两张**独立**的原生帧各自反解出 `height`，再和量到的按钮顶边相减：

| 帧 | 尺寸 | 反解 `height` = H−86 | 按钮顶边实测 | 差 |
| --- | --- | --- | --- | --- |
| 参考帧（希娜，528×261） | 528×261 | 175 | 224（`ref_btn` 裁片 y222..246 内 46 宽色块） | **49** |
| `NPC对话2.png`（526×349） | 526×349 | 263 | 312（`npc2_close` 裁片 y310..340 内 85 宽色块） | **49** |

两帧独立得到同一个 49，不是凑的。底片 `s` 内的分隔条在 +15..+20，按钮压在 +21，
`21 + t.height(28) = 49` —— 与上表自洽。
落为 `IO/UITypes/UINpcTalk.h` 的 `BTN_DY = 49`，用 `y_cord = height + BTN_DY`。

## 3. 底带按钮横向：右对齐留边 9，成对间距 65

| 常量 | 值 | 实测来源 |
| --- | --- | --- |
| `BTN_X_RIGHT` | 474 | 参考帧 46 宽按钮落在 474..519（`_r25_btn.txt` 行 `ref_btn ... (474, 519, 46)`），右留边 528−519=9 |
| `BTN_X_CLOSE` | 12 | 参考帧「结束对话」85 宽落在 12..95 |
| `BTN_PAIR_PITCH` | 65 | `NPC对话.png`（莎丽，接受/拒绝）57 宽一对落在 401 / 466 ⇒ 间距 65 |
| `BTN_X_RIGHT_WIDE` | 466 | 57 宽的 `BtQNo` 右位实测 466（466+57=523=帧宽） |
| ⇒ `x_pair` | 409 | 46 宽成对时左那颗 = 474−65 |
| ⇒ `x_pair_wide` | 401 | 57 宽成对时左那颗 = 466−65 |

分支映射（`IO/UITypes/UINpcTalk.cpp:650-728`）：
`SENDOK→OK@474`、`SENDNEXT→NEXT@474`、`SENDPREV→PREV@409`、
`SENDNEXTPREV→PREV@409 + NEXT@474`、
`SENDYESNO→YES@409 + NO@474`（主操作在左）、
`SENDACCEPTDECLINE→QYES@401 + QNO@466`。

> **原生顺序是「主操作在左、次操作在右」**（莎丽帧 401=接受 / 466=拒绝），
> 不是常见的「确定在右」。这条是从像素反推的，不是猜的。

## 4. 正文块：`TEXT_X=166 / TEXT_WIDTH=310`，`TEXT_Y` 未定案

对三张帧（528×261 / 526×349 / 523×203）用「行内暗字像素占比」找文字带、再在带内逐列找左右缘
（`_r25_dlg_frames.txt`）：

- 正文**左缘** 163..170、**右缘** 474..476 ⇒ 取中位 `TEXT_X = 166`，折行宽 `TEXT_WIDTH = 310`（右缘 476）。
- 旧实现 `x=162 + maxwidth=320` ⇒ 右缘铺到 482，正好压进右下角「确定」的 474..520 —— 这就是「按钮位置不对」的一半真因。
- 原生行距实测 **18**（`NPC对话.png` 两行墨迹 72..83 / 90..101）；
  `A12M` 位图字高 12，`linespace = height*1.35+1 = 17`，绘制点 `char_y = y + ay - ch.bt` ⇒ **墨迹顶 ≈ draw_y + 7**。
- **`TEXT_Y` 没有定案**：三帧墨迹顶分别是 39 / 58 / 72，不是同一个偏移，疑似原生按正文高度在正文区里**垂直居中**。
  先保留 cj 原值 48（落在区间中间），等真机同场景对帧。

## 5. 推翻的一条"实测"（自我纠错，必须留档）

上一轮我在注释里写了「原生 528×261 参考帧实测正文块左上 (122, 35)」，并据此落了
`TEXT_X=122 / TEXT_Y=35 / TEXT_WIDTH=387`。本轮逐列重量，该帧正文左缘是 **170**，
122 是把 2× 网格叠画图看走眼读出来的 —— **属于凭印象编数字，违反「证据驱动」**。
若没重量，122+387 会让正文向左压到立绘上、向右铺到 x509，比原 bug 更糟。
教训：**注释里写「实测」的数字必须能被 `scripts/` 里的脚本复现**。

## 6. 遗留冲突（未编码，等真机）

`NPC对话2.png` 的 46 宽按钮落在 **450..493**（右留边 33），而参考帧落在 **474..519**（右留边 9）；
同帧 CLOSE 落在 8..92 而参考帧 12..95。两帧都是 ≈529 壳宽，24px 的横向差不来自裁剪能解释。
⇒ **没有把这个孤例编码进常量**，`NEXT` 仍锚在参考帧的 474；真机开同一个 NPC 对话拍一张再定。

## 7. 顺带修掉的另外两条（同文件）

- **命中区**：`dimension` 原来写死 `height + 120`，比壳体重绘高度（`t+正文+s`）高出 30 余 px，
  那段空白会白白吞掉点击（点不到下层窗口/世界）。改成 `top.height() + height + bottom.height()`。
- **立绘朝向**：`speaker.draw(..., true)` 恒 `flip=true`，把本来就面向正文的 NPC 翻成背对。改 `DrawArgument(center_pos)`。
- **翻页步长**：原来写死 400px，而可见区只有 `height - TEXT_SLACK` ≈ 230px ⇒ 每页跳过约 170px 正文。
  改成按壳高步进 + `Range<int16_t>` 逐字裁切。
- **8 位 NPC id**：`strid.insert(0, 7 - strid.size(), '0')` 对 8 位 id 无符号下溢抛 `std::length_error`，
  改 `string_format::extend_id(npcid, 7)`。
