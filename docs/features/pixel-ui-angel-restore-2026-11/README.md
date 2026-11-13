# 像素级 UI 还原总台账（Angel + OmniParser + 多分辨率）

> 状态：Wave 0–4 收口（2026-11-13）；**live 已部署**；进图 9 档矩阵 **9/9 pass**（skillbook 下溢已修）  
> 对照：Angel 083 IDB + orange-wz（128B key）+ **正确客户端截图**  
> 验收：原生窗 1280×720 ≤1px；S 窗自定义（值进框 + 底对齐）

## 已锁定决策

见会话 Plan「Pixel UI Restore」。登录 1280×720；进图 `window.game_*`；OmniParser 截图兜底；s9 协议不动。

## 目录

```text
docs/features/pixel-ui-angel-restore-2026-11/
  README.md
  notes/                  # 波次笔记、screenshot-fallback、clamp 约定、像素量测
  evidence/screens/       # 正确端截图工作副本（中文原名 + 英文别名）
  evidence/layout-specs/  # layout.py spec 模板与实例
  tools/                  # OmniParser→layout JSON、HudLayout 单测
    patches/
```

## 波次状态

| Wave | 内容 | 状态 |
|---|---|---|
| 0 | OmniParser + layout 约定 + clamp | **已完成**（MCP 已配；后端权重仍可后台装） |
| 1 | 登录链像素 | **已核证**（Angel 对齐；权威截图全帧 1280×720） |
| 2 | 底栏/聊天/快捷键 | **已加固**（合图缝 466 = `QS_PANEL_W_26`；`HUDLAYOUT_OK`） |
| 3 | S/E/I/K/P/G/F/Q | **S V4** + **E/I/K**（`w03-eik-pixel`）+ **P/G/F 底栏 + Q 列表高亮**（`w03-pgf-q-pixel`）；KeyConfig / S EP·抗性仍残差 |
| 4 | 多分辨率收口 | **live 已部署**；进图 9 档 **9/9 pass**（见 `notes/setfield-skillbook-fix-2026-11-13.md`） |

视觉对照源（用户指定，一律以此为准）：`C:\Users\11036\Desktop\正确客户端截图`  
映射表：`evidence/screens/screenshot-map.md` · 量测：`notes/pixel-measure-2026-11-13.md`  
Live：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`（Wave3 EIK：与 `_pixel_build_stage` 同指纹，见 `notes/w03-eik-pixel-2026-11-13.md`）  
部署笔记：`notes/deploy-live-2026-11-13.md`
## 工具入口

- OmniParser-MCP：`tools/vendor/OmniParser-MCP/`（见 `notes/omniparser-setup.md`）
- layout 转换：`tools/omni_to_layout.py`
- 像素门禁：`docs/features/ui-v83-window-restore/tools/layout.py`
- Hud 公式：`Util/HudLayout.h`
