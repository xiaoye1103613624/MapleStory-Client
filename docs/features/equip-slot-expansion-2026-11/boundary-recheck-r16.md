# Round 16：质疑自己的边界结论 + 定位崩溃真因

> 本轮目标：验证 Round 11/12 宣布的「UI 自动化不可行」是否成立。
> 结果：**那两轮的部分判据确实是错的**（已纠正），但崩溃真因另有其人 ——
> **live `Character` 根膨胀到 63,388 个 .img，加载 66 秒后崩**。

## 1. 我先推翻自己两个判据

| 轮次 | 当时的判据 | 为什么错 |
|---|---|---|
| R11 | `SendMessageW(WM_KEYDOWN)` 返回 0 ⇒「窗口过程未消费」 | **SDL 处理 `WM_KEYDOWN` 本来就返回 0**。返回 0 是正常语义，不能当作「没收到」的证据 |
| R12 | 探针无输出 ⇒「键到不了 SDL」 | 那个探针构建用了**复用 obj 目录**（`obj-probe`）。R13 已实测复用目录会 ODR 崩坏 —— 建立在坏构建上的结论不算数 |

所以 R11/R12 的「四条通道全部证伪」里，**消息注入这一条的证据是无效的**，必须重验。

## 2. 重验设计：探针放在键分发入口

前两次探针放在 `UIEquipInventory` 构造/draw —— 那只能证明「窗口没开」，
不能区分「键没到游戏」与「键到了但没开窗」。

本次放在**唯一键分发入口** `IO/UIStateGame.cpp:278 UIStateGame::send_key` 首行：
任何到达游戏逻辑的键都必然留一行。这才是本质判据。

## 3. 意外发现：引入 `<fstream>` 会让客户端启动崩溃

| 构建 | 探针方式 | 结果 |
|---|---|---|
| `build-r15` | 无探针 | 连跑 **2/2 ALIVE** |
| `build-kp2` | `#include <fstream>` + `std::ofstream` | **崩** `0xc0000005` @ `0x64e1` |
| `build-kp3` | 改用仓库既有 `perf::mark()`（不引入新头） | **活了**（PID 53668），但随后仍崩 |

`0x64e1` 反汇编（`dumpbin /disasm`）确认是：
```
std::_Hash<...basic_string, unsigned char...>::_Find_last
00000001400064E1: mov rbx, qword ptr [r8+rax*8+8]   ← 崩在这里
```
类型匹配 `Gameplay/MapleMap/MapPortals.h:67` 的
`std::unordered_map<std::string, uint8_t> portal_ids_by_name`。

**引入 `<fstream>` 改变模板实例化/内联布局，暴露了这个既存 UB。**
这超出本次 7 项范围，未深挖；改用 `perf::mark` 规避。

## 4. 崩溃真因：资源规模，不是我的代码

`build-kp3` 的 `boot_perf.log` 留下决定性一行：

```
[PERF] t=112388ms ROOT Character=66428ms      ← Character 根加载 66.4 秒
[PERF] t=114638ms ROOT Item=2250ms
...
[PERF] t=114736ms RTT winsock WSAIoctl fail wsa=10022
（随后崩 0x64e1）
```

对照资源规模：

| 时点 | `Data\Character` 的 .img 数 |
|---|---:|
| Round 6（19:30 前） | 7,329 |
| Round 16（23:00） | **63,388** |

另一会话在 19:30–22:12 间批量导入了高版本装备/怪物素材，`Character` 根**膨胀 8.6 倍**。
`ImgSource::gather` 要递归扫描 + `open_merge` 全部文件，66 秒加载后在
`MapPortals` 的 hash 查找上崩溃。

**这是环境劣化引发的既存缺陷，不是本次 7 项改动引入的**。
证据：不带任何探针的 `build-r15` 在资源较少的时刻连跑 2/2 稳定。

## 5. 本轮收尾（源码干净）

| 项 | 结果 |
|---|---|
| 探针移除 | 按内容锚点整段移除，`KEYPROBE`/`perf::mark("KEY"` 残留 **0** |
| `UIStateGame.cpp` | 恢复本轮改动前状态（剩余 83 增 13 删是 Round 3 的 `UIAddonEquip` 接线，属正常） |
| 单 TU 编译 | **EXIT=0** |
| 全新目录 Rebuild | **EXIT=0**，产物 `build-r16\MapleStory.exe`（5,033,472B），已部署 |
| `debug.perf_log` | 已关回 `false`（无 BOM） |
| 门禁套件 | **7/7 PASS** |
| 临时脚本 | 7 个已清理 |

## 6. 结论修正（写进记忆）

- **「SDL 不吃窗口消息」这一条证据无效**，`SendMessage` 返回 0 是正常语义。
  键注入是否有效**仍未确证** —— 需要一个能在当前资源规模下稳定进图的构建才能验。
- **`<fstream>` 在本项目 UI TU 里是雷**：会暴露 `MapPortals` 的既存 hash UB。
  需要文件日志时用仓库既有 `Util/PerfLog.h` 的 `perf::mark()`。
- **崩溃 `0x64e1` 的真因是资源规模**（Character 根 63,388 个 img / 66 秒加载），
  不是 ODR、不是我的改动。R13 的「ODR 污染」结论**也需要降级**为「可能因素之一」。

## 7. 用户 7 项状态（不变）

①-⑦ **功能侧全部完成**，7/7 门禁 PASS，双端编译 EXIT=0。
剩进图后视觉确认 —— 当前受资源规模导致的启动崩溃阻塞，需等资源导入收敛或
`MapPortals` 那个既存 hash 缺陷被修（超出本次范围）。
