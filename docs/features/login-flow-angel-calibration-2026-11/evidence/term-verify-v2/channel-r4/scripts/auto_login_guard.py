#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R4 真机验证的 login.auto 字节级护栏（外部 runner 用，零改验证脚本）。

真源（逐字核对源码，不猜）：
  Configuration.cpp:381-385   xy-config.yaml 优先级 > Settings
  Configuration.cpp:793-796   get_auto_login() -> Setting<AutoLogin>::load()
  UIWorldSelect.cpp:419-439   auto=true 时构造期直接
                              CharlistRequestPacket(world0,ch0)，跳过频道页
  UILogin.cpp:370-377/:438-441 auto=true 预填配置账号并在首帧 login()
证据：ui_r4/auto_login_trap/（第一次 R4 真机 16 帧全抓到选角色页的全链留档）

设计：全量 PRECHECK 后再写盘；finally 字节级恢复并校验 sha；
护栏与抓帧脚本解耦，验证脚本一个字不改。
"""
import os
import json
import time
import hashlib

EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
CFG = os.path.join(EXE_DIR, "xy-config.yaml")
SETTINGS = os.path.join(EXE_DIR, "Settings")
EVIDENCE = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r4\auto_login_guard.json"

_BAK = []


def _sha(b):
    return hashlib.sha256(b).hexdigest()


def _flip(raw):
    """保证唯一一行 `auto:` 为 false；true→false / 已 false 幂等 / 其余零写盘报错。

    并发会话可能已把高优先级的 xy-config.yaml 翻成 false，而 Settings 仍 true，
    故不能写死「只认 true」。整行精确匹配，绝不误伤 `keymode: auto` 与注释。
    """
    ls = raw.splitlines(keepends=True)
    hits = [i for i, ln in enumerate(ls) if ln.strip() in
            (b"auto: true", b"auto:true", b"auto: false", b"auto:false")]
    if len(hits) != 1:
        raise RuntimeError("auto_login PRECHECK: expected exactly 1 'auto:' line, got %d" % len(hits))
    i = hits[0]
    ln = ls[i]
    stripped = ln.strip()
    if stripped in (b"auto: false", b"auto:false"):
        return raw, "already_false"
    e = b"\r\n" if ln.endswith(b"\r\n") else b"\n"
    prefix = ln[:len(ln) - len(stripped) - len(e)]
    ls[i] = prefix + b"auto: false" + e
    return b"".join(ls), "flipped_true_to_false"


def disable():
    """两文件全量 PRECHECK 通过后才写盘；任一异常零写盘。"""
    _BAK.clear()
    plans = []
    for p in (CFG, SETTINGS):
        raw = open(p, "rb").read()
        new, state = _flip(raw)
        plans.append((p, raw, new, state))
    rec = {"ts_disable": time.strftime("%Y-%m-%d %H:%M:%S"), "files": {}, "disabled": True, "restore_ok": None}
    for p, raw, new, state in plans:
        _BAK.append((p, raw))
        open(p, "wb").write(new)
        rec["files"][os.path.basename(p)] = {
            "state": state,
            "before_sha256": _sha(raw),
            "after_sha256": _sha(new),
            "head3_before": repr(raw[:3]),
            "head3_after": repr(new[:3]),
        }
    json.dump(rec, open(EVIDENCE, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    return rec


def restore():
    """无条件字节写回 + sha 比对；禁用未发生时不谎报成功。"""
    if not _BAK and not os.path.isfile(EVIDENCE):
        return False
    rec = json.load(open(EVIDENCE, encoding="utf-8")) if os.path.isfile(EVIDENCE) else {"files": {}}
    ok = bool(_BAK)
    for p, raw in _BAK:
        open(p, "wb").write(raw)
        same = open(p, "rb").read() == raw
        ok = ok and same
        d = rec["files"].setdefault(os.path.basename(p), {})
        d["restored_sha256"] = _sha(raw)
        d["restored_byte_exact"] = same
        d["head3_final"] = repr(raw[:3])
    rec["restore_ok"] = ok
    rec["ts_restore"] = time.strftime("%Y-%m-%d %H:%M:%S")
    json.dump(rec, open(EVIDENCE, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    return ok
