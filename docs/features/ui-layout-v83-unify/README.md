# UI 布局 v83 统一适配修复

> 状态：已实现（2026-09-29）  
> 适用：BeiDou gms083 / 1280×720 / `data.format=img`（无 StatusBar3）  
> 部署：`E:\MXD\BeiDou-Client-xy\`  
> 决策：策略 A（classic 适配表）+ 范围 A（登录流+局内 HUD）+ 乱码 A + 校准 A（IDA）+ 交付 A

## 1. 背景与根因

HeavenClient 偏新布局硬编码 × BeiDou v83 UI 素材 × 1280 迁移残差。  
**不是**单一全局 transform（历史 `set_world_transform` 已回退，禁止重开）。

## 2. 实现摘要

| 子系统 | 改动 | 依据 |
| --- | --- | --- |
| 门闩 | `UiAssetResolve::is_classic_hud()` | `!StatusBar3 && StatusBar.base` |
| 加载弹窗 | Notice/backgrnd/0 外框 + Loading 卷轴；Cancel y≈106 | 避免矮框 Cancel 贴边 |
| 选频道 | `channelid=0xFF` 预选关闭；公告 `BalloonForLoginProcess` | 首点只选中 |
| 选角色 | 木牌 Y 下移；清 CharLook 缓存；禁 Cursor/18 tab | 空锚点叠体 / 数字光标 |
| 登录输入 | text/hitbox 同 nudge；Caret 微调 | ASCII 过滤保留 |
| 光标 | 经典只绑 0..13；现代才绑 LEAF/聊天栏 | 防数字字库误绑 |
| 登录耗时 | Wait 保活 + 精简选角根 + gather 泵消息 | Character 扫描 |
| 进图 | `ensure_map(mapid)` 按需单图；禁全量 Map002 | 黑屏+BGM+未响应 |
| KeyConfig | classic `load_keys_pos` ← IDA `dword_BE27E0` | CUIKeyConfig |
| StatusBar | QS `648+right`；`is_classic_hud` | 800 底栏右移 |
| Equip | classic `iconpositions` ← IDA `dword_BE2260` | CUIEquip |
| ChatBar | classic 门闩对齐 | 同上 |

## 3. 关键文件

- `Util/UiAssetResolve.{h,cpp}`
- `IO/UITypes/UILoginWait.cpp`、`UIWorldSelect.cpp`、`UICharSelect.cpp`、`UILogin.cpp`
- `IO/Components/ChatBalloon.cpp`、`Textfield.cpp`
- `IO/Cursor.{h,cpp}`
- `IO/UITypes/UIKeyConfig.cpp`、`UIStatusBar.cpp`、`UIEquipInventory.cpp`、`UIChatBar.cpp`

## 4. 验证

- [x] Release|x64 构建 → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
- [ ] 手工：登录输入/LOADING Cancel/频道首点/选角双击/底栏 QS/KeyConfig/装备槽

## 5. 构建

```text
MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64
```

（工具集 v145 / VS 2022+）

## 7. 验收残差续修（2026-09-29）

| 现象 | 修复 |
| --- | --- |
| Loading 无进度、Cancel 贴 LOADING | v83 进度节点是 `bar` 非 `circle`；Cancel 底边留白加大 |
| 选角浮出「属性/排行榜」 | BeiDou 下不再绘制 `charInfo1` 大卡（含服务器排行榜），仅名牌+头顶等级 |
| 进图黑屏有 BGM | atlas `clearinternal` 在 quads 非空时延到 `clearscene`；`bmp.data()` 空则不入 atlas |
| 选角占用角色白影/叠体 | `CharlistHandler` 建 `UICharSelect` 前再 `ensure_game_roots()`；`TOP_DEFAULT_ID` 1042399→1040000（BeiDou 有素材） |

