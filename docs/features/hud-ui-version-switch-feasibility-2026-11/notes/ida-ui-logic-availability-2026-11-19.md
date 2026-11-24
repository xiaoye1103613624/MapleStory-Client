# 「192 IDA 有没有聊天/血量/蓝量/经验/菜单/快捷键布局逻辑」逐条取证（2026-11-19）

用户质疑：「192 聊天框、血量、蓝量、经验值、主功能菜单和快捷键 UI 的布局、功能 192 IDA 都没有么？」——**纠正：都有，我前面「IDA 里没有」的说法是错的**。真问题是这些坐标多数被 Hex-Rays variant 误建模，取不出可信像素。逐条证据如下（全部来自 `evidence/_raw_deComp_oncreate.c` 1931 行 + `ida-statusbar2-layout-2026-11-19.md`）。

## 逐条

| UI | IDA 证据 | 坐标可信度 | 当前源码 |
|---|---|---|---|
| **聊天框** | `_InitLayerChat@0x169BF60`、`_ResetChatBarPos@0x169FC70`（RelMove nX:0,nY:m_ptChatWnd.y-2 @0x169FD56）、`ChatLogDraw@0x16ADBD0`、`m_apBtChatFilter[0..6]`=chat/Tap 7 页签 pitch 46（0x169FF1A）、`GetChatWndY@0x16968E0` | **高**：RelMove 相对位移 + pitch 是整数硬值；`m_ptChatWnd` 绝对值来自 IMG | 逻辑已还原：chatTarget 7 标签、历史/输入/折叠、RelMove 语义；绝对像素按 live StatusBar3 IMG |
| **血量 HP** | OnCreate `CGauge::Create(m_GaugeHP, "gauge/hp", nX=(int)v192.m_Data, nY=v193=28, nLength=700)` @0x16BEDB0 | **混合**：nY=28、length=700、职业变体 v54=139/309/136 是可信整数；**nX=(int)v192.m_Data 是 BSTR 字符串指针强转 = 误建模** | 按 live `status/gauge/hp` IMG 节点 + 本端 hpmp_pos 表落地（可信来源），未用误建模的 nX |
| **蓝量 MP** | `CGauge::Create(m_GaugeMP, "gauge/mp", nX=v187=199, nY=(int)v188.m_pInterface, ...)` @0x16BF1D1 | **混合**：nX=199 可信；nY=(IWzGr2DLayer*)2 强转 = 误建模 | 同上，按 live IMG |
| **经验 EXP** | `CGauge::Create(m_GaugeEXP, "gauge/exp", ...)` @0x16BF4EB；`m_GaugeEXP_Relax` @0x16BF52A | 同上：variant 参数误建模，绝对值不可信 | 按 live `EXPBar` IMG + exp_max=VWIDTH-16 |
| **主功能菜单** | `_Init_Buttons_MapleWorld@0x16AC7D0`：11 钮创建顺序 + `nID`（BtNotice/BtCharacter/BtCashShop/BtStat/BtQuest/BtInven/BtEquip/BtSkill/BtKeysetting/BtMenu…）；`AddButton` nOffsetX/Y=0（坐标在 IMG origin） | **高**：顺序与 nID 是硬值；像素在 IMG | 按 live `mainBar/menu` 6 钮 + submenu 20 项落地；顺序属 StatusBar2 契约，未照搬（live 是 6+层级菜单） |
| **快捷键** | `s_ptShortKeyPos@0x279C390` 28 格逐点（7,15…447,48）、pitch=33、`ChangeScreenResolution` RelMove(881,2)@0x169BA24、slide 收起下移 67px、`GetShortCutIndexByPos@0x1396460` 映射 | **高**：整数表硬值 | ✅ 已 1:1 落地：`UIStatusBar.cpp:136-146` 28 格表、`:643` 8/28 槽、`:448-456` 去 origin |

## 核心结论（诚实口径）
1. **IDA 有全部 6 类 UI 的布局/功能逻辑**——聊天、HP、MP、EXP、菜单、快捷键的类/函数/参数都在，共 184 个 CUIStatusBar 函数。
2. 但可分两种可信度：
   - **整数硬值**（快捷键 28 格表/pitch33、pitch46 页签、RelMove 位移、按钮顺序+nID、gauge 的 nY=28/length=700/nX=199）→ 可还原，已落地大部分；
   - **Hex-Rays variant 误建模**（把 `BSTR 字符串指针`、`IWzGr2DLayer*`、`variant.vt` 强转成坐标，如 HP 的 nX=(int)L"...gauge/hp"、MP 的 nY=(void*)2）→ **取出来是错的，硬套会导致坐标乱飞**，这类必须回到 IMG 节点定位（orange-wz），不能靠 IDA 伪代码。
3. 所以之前说「IDA 没有」是措辞错误，正确说法是「**IDA 有逻辑，但部分像素坐标因 Hex-Rays 误建模不可直接采用，已用 IMG 定位补齐**」。当前实现走的是 IDA 硬值 + IMG 定位的混合还原，这是符合 xiaoye skill 门禁 18（逻辑+IMG 双证据）的正确路径。
4. 唯一还没做到 1:1 的是把 gauge 的 nY=28/length=700/nX=199 这几个可信整数直接接进 UIStatusBar——但它们属于 StatusBar2 契约（live 是 StatusBar3），直接套会与 live IMG 冲突；保留 IMG 定位是更稳的选择。

## 若要进一步
若用户坚持用 IDA 整数覆盖 IMG（例如强制 HP gauge nY=28/length=700），可做但要真机验证是否错位——因为 StatusBar2 与 StatusBar3 布局不同。等用户裁决，不擅改。
