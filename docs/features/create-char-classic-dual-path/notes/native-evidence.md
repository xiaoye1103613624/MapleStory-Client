# 原生客户端 REA / IDA 取证摘要
日期：2026-11-05
目标：E:\MXD\BeiDou-ClientV17.7\BeiDou.exe（PE x86，imagebase 0x400000，size 0xa94000，
      sha256 1198fa57ca5a7c489bae43ec13c69681d9cabe0f96762f3dc0357facf2e7d4df）
provider：rea / ida（attached，BeiDou.exe.i64）
既有产物：docs/features/login-flow-ui-ida-align/evidence/CUINewCharRaceSelect_ctor_0x615596.txt
          docs/features/login-flow-ui-ida-align/evidence/CLogin_ChangeStep_0x5f53c0.txt
          docs/features/create-char-classic-dual-path/evidence/ida-race-select-wnd.md

## 1. CUINewCharRaceSelect::OnCreate — sub_6157C9（0x6157C9）

反编译得到 4 个 CCtrlButton::Create 与 2 个 canvas blit 坐标：

| 控件 | id | 窗口内坐标 | 对应运行时节点 |
|---|---|---|---|
| BtKnight（骑士团卡） | 1000 | (104, 79) | RaceSelect/knight |
| BtNormal（冒险家卡） | 1001 | (293, 79) | RaceSelect/normal |
| BtAran（战神卡） | 1002 | (511, 80) | RaceSelect/aran |
| BtSelect（选择） | 1003 | (530, 422) | RaceSelect/BtSelect |
| 说明板 text/0 blit | — | (118, 311) | RaceSelect/<class>/text/0 579x163 |
| 标题 textGL/0 blit | — | (315, 52) | RaceSelect/textGL/0 182x39 |

窗口创建参数：CreateWnd(-372,-2128, 700x460)，step3 相机 {28,-1808}，
即窗口屏幕原点 (240,40)（见 evidence/ida-race-select-wnd.md、ui-v83-race-positioning/evidence/layout.md）。
=> 说明板屏幕左上 (358,351)、BtSelect 屏幕左上 (770,462)，与既有 layout.md 一致。

**判定**：说明板与三卡的坐标常量均与原生一致；问题在 BtSelect 未补偿
BtSelect/normal/0 的 origin (36,14)，详见 evidence/img-node-layout.txt §1。

## 2. CLogin::ChangeStep — 0x5F53C0

反编译（既有产物 CLogin_ChangeStep_0x5f53c0.txt）按 step 分发：
  step 3（v13=0）: 创建 CUINewCharRaceSelect（:200-218）
  step 4（v14=0）: 创建 CAvatar + CAvatarLook::Init，pOrigin = (-2369-600*step, ...)，
                   x=22，并调用 sub_5FCF8A(this) 初始化外观池（:148-198）
  step 5（v14=1）: 清标志 (*(this+120)+252)=0
即 step3 = 职业团选择页，step4 = 角色外观/装备创建页，二者是**连续的原生阶段**。

## 3. sub_5FCF8A（0x5FCF8A，size 0x1b3）— 原生外观池与骰子

反编译关键片段：
```
v2 = this + 616;
for ( i = 9; i != 0; --i ) { sub_5FE122(v2-36); sub_5FE122(v2); v2 += 4; }
...
sub_414617("Male", -1);  sub_414617("Female", -1);
v25 = 0; v14 = this + 616;
do {
  result = (*(BYTE*)(this+564) != 0) ? *v14 : *(v14-9);
  if (result != 0) {
    v16 = *(DWORD*)(result-4);
    if (v16 > 1) { v17 = rand(); result = sub_5FD13D(v25, v17 % v16); }
  }
  ++v25; ++v14;
} while (v25 < 9);
```
含义：按性别从 Male/Female 两组各 **9 类**外观池取值，rand() 随机化 →
即运行时 `Login.img/NewChar*/avatarSel/0..8` 的 9 行与 `dice` 骰子动画，
配合 `charSet`（设定角色 225x377）、`BtLeft/BtRight`（15x16）、`BtCheck`（50x24）、
`statTb`（65x77）构成原生 classic 的「角色装备条件 / 头发颜色设置」阶段。

## 4. 证据局限（必须记录）

- rea/ida 的 `search_strings` 走 find_regex 且只索引 ASCII C 字符串；
  对 `charSet` / `avatarSel` / `NewChar` / `Login.img` 均返回 n=0，
  属「其他字符串表示未知」（WZ 节点名在 exe 中多为 UTF-16/构造串），
  **不能据此判定原生没有该阶段**。
- `procedure_pseudo_code` / `batch_decompile` 在本 provider 上返回
  `unreadable_output`（上游 ida-pro-mcp 的 ok 字段类型不匹配），但
  `partial_observation` 里带完整 decompile code，已据此取证（见 §1-§3）。
- 主 exe 是启动器/框架，UI 布局常量多由 WZ/脚本驱动，exe 立即数里
  搜不到按钮字面量；因此**像素级坐标一律再回到运行时 img 节点**
  （evidence/img-node-layout.txt）交叉校验，不以 exe 立即数为唯一来源。
