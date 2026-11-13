# layout.py spec 模板（Wave 1+）

用法（仓库根）：

```text
python docs/features/ui-v83-window-restore/tools/layout.py check \
  docs/features/pixel-ui-angel-restore-2026-11/evidence/layout-specs/<Window>.json \
  docs/features/pixel-ui-angel-restore-2026-11/evidence/screens/<shot>.png
```

模板字段与 `ui-v83-window-restore/evidence/r27-oracle/*.json` 同源：`anchor_tmpl` + `probes` + `tol`。

本目录提供登录链骨架；`oracle` 需用正确端帧 `mark` 后再严判 `tol: 1`。
