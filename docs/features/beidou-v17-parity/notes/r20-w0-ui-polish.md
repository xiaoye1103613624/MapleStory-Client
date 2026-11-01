# r20 · W0 UI 抛光（Loading / 滑动 / 选频木牌 / 退出 / i18n）

> 日期：2026-10-22  
> 决策：D1=A D2=A D3=A（默认 zh-CN）D4=A D5=A  
> 部署：`E:\MXD\BeiDou-Client-xy`

## 根因与改动

| 项 | 根因 | 改动 |
| --- | --- | --- |
| 木牌无「蓝蜗牛」（第三轮） | s9 每个世界单独 SERVERLIST 包，末包仅 `0xFF`；旧逻辑**每包 clear_worlds**，结束包把世界清光 | 去掉包头 clear；`add_world` 在 `world_list_finalized` 后自动开启新一轮；结束包再 `draw_world` |
| 木牌消失（第二轮） | 误藏全部 `e` + step1 相机 X 改 137 | 恢复 step1 `X=28`；定稿后右侧填满 e |
| 左侧第三钮空白 | ColorBox 遮罩盖住 WZ，叠字未绘出 | 本地 PNG「查看所有角色」覆盖（`ui/worldselect-sidebar/v1`） |
| Loading 无动效 | 进度节点/位置不当 | 自动选 `bar/circle/ani`；多帧 Sprite 播放；槽内居中偏下 |
| 退出确认英文+偏上 | WZ 英文字图 | `locale` + 印章右侧居中 |
| i18n | — | `locale/*.ini` + `ui.language` |

正确截图侧栏顺序：选择频道(step) / 查看世界地图 / **查看所有角色** / 回到首页。

## 配置

```yaml
# xy-config.yaml 或 Settings
ui:
  language: zh-CN   # 或 en-US
```

运行目录需有 `locale/`（与 `MapleStory.exe` 同级）。

## 验收

1. 登录 Loading：进度条在 LOADING 槽内动画，窗体居中。  
2. 进入选频：相机主要为上下滚，木牌出现真实世界竖牌（非空 e）。  
3. 点世界 → 频道网格 → Go / 再点进选角；进角 Loading 同源。  
4. 侧栏：查看世界地图 / 所有角色（叠字）/ 回到首页；前两者提示暂未开放。  
5. 结束游戏：中文「确定要退出游戏吗？」位置对齐印章。  
6. `ui.language=en-US` 热重载后退出文案变英文。

## 预留

- `BtViewAll` 正式美术入库（Photoshop → orange-wz）；当前叠字为过渡。  
- 查看世界地图 / 所有角色真实功能。  
- orange-wz MCP 本轮不可用，未做节点 dump。

---

## 追补（2026-10-23）：Loading 进度条位置/满条跳页 + 选频侧栏实测坐标

用户实机反馈两项，均已按参考端截图/WZ 实测修复并构建（EXITCODE=0）。

### 1. Loading 进度条偏左下、未到 100% 就跳频道页

根因（位置）：旧偏移 `kBeiDouBarOff(46,98)` 压在 LOADING 字形左侧；r20 改的
「槽底居中」`((282-109)/2, 144-8-32)=(86,104)` 又偏左下且与文字不同基线。
根因（跳页）：`bar` 是 11 帧循环动画（bar/0..10，每帧多亮一格，0%..100%，
109×8，帧无 delay 节点 → 100ms/帧），由 `UIElement::update` 驱动循环播放；
登录/选角回包到达即 `remove(LOGINWAIT)`，循环被从中段掐断 → 「没到 100% 就跳页」。

实测（`tools/_wz.py` 解码 `Notice/Loading/backgrnd` 282×144 + 像素扫描）：
白槽区 y=90..113（中心 101），LOADING 字形 x=51..95（基线中心 y≈101）。

修复（`IO/UITypes/UILoginWait.*`）：

- 进度条绘于 **(99, 97)**：紧随 LOADING 右侧（95+4）、与文字同基线居中（101-4）。
- 帧改为**手动按进度绘帧**（不再入 `sprites` 循环）：`fill_elapsed` 按 TIMESTEP
  累加，满条停在 bar/10（100%）。
- 新增 `begin_finish(oncomplete)`：回包到达不立即拆窗，标记后由 `update() 在
  满条后 deactivate + 执行续场逻辑；`close()`（取消）会丢弃续场逻辑。
  `LoginResultHandler` 成功路径与 `CharlistHandler` 改走 `begin_finish`
  （无进度条素材时退化为立即执行，保持旧行为）。

### 2. 选频页左侧按钮位置/尺寸不对

参考端截图实测（1280×720，`evidence/r20/correct_shots/选择频道页.png`，
暗红带像素扫描 + WZ 精灵内容边界对齐）：

| 元素 | WZ 节点（尺寸） | 参考端坐标 |
| --- | --- | --- |
| step「选择频道」 | `Common/step/1`（150×57，暗红带在精灵内 y=3..56） | (0, 33) |
| 查看世界地图 | `WorldSelect/BtViewChoice/normal/0`（99×35） | (0, 127) |
| 查看所有角色 | `WorldSelect/BtViewAll/normal/0`（99×35，WZ 文案错为「查看世界地图」→ 本地 PNG 覆盖） | (0, 377) |
| 回到首页 | `Common/BtStart/normal/0`（98×34） | (0, 427) |

按钮与参考端同节点同尺寸（原生 WZ 大小），差异纯在坐标：旧实现 x=42 且三钮
从 step 下 36px 起连续堆叠（193/256/319）。修复为贴左缘 x=0 + 上表坐标；
第一钮与第二钮之间是大段留白（参考端如此）。`WS_SIDEBAR_AFTER_STEP`/
`WS_SIDEBAR_GAP` 常量删除。

