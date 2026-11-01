# UI 版式取证小工具（从 `E:/project` 临时目录归档而来）

配合 `tools/nx_inspect.py`（只读 IMG/NX）使用。全部纯本地、无副作用。

| 脚本 | 用途 | 典型调用 |
| --- | --- | --- |
| `probe.py` | ① 目标色在全帧的像素数与 bbox（`find`）② 区域颜色直方图 top16（`probe`） | `python probe.py _now2.png find 103 103 103 3`<br>`python probe.py _now2.png probe 104 8 142 22` |
| `runs.py` | 逐行找某色的最长水平段 → **定位条带几何**（比目测靠谱） | `python runs.py _now2.png 103 103 103 0 200` |
| `bright.py` | 目录下所有 PNG 的逐通道 max 与 >200 像素数 → **抓帧可信度自检** | `python bright.py` |
| `cmp.py` | `crop(src,box,out,scale)` 裁切放大、`stack([(src,box)…],out,gap,scale)` 上下对照拼装；内置 `REF`/`CUR` 指向桌面两组对照图 | `from cmp import stack, REF, CUR` |
| `layout.py` | **R27 版式门禁**：把「原生帧量到的坐标」定成 oracle，客户帧逐格对账，Δ>tol 判未修复（非 0 退出，可挂构建） | `python layout.py bias <spec>`（**建 spec 后第一件事**：审锚点，紧裁帧的 dy 会被「模板必须装得下」强制成 0）<br>`python layout.py mark <spec> <原生帧> [--anchor X Y]`<br>`python layout.py check <spec> <客户帧> --png OUT [--mode M]`<br>`python layout.py selftest <spec>` |
| `scratch_sweep.py` | 清本机 `E:/project` 根目录的 `_` 临时件：按「是否仍被引用」三档分类，删后再反查有无引用断链 | `python tools/scratch_sweep.py classify`<br>`python tools/scratch_sweep.py verify <dropped.txt>` |
| `live/` | 真机自证驱动：启动→登录→进世界→开窗→展开→抓帧 | 见 [live/README.md](live/README.md)；口令走 `R27_USER`/`R27_PASS`，不入库 |
| `measure/` | 字体/墨迹量具：字模宽高、单词宽、探针落位实色 | `python measure/font_probe.py` 等，原始读数在 [../evidence/w0-common/](../evidence/w0-common/) |

`layout.py` 的 spec / 模板 / oracle 都在 [../evidence/r27-oracle/](../evidence/r27-oracle/README.md)，
其中一条规矩单列：**oracle 只能由 `mark` 量出来，手写 oracle = 把现有错误常数供认为标准，整轮对账作废**。
**锚点同样是量出来的，也会被量错**：用户给的原生截图多为紧裁，帧高 == 图高时自动锚点的 dy 被
「模板必须装得下」强制成 0 ⇒ 整张 spec 齐偏 1px（实测 Item 中过一次，三窗同时 Δ=1 差点去改公共件）。
所以每张 spec 建好先 `bias`，看记录锚点是不是 meanSAD 最优格，不是就 `mark --anchor` 校正重写。

## 两条硬规矩（都是踩出来的）

1. **判颜色前先跑 `bright.py`**。PrintWindow 抓帧可能整帧被压暗（实测见过 max=197 的帧），
   这种帧只能用于版式/几何。见 [../notes/open-issues.md §B5](../notes/open-issues.md)。
2. **怀疑「素材颜色不对」时比对像素计数，不要目测**：抓帧区域直方图 vs `nx_inspect image` 导出的
   素材直方图，各色**像素个数逐个对上、只有通道值同乘一个常数** ⇒ 不透明度被污染，不是素材差异。
   实例：[../evidence/w11-map/pixels-alpha.md](../evidence/w11-map/pixels-alpha.md)。

## Windows 侧注意

- 本机 stdout 是 **cp936**：`nx_inspect list` 打中文会糊 → 前面加 `PYTHONIOENCODING=utf-8`，
  或重定向到文件再读。
- 不要用 `exec(open(...).read())` 载入这些 UTF-8 脚本（默认按 gbk 解码会抛
  `UnicodeDecodeError`）→ `sys.path.insert(0, <dir>)` 后 `from cmp import stack`。
- 放大一律 `Image.NEAREST`；往不透明底上贴图前先 `alpha_composite`，否则透明区变黑。
