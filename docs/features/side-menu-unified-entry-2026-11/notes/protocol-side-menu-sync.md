# SIDE_MENU_SYNC（S→C）

| 字段 | 值 |
|---|---|
| Opcode | `0x3743`（14147） |
| 名称 | `SIDE_MENU_SYNC` |
| 方向 | Server → Client |
| 客户端 | `SideMenuSyncHandler` |

## Wire

```
int32  revision          // 日志用；权威 revision 以 JSON 内字段为准
maple  json_utf8         // short length + UTF-8 bytes，完整 side_menu 文档
```

客户端：`SideMenuConfig::apply_remote_json` → 若 `revision` 更新则写 `side_menu.json` → `UISideIconBar::reload_config`。

## 推送时机（服务端待接）

- 角色进游戏（SetField / 登录完成）必推
- Web「发布」后对在线角色广播

## 阻塞

本仓库为客户端；服务端需在 s9/BeiDou `SendOpcode` 登记同号并实现打包。未接前用本地文件热重载验证。
