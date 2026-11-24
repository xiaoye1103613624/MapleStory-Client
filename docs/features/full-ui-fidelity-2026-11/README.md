# 四功能按参考包完整还原 · 交付记录（2026-11-21）

## 用户诉求
三个竖栏窗口之前是简化版（纯Text+隐形热区），未按需求文档/参考插件完整实现；掉落特效效果不对。用户拍板：**全部按参考包完整还原** + **掉落特效按参考包帧序重做**。

## 参考证据（行号级，见 PLAN.md）
- 染色 `coloringprism.cpp:260-380`：301×406、5页签、投放槽、预览窗121..271、3滑条trackTone/Chroma/Bright 176×8+thumb22×14、OK/Cancel四态
- 伤害统计 `uiDamageRank.cpp:25-60/510-560/1333-1520`：223宽、30/15行、背景4段+title1/2+gauge六种+5四态按钮+滚动条
- 签到 v4 整窗513×346（中文烧进栅格）+cell/v1四状态图67×71
- 掉落 `dropitemaura.cpp:131-134`：grade档位+front/back分层（本端服务端grade 0-5六档）
- 高版本客户端（cms085/HeavenClient）grep 0命中，无高版本可参照

## 资源（全部经 orange-wz 导入 live 并 probe）
ColorPrism.img(6节点)、签到backgrnd+cell4、伤害统计9canvas+gauge六种+5按钮四态、掉落12webp→6档front/back。详见 PLAN.md 资源表。

## 代码（按参考重写）
- `UIDailyCheckin.h/.cpp`：去英文Text+cell状态图叠加，格子几何按 manifest(67×71,原点7,59,X间距5,行距72)，中文提示
- `UIColorPrism.cpp/.h`（子代理）：5页签+投放槽+CharLook预览+3滑条track/thumb拖拽(参考数学)+OK/Reset真按钮+中文文案；简化项：预览用CharLook站姿(非v83 Detours物理走动)、数值键盘输入未做、双图层chip固定本体
- `UIDamageRank.cpp/.h`（子代理）：背景4段+title1/2+gauge六种(ratio裁剪,本机self金黄/他人common)+5四态按钮+滚动条+中文文案；简化项：固定223高(玩家7行/技能4行可见+滚动),非参考的随行数增高
- `DropAuras.cpp/.h`：grade 4字符串→6数字映射(PacketCreator.java:1884-1895 0-5)，front/back分层

## 构建与部署（三证+铁律）
全量 Rebuild：`BUILD_EXITCODE=0`、链接行、error=0、产物5163520B。
按铁律部署 `E:\MXD\BeiDou-Client-xy\MapleStory.exe`，实时核对 live SHA = 产物 SHA = **`05AF98BCD24D38A07DDB732C84113FEC033E53732394F0B6D07C4BBF51FDD582`**（11:18:12）。

## 诚实边界
1. **掉落特效16帧逐帧动画无素材**：需求包每档仅1张webp，参考包16帧(120ms循环)依赖BasicEff.img原包——本版实现"每档正确光环+光柱静态分层+6档grade映射"（此前grade全错+节点空所以完全无效），逐帧流动动画需补16帧素材，属新资源需求。
2. 染色预览/伤害统计窗口高度如上简化（已注释说明）。
3. **真机未验收**：构建过≠显示对，待用户用当前 live 正式 exe 复核，截图须本端。

## 回滚
live 部署前在场的 6 个 `MapleStory_*20261120.exe` 变体未删（可选用其一回滚）；源码 git 分支 feature/dev_0.0.1 未 commit。
