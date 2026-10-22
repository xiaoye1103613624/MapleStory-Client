# r09 · 登录链路协议与流程正确性（2026-10-22）

> 范围：登录 → 选世界 → 选频道 → 选角 → 新建/删除角色 → 进入游戏 的**协议与流程**对齐（不含坐标版式，见 r04–r08）。
> 参考端：`E:\MXD\BeiDou-ClientV17.7\BeiDou.exe`（REA/IDA attached，base 0x400000，sha256 `1198fa57…`）
> 服务端事实来源：`E:\project\BeiDou-Server_s9`（gms083，`org.gms.net.server.handlers.login.*`、 `util/PacketCreator.java`）
> 本轮原则：**只改有双向证据（服务端读写 + 客户端读写）支撑的点**；语义未证实的按钮保持 stub 并登记（见 §4）。

## 1. 修复清单

| # | 缺陷 | 根因 | 修复 | 文件 |
|---|---|---|---|---|
| 1 | **创建角色必然失败** | 服务端 `CreateCharHandler` 读 **11** 个字段（含 `hairColor`），客户端 `CreateCharPacket` 只写 **10** 个，且把「已含颜色」的 id 当 `hair` 发送 → 逐字段错位，命中 `isNewCharDefaultHair`/`isNewCharDefaultHairColor` 校验失败 | `CreateCharPacket` 增加 `hair_color`（写在 `hair` 之后、`skin` 之前）；捏人页改为发 `hair=(hairStyle/10)*10`、`hairColor=hairColors[i]-hair` | `Net/Packets/CharCreationPackets.h`、`IO/UITypes/UICommonCreation.cpp` |
| 2 | 服务端状态被丢弃 | `ServerStatusHandler` 只读 `short` 后丢弃，拥挤/满员无任何呈现 | 解析 0/1/2 转发 `UIWorldSelect::set_server_status()`：1 提示 `POPULATION_TOO_HIGH`（同世界只弹一次），2 额外阻止进入并再次提示 | `Net/Handlers/LoginHandlers.cpp`、`IO/UITypes/UIWorldSelect.{h,cpp}` |
| 3 | **取消等待后世界/选角被永久锁死** | `UILoginWait` 取消只 `deactivate()`（不能在按钮回调里 `remove(this)`），残留实例使「存在 Wait」门闩恒真；`UICharSelect::request_pending` 也无人复位 | `UIStateLogin::update()` 增加门闩看护：下一帧移除失效 Wait 并复位两个窗口门闩；世界侧门闩由「存在」改为「存在且 `is_active()`」 | `IO/UIStateLogin.cpp`、`IO/UITypes/UIWorldSelect.cpp` |
| 4 | 创建失败会插入空白角色 | `AddNewCharEntryHandler` 无条件 `skip(1)` 后解析条目；失败由 `DELETE_CHAR_RESPONSE(cid=0,state=9)` 走另一条分支 | 先读结果码，非 0 或无后续字节直接返回 | `Net/Handlers/LoginHandlers.cpp` |
| 5 | 角色列表被静默丢弃 | `CharlistHandler` 要求「存在且活动的 Wait」才处理；重连/补发场景下 Wait 已被看护移除 → 选角页永远出不来 | 移除该必要条件（保留 Wait 清理语义） | `Net/Handlers/LoginHandlers.cpp` |
| 6 | 自动登录下发的是槽位值 | 自动登录直接 `CharlistRequestPacket(world, channel)`；手工路径走的是 `get_worldbyid()` 槽位→真实世界 id 映射 | 自动登录同样走 `get_worldbyid()` | `IO/UITypes/UIWorldSelect.cpp` |

## 2. 关键证据

**建角字段顺序**（服务端 `CreateCharHandler.java`）：
```
name(String) → job(int) → face(int) → hair(int) → hairColor(int) → skinColor(int)
→ top(int) → bottom(int) → shoes(int) → weapon(int) → gender(byte)
```
校验（`constants/inventory/ItemConstants.java`）：
- `isNewCharDefaultHair`：男 30000/30020/30030，女 31000/31040/31050 ⇒ **hair 必须是不带颜色增量的基础发型**
- `isNewCharDefaultHairColor`：0/2/3/7 ⇒ **hairColor 是增量**，服务端最终 `hair + hairColor`
- `isNewCharDefaultFace`（20000–20002 / 21000–21002）、`isNewCharDefaultSkinColor`（0–3）与客户端数据来源一致（`skinColors` 由 `WZ2_skin` 的 id−12000 得到 0–3）

