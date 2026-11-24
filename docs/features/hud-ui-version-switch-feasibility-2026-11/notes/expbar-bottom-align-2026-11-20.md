# 经验条贴底修正（2026-11-20 00:2x，用户澄清「应挨着底边上方」）

## 用户期望（原话）
「经验条应该挨着底边上方，而不是距离底边还有一点距离」

## 关键拆解：经验条是两层，不是一层
`StatusBar3.img/mainBar/EXPBar` 有两层：
| 节点 | 尺寸 | origin | 作用 |
|---|---|---|---|
| `EXPBar/backgrnd` | **1×24** | (0,0) | 通栏底（用 DrawArgument 横向拉伸到 VWIDTH） |
| `EXPBar/<w>/layer:back` | 1920×**10** | **(0,-23)** | 亮绿/黄绿填充条（真正的"经验条"视觉） |
| `EXPBar/<w>/layer:gauge` | 1904×7 | (-205,-25) | 进度量规 |
| `EXPBar/<w>/layer:cover` | 1522×7 | (-205,-25) | 覆盖层 |

## 错误尝试与纠正（留痕，防复发）
1. **第一次错误**：把 `position.y` 从 `VHEIGHT-111` 改 `VHEIGHT-97`（整体下移 14px）
   → backgrnd(24px) 下半截超出视口，经验条文字被裁掉一半。
2. **第二次错误（更早）**：`exp_pos.y = EXP_BOTTOM`（87）当"视觉顶"
   → 实际视觉顶 = 87 − (−23) = 110，绿条被推出视口 → 用户报「经验条不见了」。
3. **最终正确解**：
   - `position.y = VHEIGHT - 111`（backgrnd 24px 底 = 87+24 = 111 = VHEIGHT 贴底）
   - `exp_pos.y = EXP_BOTTOM + EXP_ORIGIN_Y + 14 = 87 - 23 + 14 = 78`
     → `layer:back` 视觉顶 = 78 − (−23) = 101，视觉底 = 101+10 = **111 = backgrnd 底 = 视口底**（绿条贴底）
   - `statset_pos.y = EXP_BOTTOM + 15 = 102`（文字在 101..111 的绿条内垂直居中）

## 代码
- `IO/UITypes/UIStatusBar.cpp:361` `exp_pos = (0, EXP_BOTTOM + EXP_ORIGIN_Y + 14)`
- `:402` `statset_pos = (VWIDTH*21/100 + 120, EXP_BOTTOM + 15)`
- `:564/571/578/585/592` `position.y = VHEIGHT - 111`

## 验证
- BUILD_EXITCODE=0；live SHA `a3a576c17c939d4d16ffd7ec9a2f4e31…` 与产物一致（实时核对）。
- 截图 tools/_v28_exp.png：黄绿经验条紧贴视口底边，文字 `616169972 [12.85%]` 在条内完整可见。
- 客户端 PID 50900 进图正常。
