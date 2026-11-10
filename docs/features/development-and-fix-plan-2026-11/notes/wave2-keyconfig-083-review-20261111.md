# round75 · UIKeyConfig 键盘设置窗按 Angel 083 CUIKeyConfig 复核

## 0. 授权与性质
- 目标（m00690 等 / goal-11388c42）：以 Angel(13339) 083/v83 带符号 IDB 为权威，从登录链路逐功能 REA→IDA→IMG 只读取证 + 对照清单 + 决策点 + 一次性 Plan。
- 用户硬指令（m09859）：「若碰到ui布局、动画一定要确认是否修复了，如果不是按照Angel 布局修复过的，可以重新修复；之前有很多参照BeiDou或者自行修复的ui、动画都有些问题」。
- 本轮 = read-only 复核 + 纯注释口径纠正（0 行为字面量）；m04292 授权自测/提交/占用直关/不能自测记 backlog。

## 1. 选面
- 全仓扫「截图 SAD 自造版式」：`UIKeyConfig.cpp:114` 自造信号最纯粹（「坐标按 正确客户端截图/键盘设置.png 逐像素实测」）+ 083 有完整 CUIKeyConfig 符号族（10 个）。
- 候选对比：`*Channel*` 12 / `*CharSelect*` 4 / `*NewChar*` 11 / `*CommonCreation*` 0 / `*KeyConfig*` 10 / `*UIGuild*` 18。

## 2. 083 活 IDB 权威（工具 tools/_ida_keyconfig/）
- 10 真符号：ctor @0x832586 / OnButtonClicked @0x8335d0 / OnMouseButton @0x83372c / OnDestroy @0x833749 / OnMouseMove @0x8338f9 / GetShortCutIndexByPos @0x833a65 / TryBeginDragFuncKeyMappedIcon @0x83550d / CNoticeDlg ctor @0x83665b / _ReleaseRaw @0x836de7。
- ctor：CUIWnd(5,3,606,7,&off_B397F0) + SP_2406_UI_UIWINDOWIMG_KEYCONFIG + CreateWnd(z:10,bSetFocus:1)，**无按钮坐标立即数**。
- GetShortCutIndexByPos @0x833a65：dword_BE27E0 成对 int32、32x32 槽（x+32/y+32）；py_eval 实测 (48,66)(82,66)...(422,66) 步长 34 ⇒ 当前端 keys_pos 逐字抄自 083。
- CNoticeDlg @0x83665b：SP_2503 建 266x116 确认框（CreateDlg a4:266 a5:116）。

## 3. 排除「083 其实给了按钮坐标」（拼图闭合）
- off_B397F0 = L"KeyConfig" BSTR 数据，非坐标表；sub_832834 = 析构（释放 6 个 CCtrlButton ZRef）；区间 0x832000-0x837000 无本窗 CCtrlButton::CreateCtrl；全库 CreateCtrl 仅 CheckBox/ScrollBar/Slider 三个具名；vtable 0x832979 是 CUISoftKeyboard::OnCreate。
- 结论：083 主路径确给不出五按钮坐标，y=234 截图定标属门禁 18 合法备选，但必须标「非 083 背书」。

## 4. orange-wz 门禁 + .nx
- AGENTS.md：先 orange-wz MCP（65536B GMS key / sha256_16 268a8407a47ee76c / iv TSPHKw== / exclusive:false）；9 节点×2 IMG 全部 `IMG 解析失败: UIWindow.img` / `StatusBar.img`（GBK 原文留证 tools/_kc_probe_stdout.txt）→ 合规 .nx 回退。
- .nx 实测 UIWindow.img/KeyConfig 12 直接子节点（backgrnd 629x373 origin(314,186) + 7 颗 Bt* + icon/key/notice/quickslotConfig），**无任何坐标/consent 属性**；StatusBar.img/KeyConfig NOT FOUND → 当前端 fallback window("KeyConfig") 属实。

## 5. 对照清单 + 定性（混合面，非第 8 例同族病）
- 按 083 反例部分：dimension/keys_pos/7 颗真 Bt*/回退判断全部落在真数据表与真节点上。
- 截图派生部分：5 按钮 y=234 一行坐标来自截图，083 主路径已尽仍缺该立即数 → 门禁 18 合法备选；无虚构资源根、无自造兜底替代真资源、无冒称 083 依据 ⇒ 不是 round66/67/68 那种同族病。
- 真机观感残差：5 按钮是否按真机截图微调 → 决策点 D-F，交用户裁决，本轮零实施。

## 6. 决策点 + 一次性 Plan
- D-F：5 按钮 y=234 观感对齐（需五屏实机截图）。Plan（用户裁决后）：① 采 5 屏原生截图 ② 逐像素比对 ③ 仅微调坐标常量 ④ harness + GATE + 真重编 + 原子提交。本轮不动。

## 7. 补丁（0 行为字面量）
- IO/UITypes/UIKeyConfig.cpp 3 块纯注释：① 五按钮块补 083 依据 + 标「截图备选 / 非 083 背书」② load_keys_pos 补 GetShortCutIndexByPos @0x833a65 32x32 + dword_BE27E0 实测字节 ③ button_pressed(DEFAULT) 补 CNoticeDlg @0x83665b 266x116 依据。
- 行为字面量（5 个 Point(...,234) / NUM1(48,66) / dimension=bg_dimensions / Locale::text_or）逐字不动。

## 8. 提交 scope
- 精确 git add 6 文件：UIKeyConfig.cpp / tools/test_round75_keyconfig_083.py / evidence/keyconfig-083-round75.json / notes/wave2-keyconfig-083-review-20261111.md / dev-plan README（2-30）/ PLT README（§41）。一次性脚本与 IDA 反编译仅本地证据链留存，不进版本库。

## 9. 教训
- 连续三面（UILoginWait / UILogin / UIKeyConfig）证明「截图 SAD」标签下既有反例也有合法混合面，必须逐面用 IDB 立即数 + IMG 节点定性，不能一刀切。
- CRLF 源码补丁用锚点行自动取缩进，避免 tab+中文 old_string 失配；unicode 箭头只在 MD 用，终端打印一律 ASCII-safe。
- 083 主路径「给不出坐标」本身是可取证结论（BSTR / 析构 / 无 CreateCtrl / IMG 无属性四证），不是放弃取证的借口。
- 校准≠改回原生 / s9 优先：即便 083 与 s9 的确认框实现不同（CNoticeDlg 266x116 vs UIOk+text_or），也不动行为。