**SERVERSTATUS**（`PacketCreator.getServerStatus` 只写 1 个 short；`ServerStatusRequestHandler` → `World.getWorldCapacityStatus()`，世界不存在回 2）：
0 正常 / 1 拥挤 / 2 满员。客户端 `UILoginNotice::Message::POPULATION_TOO_HIGH` 即对应文案。

**ADD_NEW_CHAR_ENTRY**（`PacketCreator.addNewCharEntry`）：`writeByte(0)` + 角色条目；创建失败一律走 `deleteCharResponse(0, 9)`（`CreateCharHandler` 的非法名/非法外观分支）。

**PIC**：`db/migration/V1.7.0__create_game_config.sql` 中 `enable_pic='false'`、`enable_pin='false'`（默认关闭），故选角走 `CharSelectedHandler`/`DeleteCharHandler`（后者仍读 `pic` 字符串，客户端 `DeleteCharPacket` 的 `("", cid)` 与之匹配）。

## 3. 验证

- `tools/build_smoke.bat`（Release|x64，OutDir = `E:\MXD\BeiDou-Client-xy\`）EXITCODE=0，exe 时间戳刷新。
- 自动登录回归（admin/admin）：`【1-Admin】` 进图成功，`_enter_game.log` 走到
  `UI State::GAME ok → transfer_player ok → fadeprocedure done locked=0`，无回归。
- **未完成的实机验证**：本机游戏窗口（含登录界面）截出来全黑，`CopyFromScreen` 与
  `PrintWindow(PW_RENDERFULLCONTENT)` 结果一致；把本轮改动 `git stash` 后重建的基线 exe 同样全黑
  ⇒ 属环境/渲染层既有问题，与本轮改动无关。建角端到端（需键盘输入角色名，SendKeys 对当前端无效）
  待该问题解决后由实机或用户确认。

## 4. 未完成 / 显式预留（未臆改）

| 项 | 状态 | 原因 / 下一步 |
|---|---|---|
| `BtViewChoice` / `BtViewAll` | 保持 stub | 参考端 exe 内搜不到 `BtViewChoice` 字面量（`search_strings` 0 命中），语义无证据；需定位 `CUIWorldSelect::OnButtonClicked` 对应分支 |
| `BtChangePIC` / `BtResetPIC` | stub（`show_pic_btns` 恒 false） | 依赖 PIC 体系，服务端默认关闭；v83 数据缺 `KeyConfig/KeyType`，无法构造输入界面 |
| `BtCharacter`（查看全部角色） | 空实现 | 需新增 `VIEW_ALL_CHAR(8)` 请求与 `showAllCharacter(Info)` 两个回包 handler + 新 UI，工程量独立 |
| opcode 4 `GENDER_DONE` / 5 `CONFIRM_EULA_RESULT` / 6 `CHECK_PINCODE` / 7 `UPDATE_PINCODE` | 未注册 handler | 仅在 opcode 名称表出现；北斗默认 `enable_pin=false` 且 EULA/性别流程不触发，暂不影响主链路 |
| 删除失败码细化 | 仅 10/20 有文案 | 服务端注释给出 0x16/0x18/0x1A/0x1D 等，但客户端 `UILoginNotice` 无对应枚举，需先补文案 |
| `LoginResultHandler` default 分支 | `int8_t(reason-1)` 回绕 | reason>128 会落到错误文案；待与服务端 reason 取值域核对后加固 |
| `ServerlistHandler` 重复下发 | `worlds` 累加未清理 | 重连场景会重复世界条目；需先确认是否由看护层统一清理 |
| `rank`/`jobrank`、`burning`、`use_timestamp` | 未呈现 | 需参考端选角面板版式证据（与 r08 的 Burning 文本重叠同一处） |
| `UIRegion` / `BtRegion` | 按钮不可达 + `set_region` 空 | 北斗单区；需先确认参考端是否有大区入口 |
| 请求超时 | 未加 | 现有 Cancel 按钮 + 门闩看护已可恢复；自动超时可能误伤慢速登录，需先定阈值依据 |

## 5. 下一轮建议

1. 先解决**渲染黑屏**（基线同样黑，属环境问题），否则登录链路无法留下实机截图证据。
2. 黑屏解决后优先端到端验证建角（本轮 #1/#4），这是唯一会导致「功能完全不可用」的协议缺陷。
3. 再按 §4 表格自顶向下补 handler 与新 UI（VIEW_ALL_CHAR 需独立一轮）。
