# Web 管理页拆分：魔女森林 / 突破石 分开管理（2026-11-23）

> 用户要求「魔女森林和突破石 web 端分开管理」。把原来单个 `witchConfig` 页（3 个 tab）拆成两个独立菜单/页面。
> 后端 API、DB 表、协议均不变；只改前端路由、页面、locale。

## 改动清单（行号级/文件级）

| 项 | 改动 |
|---|---|
| 删除 | `src/views/game/witchConfig/`（旧单页 + 其 locale） |
| 新增页 1 | `src/views/game/witchForest/index.vue` — 奖项表格（普通/高级池筛选）+ 全局活动配置（消耗/十连/奇迹时间/轮换周期）+ reload 按钮 |
| 新增页 1 locale | `src/views/game/witchForest/locale/{zh-CN,en-US}.ts`（`menu.game.witchForest` + witchForest.* 词条） |
| 新增页 2 | `src/views/game/breakthroughStone/index.vue` — 档位表格（itemId/成功率/增量/生效时段）+ reload 按钮 |
| 新增页 2 locale | `src/views/game/breakthroughStone/locale/{zh-CN,en-US}.ts`（`menu.game.breakthroughStone` + breakthrough.* 词条） |
| 路由 | `src/router/routes/modules/game.ts`：原 `witchConfig` 一条 → `witchForest` + `breakthroughStone` 两条（admin/requiresAuth） |
| 全局 locale | `src/locale/{zh-CN,en-US}.ts`：import/spread 从 `witchConfig` 换成 `witchForest` + `breakthroughStone` |
| 复用未改 | `src/api/witchConfig.ts`（两页共用同一套后端 API；后端 `/witchConfig/v1/*` 不变） |

## 两页各自职责

- **魔女森林页**（菜单「魔女森林」）：
  - Tab「奖项配置」：列表/新增/编辑/删除奖项，按奖池（普通50/高级100）筛选，字段 itemId/数量/权重/评级(0-4)/生效时段/启用。
  - Tab「活动配置」：单行全局配置（普通池消耗/高级池消耗/十连次数/奇迹时间窗口/轮换周期/启用）+ 保存/删除。
  - 顶部状态：生效奖项数、配置是否加载、轮换周期；reload 按钮。
- **突破石页**（菜单「突破石」）：
  - 档位列表/新增/编辑/删除：itemId/成功率(0-100)/破功增量/生效时段/启用。
  - 顶部状态：生效档位数、活动配置是否加载、轮换周期；reload 按钮。
  - 提示：客户端使用第一个持有且生效的档位。

## 验证（实测）

- `npx vue-tsc --noEmit --skipLibCheck`：本功能（witchForest/breakthroughStone/locale/router）**0 错误**；总 3 个错误均为既有 `src/views/game/file/index.vue`（javascriptDefaults/ScriptTarget，本功能未触碰）。
- `npx vite build --config vite.config.prod.ts`：EXIT 0，两个新路由均已打包进 dist。
- dev server 8787 存活（vite dev 读源码，刷新即见两个独立菜单）。

## 说明

- 后端 reload 是全量快照重建（奖项+档位+配置一起），所以两页的 reload 按钮都走同一个 `POST /witchConfig/v1/reload`；各自只展示与自己相关的状态字段，互不干扰。
- 严格 DB 语义、时段轮换、时区等规则不变（见 [README-CONFIG-DB.md](README-CONFIG-DB.md)）。
