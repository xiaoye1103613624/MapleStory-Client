# 身份澄清与「大小不一样」解释

> 2026-11-12 live 取证。**13337 ≠ BeiDou**（用户已关闭 BeiDou；端口被新 IDB 占用）。

## 当前两端是谁

| 端口 | idb_path | module | input_md5 | 角色 |
|---|---|---|---|---|
| **13337** | `E:\MXD\反编译IDA\v83\083\v83.i64` | ``Angel.exe` | `cbe5ccba04cbe2760d8eabc537926bd1` | 本轮分析对象（`083/v83.i64`） |
| **13339** | `E:\MXD\反编译IDA\v83\`Angel.i64` | ``Angel.exe` | `cbe5ccba04cbe2760d8eabc537926bd1` | Angel IDA（`` `Angel.i64 ``） |

- 两端 **input_path 相同**：`C:\Users\Admin\Desktop\v83 Server\`Angel.exe`
- 两端 **SHA256 相同**：`1108f0f7ee551291273f5df98bc7f7b134b19516686dc7de15f1866b21c4683c`
- 两端 **input_file_size 相同**：9920512 bytes（≈9.46 MB）
- `survey_binary.image_size`：**两侧都是 `0xa95000`**（不是历史文档里 BeiDou 的 `0xa94000`）

## 与历史文档的关系

| 旧口径 | 现状 |
|---|---|
| 13337 = BeiDou.exe.i64 | **作废**。BeiDou 已关；13337 现为 `v83/083/v83.i64` |
| 13339 = Angel.i64 | **仍成立**（live 确认） |
| Angel vs BeiDou 差 0x1000 image_size | **不适用于本轮**。本轮两侧 image_size 相同 |

## 「大小不一样」指什么

1. **不是 exe 大小不同**：input MD5/SHA256/size 全等。
2. **不是 PE image_size 不同**：两侧 `0xa95000`。
3. **是 IDB 数据库文件大小不同**（分析库落盘）：
   - `v83.i64` ≈ 28,053,268 B（2026-11-12 新建）
   - `` `Angel.i64 `` ≈ 27,998,358 B（2026-11-08）
   - 工作集 `.id0`：v83 ≈ 109 MB vs Angel ≈ 126 MB（打开时长/分析缓存不同）
4. **函数数几乎相同**：55015 vs 55016（差 1，IDA 边界分析噪声级）
5. **符号状态同级**：MSVC 修饰名 3122、类 399、类方法合计 2984、CUI* 类 77 — 两侧一致

## 结论（校准该用哪边）

- **同一份 Angel.exe 的两份 IDB**，不是「两个不同客户端」。
- 业务校准任选；习惯上 13339 `` `Angel.i64 `` 文档链更长，13337 `v83.i64` 是同符号新鲜实例。
- **不要**再拿历史 BeiDou stripped 结论套到当前 13337。
