# Dual/Evan asset sync result (2026-11-12)

## Pipeline
- Tool: `tools/_sync_dual_evan_assets.py`
- Key: GMS 128B (`tools/_gms_key_128.b64`) + IV `TSPHKw==`
- Children via `get_node_tree_json` (root `list_children` returns empty in this MCP build)
- Flow: `load_files` → tree → `create_img_file` → `copy_paste_nodes` → `save_as` → live replace

## Result
- `ok=131 fail=0` (`tools/_staging/wave2_assets/sync_results.json`)
- Client xy: Map prefixes 103050/1000301/900010/910350; Npc 1056000/1013000/101310x/1057001/1057003; Weapon 01342/01332/01372/01382; Dragon S9 set
- Server s9: Map/Npc XML + Eqp String (1342/1332/1372/1382) + npc scripts 1013100–1013104

## RaceSelect
- Classic 5-card labels + force classic when `Login.img/RaceSelect` exists (`LoginUiAssets::use_classic_race_select`)
- Modern fallback: enable `EVAN`/`DUALBLADE` in `class_isdisabled`
