# r03 · 「商店」按钮闪退根因与修复（2026-11-12）

## 现象

点击「商店」按钮 → 客户端闪退 → 服务端日志 `SocketException: Connection reset` +
`NullPointerException: mapData is null`（后者是连接被客户端断开后的连带清理异常，非根因）。

## 根因（客户端，非服务端）

`UICashShop::draw()` 对 **空 `std::vector` 用 `operator[]`** 越界解引用 → 首帧 draw 即崩。

本部署（BeiDou-Client-xy，`.nx` 实测）：
- `CashShop.img/CSPromotionBanner` → **NOT FOUND**
- `CashShop.img/CSMVPBanner` → **NOT FOUND**

原构造里这两段是 `if (node)` 守卫，节点缺失则不填充 `promotion_sprites` / `mvp_gauge`；
但 `draw()` 第 265/267/268 行对 `promotion_sprites[promotion_index]` 与 `mvp_sprites[mvp_grade]` /
`mvp_gauge` 是**无条件索引**。空 vector 的 `operator[]` 返回越界引用 → UB → 崩溃。

> `mvp_sprites` 是定长数组（空 Sprite 绘制安全），本可不必改；但为一致也加了 `has_mvp` 开关。
> `sprites` / `preview_sprites` 空节点由 `Texture::is_valid()` 早退，不崩。

## 同类潜在越界（一并加固）

- `item_labels[item.label + 1]`（HEAD 已有历史代码）：商品 label 取未映射枚举值时会越界 → 加 `li < item_labels.size()` 守卫。
- `BtNext/BtPrev`：`promotion_sprites.size() - 1` 在空 vector 时无符号下溢 → 加 `has_promotion && !empty()` 早退。

## 修复清单（仅 `IO/UITypes/UICashShop.{h,cpp}`）

1. 构造期补 `has_promotion` / `has_mvp` 标志（节点存在才置 true）
2. `draw()`：`if (has_promotion && !empty())` / `if (has_mvp)` 守卫
3. `item_labels` 加边界检查
4. `BtNext/BtPrev` 加 `has_promotion` 守卫

**零协议改动**（git diff 无 Opcode/OutPacket/RecvOpcode 触及）。

## 构建验证

- 编译（v142 ClCompile）：0 error，仅历史 warning ✅
- 链接未验：`MapleStory.exe` 被占用进程锁定（3 实例，`taskkill /F` Access denied）
- 协议门禁 `gen_opcode_matrix.py --check` 报 4 行漂移（s2c 373/374/378 + meta count），**与本次修复无关**，属预存在基线漂移（详见 r04）

## 待办

- 用户关闭客户端后跑 `tools/build_smoke.bat` 链接 + 真机点「商店」验收
