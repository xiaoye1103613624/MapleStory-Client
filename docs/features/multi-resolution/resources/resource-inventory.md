# 资源清单（multi-resolution）

| 资源 | 类型 | 状态 | 来源 / 处理 | 运行时消费方 |
| --- | --- | --- | --- | --- |
| `E:\MXD\扩展改动\083多分辨率\分辨率.png` | 参考截帧 299×396 | 用户提供，未修改 | 含「窗口大小」行与蓝色高亮条，同时含全部运行时控件 | 仅作视觉基准，不直接加载 |
| `ui/uisysopt/v1/reference_分辨率.png` | 参考截帧存档 | 新增 | 原图 1:1 归档 | — |
| `UI/SysOpt/sysopt_backgrnd.png` | 运行时纯底板 299×396 | 新增 | `tools/resolution/make_sysopt_bg.py`：前 365 行取运行时 `UIWindow.img/SysOpt/backgrnd` 纯底板，新增 31 行取参考图浅灰背景条平铺，底边补 1px 黑框；不烘焙蓝色选中态/控件 | `UISysOpt` 经 `LocalPngTexture::load`，atlas id `0x5359534Full` |
| `ui/uisysopt/v1/sysopt_backgrnd.png` | 生成图存档 | 新增 | 与上同文件副本 | — |
| `ui/uisysopt/v1/manifest.md` | 资源说明 | 新增 | 记录来源、像素处理、1x、atlas id、回退策略，以及「未出 PSD 分层」的规范例外与用户决策 | — |
| `patches/make_sysopt_bg.log` | 生成日志 | 新增 | `ref_vs_base_diff_px(>30)=59251 ratio=0.5429`，说明为何不能直接贴参考图 | — |
| `locale/zh-CN.ini` | i18n | 修改 | 追加 `sysopt.label.resolution` + `sysopt.combo.resolution.0..8` 共 10 键 | `UISysOpt::draw_resolution_combo/list` |
| `locale/en-US.ini` | i18n | 修改 | 同上英文文案 `800 x 600` … `2560 x 1440` | 同上 |
| `xy-config.example.yaml` | 配置样例 | 修改 | 追加 `window.game_*` 示例与 9 档注释 | `Configuration` |
| `E:\MXD\BeiDou-Client-xy\xy-config.yaml` | 部署配置 | 仅加注释 | 刻意**不写死** `game_*`：部署 yaml 优先级高于 `Settings`，写死会覆盖用户保存的档位 | 配置层 |
| `E:\MXD\BeiDou-Client-xy\Settings` | 用户设置 | 运行时生成 | `game_tier=5` / `1280×720`；验证期由矩阵脚本逐档改写，收尾已恢复 | `Configuration` |

## 未改动的运行时 img

`UIWindow.img/SysOpt/backgrnd` 仍是运行时 299×365 原底板，未改任何 img 二进制。
新 299×396 底板通过本地 PNG 通道独立加载；PNG 缺失时 `UISysOpt` 回退到
`Texture(window("SysOpt")["backgrnd"])`，`dimension.y()` 仍保持 396。

## 未产出的资源（显式决策）

- PSD / 分层工程：用户明确「新 ui 图片用它就行了」，按 `xiaoye-MapleStory-dev` 规范例外处理，
  在 `ui/uisysopt/v1/manifest.md` 记录原因，改为用脚本做逐像素控件区剥离，可复跑校验。
- 新 IMG 节点：IMG 全库不存在分辨率选择节点（原生 v83 也没有，插件走 `Custom.wz` 覆盖），
  因此不新增 `UIWindow.img` 节点，避免改动运行时数据。
