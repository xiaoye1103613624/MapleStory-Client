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
