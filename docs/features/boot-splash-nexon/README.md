# 启动 Logo 加载动画（BootSplash）

> 状态：已闭环（2026-09-30）  
> 适用：BeiDou gms083 / HeavenClient / `data.format=img`  
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`  
> 决策：1B = `Logo.img/Nexon`；2A = 资源前程序化 spinner；资源加载完直接进登录页（跳过 UILogo）

## 1. 背景与目标

- 冷启动窗口起来后原先只有黑底 + 「Loading data」，体感像卡死。
- 目标：先出窗 → 程序化加载动画 → 资源就绪后播 Nexon Logo → **直接进登录页**（不再播 Wizet/UILogo）。

## 2. 影响范围

| 层级 | 路径/模块 | 变更类型 |
| --- | --- | --- |
| 客户端 | `Util/BootSplash.{h,cpp}` | 新增 |
| 客户端 | `MapleStory.cpp` | `boot_pump` → `BootSplash`；`set_start_shown(true)` |
| 工程 | `MapleStory.vcxproj` / `.filters` | 登记源文件 |
| WZ/IMG | 无改动（读既有 `UI/Logo.img/Nexon`） | — |

## 3. 最终实现逻辑

### 3.1 主流程

```text
Window::init
  → BootSplash.pump("Loading...")          # 程序化 spinner
  → PatchUpdater::sync
  → BootSplash.pump("Loading data...")
  → ResourceService::init                   # UI 根就绪
  → BootSplash.try_load_nexon()             # Logo.img/Nexon
  → BootSplash.pump(...)                    # Nexon 循环 + 底部状态字
  → Session / Sound / Music / Stage ...
  → set_start_shown(true)
  → LoginBackground::init → UI::init        # UIStateLogin → UILogin
```

### 3.2 资源节点

- `UI.nx` / `UI.img` → `Logo.img/Nexon`（约 137 帧；`0`=800×600，其后多为 720×480）
- 缺节点时留在程序化 splash，打 `[BOOT] Logo.img/Nexon missing` 警告

### 3.3 跳过 UILogo

- `UIStateLogin` 读 `Configuration::get_start_shown()`；为 true 时直接 `emplace<UILogin>()`
- init 末尾强制 `set_start_shown(true)`，避免 Settings 里为 false 时再播一遍 Wizet

## 4. 验证清单

- [ ] 冷启动：窗口出现后立刻有 spinner / 进度条，而非纯黑字
- [ ] 资源装完后出现 Nexon Logo 动画（黑边 letterbox 可接受）
- [ ] 初始化结束后**直接**到登录木牌页，不再出现 Wizet
- [ ] 无 `Logo.img/Nexon` 时仍能进登录（程序化兜底）

## 5. 构建

```text
MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64
```

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
