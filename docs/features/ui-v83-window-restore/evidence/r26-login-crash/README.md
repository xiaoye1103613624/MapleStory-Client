# R26 — 部署后登录阶段 100% 崩溃（增量构建脏 obj）

> 结论先说：**不是逻辑 bug，是构建系统的 ODR 布局错乱**。构建门禁「0 error」抓不到，只有真机能抓到。

## 1. 现象

| 项 | 值 |
| --- | --- |
| 崩溃产物 | `build_check/MapleStory.exe` md5 `0688b7c146e2eccb9fbd0df198acf2bc`，4,340,736 B，mtime 2026-11-01 05:48:31 |
| 部署位置 | `E:/MXD/BeiDou-Client-xy/MapleStory.exe`（同 md5，已核） |
| 上一份能跑的产物 | md5 `51d3fe1790495a4bfbbd1b1c80cb11eb`，4,338,176 B，mtime 04:42:46（PID 49660 连续在线约 1 小时） |
| 崩溃时机 | 启动后 11~25s，NEXON splash / 登录音乐（`BgmUI.img/Title` 已进 `_audio.log`）之后，**进图前的 `Stage::loadplayer`** |
| 复现率 | 2/2（PID 44320 @05:52、PID 44192 @05:58），两份 minidump 的 rip / fault_va / 全部寄存器**逐字节相同** |

抓帧 `_r26_boot_small.png` 是 NEXON splash —— 顺带暴露 `_boot.py` 的 HUD 判据缺陷：
它只看**客户区底部 60 行亮像素 > 500**，splash 的白底直接满足，于是把「还停在开屏」误报成
`HUD bright px=66560 after 11s -> in game`。**任何「已进图」的自动判定都不能只用底部亮度。**

## 2. 崩溃点取证（无 IDA / 无 cdb，用 WER minidump + dbghelp）

dump 位置：`%LOCALAPPDATA%\CrashDumps\MapleStory.exe.<pid>.dmp`。
本会话没装 Windows SDK Debuggers（无 `cdb.exe`），也没有 ida-pro-mcp，所以用
`scripts/_r26_dump.py`（纯 struct 解析）+ `scripts/_r26_sym.py`（`dbghelp!SymLoadModuleExW`
挂 `build_check/MapleStory.pdb`）自己搭了一条符号管线，结果：

```
MAPLESTORY base=0x7ff7fc2f0000 size=0x494000
EXCEPTION tid=22412 code=0xC0000005 (ACCESS_VIOLATION) nparams=2
  rip = 0x7ff7fc2fe39f  ->  MapleStory.exe+0xe39f  ->  xstring:1296 (+26)
  access = READ   fault_va = 0xfffffffffffffff8
  rax=0  rcx=0x100000000  rdx=0x100000001  rbx=exe+0x4149e8  rbp=exe+0x4142f8
  栈扫描命中的最近代码返回地址：sp+0x0e8 -> exe+0x6e5a6 -> Stage.cpp:66 (+30)
```

- `xstring:1296` 落在 `basic_string::operator=(basic_string&& _Right)` 的 `_Tidy_deallocate();` 上
  （对照本机 `VC/Tools/MSVC/14.51.36231/include/xstring` 逐行数出来，见 `_r26_sym_44320.txt`）。
- `Stage.cpp:66` = `player = entry;`，在 `Stage::loadplayer(const CharEntry& entry)` 里。
- `fault_va = -8`、`rax = 0` ⇒ 以近空基址访问负偏移成员 = **两个编译单元对同一类的字段偏移认知不一致**的典型指纹，
  而不是「某个指针忘了判空」（判空缺失通常落在 `+0x0`/`+0x8` 这类正偏移）。

解析 minidump 时有两个必须记住的字段顺序坑（第一版脚本就因此读出 `nparams=4230996895` 这种鬼数）：

1. `EXCEPTION_RECORD64` 的 `ExceptionAddress` 在 `NumberParameters` **之前**
   （`u32 code, u32 flags, u64 rec, u64 addr, u32 nparams, u32 pad, u64 info[15]`，共 152B），
   跟 32 位 `EXCEPTION_RECORD` 一致，**不是**文档里常被抄成「NumParams 紧跟 Record」的那个顺序。