- 既有 IDA 笔记里「原生 classic 只有命名一态」的说法来自早期截图推断，
  与本轮 §2/§3 + 运行时 charSet/avatarSel/dice 资产冲突，已推翻。

## 5. 深挖复核（2026-11-05 第四轮，回答「REA 是否彻底」）

上一轮只到 `CLogin::ChangeStep` 的 step4 分发与 `sub_5FCF8A` 外观池，
本轮补 xref / 反编译 / 全量 PE 串扫描，结论如下。

### 5.1 调用链

| 锚点 | xref / 反编译结果 | 语义 |
| --- | --- | --- |
| `0x5FCF8A` | 唯一调用者 `0x5F5571`（在 `sub_5F53C0`） | step4 的 9 类外观池初始化，不是窗口类 |
| `0xAF6C28` | `0x5FBED7`（在 `sub_5FBA85`） | `sub_5FBA85` 读 `Etc/MakeCharInfo.img/OrientCharMale\|Female`，按 2 性别 × 各 8~9 类装外观 ID 池；不是 UI 窗口 |
| `0xAF6D1C` | `sub_5FCF8A` 与 `sub_7D5CBA` | 外观池数据引用 |
| `0x6246CE` | 调用者 `0x5F42CD` / `0x5F53C0` / `0x5F95B7` | 小窗口 ctor：`CreateWnd(-140,-804,400,100,10,0,...)`，不是 step4 面板 |
| `0xBEDA6C` | 多处 `CLogin` / `CUINewCharRaceSelect` | 职业团页窗口单例 |
| `0x5F5962` | 反编译 | `CLogin` 按 step 批量显隐 `dword_BEDA34..BEDA6C` 全局窗口单例的调度器；**step4 不新建窗口类** |
| `sub_6157C9` | 反编译 | `CUINewCharRaceSelect::OnCreate`：`CCtrlButton::Create(1000,104,79)` / `(1001,293,79)` / `(1002,511,80)` / `(1003,530,422)`，说明板 blit `(118,311)`，标题 `(315,52)`，`CreateWnd(-372,-2128,700x460)`；大量 `VARIANTARG` / `IDispatch` 脚本引擎调用 |

### 5.2 全量 PE 字符串扫描（Python 直读，绕开 `search_strings` 的 ASCII 索引限制）

对 `BeiDou.exe` 全镜像按 ASCII 与 UTF-16LE 两种编码检索
`charSet` / `avatarSel` / `BtCheck` / `BtDice` / `BtLeft` / `NewChar` / `RaceSelect` /
`scroll` / `statTb`：**全部 0 命中**。唯一相关明文是
`Etc/MakeCharInfo.img/OrientCharFemale` / `OrientCharMale`（`sub_5FBA85` 引用）。

### 5.3 由此得到的确定性结论

1. **原生 step4 存在**：`CLogin::ChangeStep` step4 分支初始化 `CAvatar::Init(pOrigin=-2369-600*step, x=22, nScale=100)`
   与 9 类外观池；运行时 `Login.img/NewChar*/charSet` / `avatarSel/0..8` / `dice` / `scroll` /
   `statTb` / `BtCheck` / `BtLeft` / `BtRight` 资产齐全。
2. **step4 没有独立窗口类**：`sub_5F5962` 是复用既有单例显隐，故拿不到「第二个 OnCreate 的
   `CCtrlButton::Create` 立即数表」。这是原生架构结论，不是取证不足。
3. **面板节点名由脚本/WZ 按名查**，主 exe 无按钮字面量，故该面板的**屏幕落点没有 exe 立即数证据**。
4. 与职业团页不同（那一页的坐标来自 `sub_6157C9` 立即数），step4 只能以
   **运行时 img 位图像素锚点 + 同页已实机验证的锚点**作为坐标依据。

### 5.4 推翻上一轮的方法错误

上一轮以「img 节点无 `pos` 向量」判「显式预留」。v83 经典建角 UI 本就把控件坐标硬编码在
代码/脚本里，img 只给位图；`pos` 向量不是唯一的位置信息形式。位图内部的烘焙槽位
（见 `evidence/img-node-layout.txt` §5 与 `notes/native-step4-layout.md`）加上位图尺寸、
origin、帧 `delay`，足以推出板内相对坐标。故该裁决不成立，本轮按原生逻辑实现。

### 5.5 仍然拿不到的证据（不得伪装成原生常量）

- `charSet` 面板在 1280x720 视口里的**绝对落点**：无 exe 立即数、无 img `pos`、无原生截图。
- step4 与命名态的**严格先后**：IDA 只钉到 step4 存在；顺序依据用户缺陷 3/6 与
  `C:\Users\11036\Desktop\正确客户端截图` 中 4 张建角截图均为命名页（观感参考，非坐标来源）。
- `charAlert`（187x120 白底框）与 `statTb` 的显示时机：无 xref 可钉。
