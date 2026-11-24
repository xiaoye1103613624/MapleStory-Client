-- SideMenu 管理后台草案（MySQL）。本客户端仓库不执行；供 admin 工程接入。

CREATE TABLE IF NOT EXISTS side_feature (
  id VARCHAR(64) PRIMARY KEY,
  title VARCHAR(128) NOT NULL,
  open_key VARCHAR(64) NOT NULL,
  enabled TINYINT(1) NOT NULL DEFAULT 1,
  sort_order INT NOT NULL DEFAULT 0,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS side_menu_item (
  id VARCHAR(64) PRIMARY KEY,
  parent_id VARCHAR(64) NULL,
  kind ENUM('folder','leaf') NOT NULL,
  title VARCHAR(128) NOT NULL,
  icon_key VARCHAR(64) NOT NULL DEFAULT 'Content',
  action ENUM('Builtin','NpcScript','ChatCmd') NOT NULL DEFAULT 'Builtin',
  arg VARCHAR(256) NOT NULL DEFAULT '',
  enabled TINYINT(1) NOT NULL DEFAULT 1,
  sort_order INT NOT NULL DEFAULT 0,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  KEY idx_parent_order (parent_id, sort_order)
);

CREATE TABLE IF NOT EXISTS side_menu_meta (
  id TINYINT PRIMARY KEY DEFAULT 1,
  revision BIGINT NOT NULL DEFAULT 1,
  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

INSERT INTO side_menu_meta (id, revision) VALUES (1, 1)
  ON DUPLICATE KEY UPDATE id=id;

-- Web API 草案：
-- GET  /api/side-menu          → 完整 JSON（features+items+revision）
-- PUT  /api/side-menu/features → 注册表
-- PUT  /api/side-menu/items    → 树（含 sort_order 拖拽）
-- POST /api/side-menu/publish  → revision++ 并广播 0x3743