2. `MINIDUMP_LOCATION_DESCRIPTOR` 是 **size 在前、rva 在后**；`sizeof(MINIDUMP_MODULE)=108`
   （`VS_FIXEDFILEINFO` 是 52B 不是 64B）；`sizeof(MINIDUMP_THREAD)=56`，`ThreadContext` 在 `+48`。

## 3. 根因

本轮 R23-A/R23-B 给两个被大量源文件 include 的公共类**加了成员**，即改了类布局：

```cpp
// Gameplay/Physics/Foothold.h
uint8_t m_layer;
int16_t m_force;   // ← 新增（履带速度）

// Gameplay/Physics/PhysicsObject.h
double  hbelt = 0.0;   // ← 新增（履带位移，不进 hspeed/摩擦）
...
int16_t fhforce = 0;   // ← 新增（当前落脚点履带速度缓存）
```

而 05:48 那次是**增量构建**（只给 `-p:Configuration=Release`，没给 `-t:Rebuild`）。
事后数 `x64/obj-Release/` 的时间戳：

| 批次 | obj 数 |
| --- | --- |
| 10-31 18:38 / 18:37 / 20:02 / 21:53 …（**改动前**） | **178** |
| 11-01 05:24 ~ 05:26（本轮重编） | 92 |
| 11-01 05:48（UINpcTalk） | 1 |

`Stage.obj / Spawn.obj / Foothold.obj / FootholdTree.obj / Physics.obj / UIQuestLog.obj` 是新的，
`Char.obj / CharStats.obj / Combat.obj / Camera.obj / Body.obj / Animation.obj …` 是旧的。
新旧 obj 对 `PhysicsObject` 派生类（`Character`→`PlayCharacter`，正是 `player` 的类型）
用**不同的字段偏移**，链接器不会报错 —— 于是 `player = entry;` 里的 `std::string` 赋值按错偏移取基址，读到 `-8`。

**所以：改公共头的类布局，增量构建产物不可信，必须全量 `-t:Rebuild`。**

## 4. 处置与验收

1. `MSBuild MapleStory.sln -t:Rebuild -p:Configuration=Release -p:Platform=x64 -p:OutDir=...\build_check\`
   （日志 `E:/project/_r26_rebuild.log`）。
2. 部署前先备份：`MapleStory.exe.bak_r24-20261101`（md5 `51d3fe17…`，即上一份能跑的）。
3. 退出走 `_quit.py`（`WM_CLOSE` → 悬停确认红色高亮 → 点「确定」，**全程没杀过进程**）。
4. 验收门禁（顺序不可颠倒）：
   - **第一关：能否登录进图**（`_drive.py where` 有窗口 + 抓帧能看到 HUD，而不是 splash）；
   - 第二关才是 §J 的 UI 版式项。

## 5. 顺带记下的既有问题（非本轮引入）

`49660`（04:42 那份能跑的产物）在 05:51 我执行 `_quit.py` 时也留了一份 dump：
`code=0xC0000374 STATUS_HEAP_CORRUPTION`。`45244`（04:42）同样。
⇒ **退出路径上有一个早就存在的堆损坏**，与本轮 R23/R25 无关，之前一直被当成「正常退出」放过了。
已登记为独立问题（见 `notes/open-issues.md` A12），未擅自扩范围去修。

## 6. 补的工具：`scripts/_r26_ingame.py`（替代 `_boot.py` 的假阳性判据）

不再只看底部亮度，改用**全帧**统计量分类 `SPLASH / LOGIN/CHARSEL / INGAME`。
对已有帧回测（`1296×759`）：

| 帧 | 判定 | `white`（>240 占比） | `band_mid` | `colourful` |
| --- | --- | --- | --- | --- |
| `_v_boot.png`（本轮开屏） | **SPLASH** | 0.9362 | 0.0 | 0.0589 |
| `_v_r23_hud.png` | INGAME | 0.0883 | 0.284 | 0.3903 |
| `_v_r23_skill.png` | INGAME | 0.1101 | 0.2813 | 0.3956 |
| `_v_r23_shortcut.png` | INGAME | 0.0927 | 0.2816 | 0.3917 |

`white` 一维就有 0.94 vs 0.09 的量级差，判据稳。用法：
`python _r26_ingame.py wait 300`（轮询到真进图才返回 0；窗口消失 ⇒ 返回 2 并报「crashed」）。

