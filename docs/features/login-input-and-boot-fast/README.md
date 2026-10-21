# 登录输入可见/保存 + 启动加速（方案 C）

> 状态：已实现（2026-09-28）；输入可见二次修复；保存闪退修复同日  
> 部署：`E:\MXD\BeiDou-Client-xy\`  
> 适用：BeiDou gms083 / `data.format=img`

## 1. 决策

- 启动：**C** = 先出窗 + img 重根延后到登录成功  
- 登录：保存输入框账号立刻写盘；BeiDou 输入白字 + 竖直微调  

## 2. 改动

| 模块 | 文件 | 内容 |
| --- | --- | --- |
| 保存账号 | `IO/UITypes/UILogin.cpp` | 点连接 / 勾选保存时写 `login.account` + `Configuration::save()` |
| 成功包 | `Net/Handlers/LoginHandlers.cpp` | name 非空才覆盖；登录成功后 `ensure_game_roots()` |
| 输入可见 | `UILogin.cpp` | BeiDou `WHITE` + `A11M`；绘制上移 `(0,-3)` + 光标再抬 `-3` |
| 文本输入 | `Window.cpp` / `UI.cpp` | 聚焦时 `SDL_StartTextInput`；桌面/安卓均处理 `SDL_EVENT_TEXT_INPUT`；启文本输入时 KEY_DOWN 不再重复插入字符 |
| **保存闪退** | `TypeMap.h` / `Configuration.cpp` | `TypeMap::get` 改 `find`（禁止 `operator[]` 插入空指针）；ctor `emplace` 全部 `Patch*` 配置项 |
| 先出窗 | `MapleStory.cpp` | `Window::init` 提前；加载期 `boot_pump("Loading...")` |
| 懒加载 | `ImgSource` / `ResourceService` | boot 只 ensure UI/Map/Sound/String/Effect/Etc；其余 `ensure_remaining` |

## 2.1 保存闪退根因

`PatchUpdater` 通过 `Setting<PatchEnabled>::get()` 等读取补丁配置，但 ctor 未 `emplace` 这些项。`TypeMap::get` 原先用 `unordered_map::operator[]`，会插入 **空 `unique_ptr`**。随后点「保存账号 / 连接」调用 `Configuration::save()` 遍历 `settings` 时对空指针解引用 → 闪退；退出写盘同样失败，表现为 `Settings` 被截成 0 字节。

## 3. 验证

- [ ] 冷启动窗口出现明显短于改前（目标相对 ~20s 大幅下降）  
- [ ] 勾选保存 → 输入账号 → 连接后 `Settings` 有 `login.account`；重启自动填入  
- [ ] 输入账号/密码在木牌槽内可见，光标与「账号」标签大致齐平  
- [ ] 登录成功后选角/进图不因缺根崩溃  

## 4. 后续追踪（未闭环）

**进游戏只有 BGM、画面仍异常**：与本次启动/登录输入分开跟踪。  
建议排查顺序：`_enter_game.log` 是否 done → atlas 是否再误杀大图 → 首帧绘制/相机 → HUD 构造。  
见 `docs/features/enter-game-blackscreen-fix/`。

## 5. 构建

```text
MSBuild MapleStory.sln /p:Configuration=Release /p:Platform=x64
```

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
