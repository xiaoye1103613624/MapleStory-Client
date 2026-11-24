# 系统设置 UI 修复 + 怪物 foothold 边缘修正（2026-11-22）

> 分支：`feature/dev_0.0.1`
> 依据：两批 10 问代码取证 + 3 项 IDA/REA/服务端补证（见 `docs/features/native-rea-supplement-2026-11-22/README.md`）
> 用户决策：095 主题置灰预留、移除 Modern（本轮已采用）

## 1. 改动范围（一次性闭环）

| # | 文件 | 改动 |
|---|---|---|
| A1 | `IO/UITypes/UIOptionMenu.cpp` | 删除与底板重复的自绘「小/大」（原 :844-851） |
| A2 | `IO/UITypes/UIOptionMenu.cpp` | 下拉字体统一 A12M→A11M + 收起/展开基线对齐（draw_combo_list、draw_meta_combo/list 均 +2）；`fit_combo_text` 裁剪 |
| A3 | `IO/UITypes/UIOptionMenu.h/.cpp` | 语言改下拉（CN/EN），主题改下拉（083/192/095），095 置灰不响应，Modern 移除 |
| A4 | `locale/zh-CN.ini`、`en-US.ini` | 语言项文案改 CN/EN；新增 `sysopt.label.theme`、`sysopt.combo.theme.0/1/2` |
| B1 | `Gameplay/Physics/FootholdTree.cpp` | `get_edge()` 重写：删「≥3 段返地图墙」自造规则，改沿 prev/next 链走到真末端（邻板不存在或 blocking），64 段防环；对齐 Angel `CVecCtrlMob::CollisionDetectWalk/Float` 的 20px 内缩+链末端翻向语义 |

## 2. 关键实现细节

- 语言/主题下拉沿用文件作用域状态 `lang_open`/`theme_open`（避免改类布局触发多 TU 全量 Rebuild 的历史坑；本轮仍因新增枚举/方法全量 Rebuild）。
- 展开列表**向上**展开（底部按钮条 y=366 空间不足，向下会超出窗口命中区）：行顶 = `Y - COMBO_H * count`，命中区同步。
- 互斥：语言/主题/原三个下拉（截图/怪物/分辨率）两两互斥，CANCEL/ESC 全部复位。
- 095 项：`set_meta_rows` 中 `set_active(show_theme && i != 2)`，且 `THEME_OPT2` 分支仅 LOG no-op，不落盘不切主题。
- 主题 ordinal 映射：`0=Classic083 / 1=StatusBar2_192`；`load_values` 将 resolve 到的 Modern 映射回 083（设置界面已无 Modern）。

## 3. 构建与部署

- 构建脚本 `tools/_sysfix_build.bat`（隔离 OutDir `build-sysfix/` + 全新 IntDir `build-sysfix-int/`）
- 全量 Rebuild：`MSBUILD_EXIT=0`、error 0；产物 `build-sysfix/MapleStory.exe` 5341184 B，SHA `2b6bdc97507abf5739ab4f500d3f170bd9fbdd3eda1d4500a2d2f927300527b5`
- 部署：备份 live → `MapleStory.exe.bak-before-sysfix-20261122` → 覆盖正式 exe `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- 回滚注意：登录器/客户端启动会对 exe 写入约 1009B 签名/校验信息，live SHA 变为 `ad63fe4fcda6384c920bf8ec1a326b5abfa98a273c3862165990575146ac74dd`；**字节级验证确认新代码已在 live**：`sysopt.combo.theme.`×3、`sysopt.label.theme`×1 命中（旧备份 0 命中）。
- locale 已随构建复制到 live `locale/zh-CN.ini`、`en-US.ini`。

## 4. 真机验收清单（未验收，按 USER.md 红线如实声明）

### 系统设置
- [ ] 背景音乐/效果音响行：「小」「大」各只出现一次，无重影、无底部遮挡。
- [ ] 截图保存位置/怪物情报下拉：展开条目字体不溢出、不高亮错位，选中项与收起态对齐。
- [ ] 语言下拉：展开 CN/EN，点选后界面语言即时切换。
- [ ] 主题下拉：展开 083/192/095 三项，095 灰色不可点；切 083/192 后底栏即时生效；Modern 不再出现。
- [ ] 确定/取消/ESC：下拉展开态正确复位；分辨率切换不回归。

### 怪物 foothold
- [ ] 长平台（≥3 段 foothold）上的怪物走到平台末端即翻向，不越过末端、不掉到下层。
- [ ] 移动 NPC（movable）在平台边缘的行为同步改善。
- [ ] 普通单层平台、下跳穿板（forbidFallDown）无回归。

## 5. 诚实声明与本轮未纳入项

- **真机未验收**：以上清单需要你实机逐项确认，按 USER.md 不称「完全修复」。
- **本轮未改**（需资源/抓包/拍板，非代码层可闭合）：天气云层缺失与路灯 z-order（需 orange-wz 资源+密钥转换）、底部蓝带 A/B/C、商城退出天气（服务端已证实补发 0x373D，需抓包确认客户端时序）、龙咆哮命中特效（原生无视觉 hit 播放器，需 WZ 比对+真机指认）。
- 本轮全程只改上述 7 个文件 + 新增构建脚本与本文档；未碰无关功能。
