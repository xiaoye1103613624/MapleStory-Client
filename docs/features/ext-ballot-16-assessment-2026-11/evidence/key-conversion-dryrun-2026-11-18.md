# 转 key 流水线实跑证据（2026-11-18）

## 结论
阶段一流水线**核心产出环节首次实跑成功**——不再是只读验证。`batch_convert_key` 对 2 个标准 GMS img 完成转换 + verify，staging 产物回读可解析，闭环成立。

## 实测命令与结果
- 工具：`batch_convert_key`（orange-wz v1.157.49，HTTP 直连 10012）
- 源：`E:/MXD/扩展改动/动漫皮肤/Character/Shoes/{01076000,01076001}.img`（头 `73f86c77…` GMS 标准）
- 源 key = 目标 key = 128B GMS（`_gms_key_128.b64`）+ IV `TSPHKw==`（验证同规格转换正确性）
- outputDir：`E:/MXD/_ext_import_staging/2026-11-18/converted`（异于源根，符合硬门禁）
- verify:true, overwrite:true, parallelism:2

**返回**：`total:2, success:2, failure:0, verify:true, ok:true, elapsedMs:210`
| 文件 | message | fingerprint |
|---|---|---|
| 01076000.img | ok+verified | `101385414c261e9e7a42dda09dd342bf668e0dfd1c19364e6d37ff595defc52e` |
| 01076001.img | ok+verified | `0bc263226ac799db973dae07f2f6493b0462e1bd238faba2ed8c1d8082a40e73` |

**staging 产物**：size 380493 / 1443766（与源完全一致），SHA256 前16 `6b32ecd573a53141` / `d0389961a110217e`。

**回读闭环**：staging/01076000.img 重新 load_files + get_node_tree_json 均 `isError:false`，返回 23854 字符节点树（`walk1/0/...`），证明转 key 产物可解析、与原源同构。

## 意义
- SOP §3 流水线第 5 步（`batch_convert_key` + verify）已由文档断言升级为实跑验证。
- 第 7 步"原子替换回 live"仍未执行——需用户明确授权改 `E:\MXD\BeiDou-Client-xy\Data`，本轮未动 live 一个字节。
