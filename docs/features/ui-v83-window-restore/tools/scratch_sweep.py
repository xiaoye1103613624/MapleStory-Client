# -*- coding: utf-8 -*-
"""本机临时件按「是否仍被引用」分类，可反复运行；两种模式。

    python scratch_sweep.py classify          # 三档：精确名引用 / 仅裸词干 / 无引用，写清单不删
    python scratch_sweep.py verify <dropped>  # 反查：文档/记忆里出现的引用是否还有实体（自检清单自身除外）

判据来源 = MapleStory-Client 工作树 + 项目/用户记忆 + 全局 skill；再对已判 KEEP 的文本脚本做
传递闭包（闭包只认精确文件名，避免「脚注引用脚本、脚本又引用图」被误删）。
目录不参与判定：本机 _ 目录里混着 IDA 数据库（_ida_beidou 174 MB）这类非临时件，删除须人工确认。
控制台是 cp936，故统计走 stdout、明细落 UTF-8 文件。
"""
import os
import re
import sys
import json

ROOT = os.path.normpath('E:/project')
CLIENT = os.path.join(ROOT, 'MapleStory-Client')
MEM = [r'C:/Users/11036/.qoder/projects/E--project-MapleStory-Client/memory',
       r'C:/Users/11036/.qoder/memory',
       r'C:/Users/11036/.agents/skills']
# 本工具自己的产物 + 上一轮清理的入库清单：若算进语料，等于「清单引用自己」，判据失真
SELF_OUT = ('_r27sweep_manifest.json', '_r27verify.json')
SKIP_DIR = {'build', 'build_check', '.git', 'node_modules', '.vs', 'x64', 'Debug', 'Release',
            '__pycache__', '.qoder', '.codegraph', 'out', 'includes',
            'scratch-sweep-20261101'}
EXT_TEXT = ('.py', '.ps1', '.bat', '.txt', '.json', '.md', '.cpp', '.h', '.yaml', '.c')
EXACT = re.compile(r'(?<![A-Za-z0-9_])_[A-Za-z0-9][A-Za-z0-9_.\-]*'
                   r'\.(?:png|txt|py|ps1|json|log|bat|cpp|csv|md|xml|ini|cfg|html|i64|n64|idb)'
                   r'(?![A-Za-z0-9_\-])')
BARE = re.compile(r'(?<![A-Za-z0-9_.\-/\\])_[A-Za-z][A-Za-z0-9_\-]{1,30}(?![A-Za-z0-9_\-])')
# 活体驱动链：文档一旦撤掉引用就自动降级，这里兜底只是避免正在用的工具被误判
FORCED = {'_drive.py', '_postkey.py', '_cap2.py', '_rclick.py', '_quit.py'}


def walk_corpus():
    for root in [CLIENT] + MEM:
        if not os.path.isdir(root):
            continue
        for dp, dn, fn in os.walk(root):
            dn[:] = [d for d in dn if d not in SKIP_DIR]
            for f in fn:
                if os.path.splitext(f)[1].lower() in EXT_TEXT and f not in SELF_OUT:
                    yield os.path.join(dp, f)


def scan(path, names, stems, ex, ba):
    try:
        if os.path.getsize(path) > 4 * 1024 * 1024:
            return
        with open(path, 'r', encoding='utf-8', errors='ignore') as f:
            t = f.read()
    except OSError:
        return
    for m in EXACT.finditer(t):
        if m.group(0) in names:
            ex.add(m.group(0))
    for m in BARE.finditer(t):
        s = m.group(0)
        if s in names:
            ex.add(s)
        ba.update(stems.get(s, ()))


def classify():
    entries = [e for e in os.listdir(ROOT)
               if e.startswith('_') and e not in SELF_OUT]
    names = set(entries)
    stems = {}
    for n in names:
        if os.path.isfile(os.path.join(ROOT, n)):
            stems.setdefault(os.path.splitext(n)[0], set()).add(n)

    exact, bare = set(), set()
    for p in walk_corpus():
        scan(p, names, stems, exact, bare)

    keep = set(exact) | FORCED
    for _ in range(4):
        prev = set(keep)
        for n in sorted(keep):
            p = os.path.join(ROOT, n)
            if os.path.isfile(p) and os.path.splitext(n)[1].lower() in EXT_TEXT:
                ex2, ba2 = set(), set()
                scan(p, names, stems, ex2, ba2)
                keep |= ex2
        if keep == prev:
            break

    dirs = sorted(e for e in entries if os.path.isdir(os.path.join(ROOT, e)))
    files = [e for e in entries if e not in set(dirs)]
    mb = lambda xs: round(sum(os.path.getsize(os.path.join(ROOT, x))
                              for x in xs if os.path.isfile(os.path.join(ROOT, x))) / 1048576, 1)
    drop = sorted(e for e in files if e not in keep and e not in bare)
    review = sorted(e for e in files if e not in keep and e in bare)
    out = {'files_total': len(files), 'keep': sorted(keep), 'review_bare_only': review,
           'drop': drop, 'dirs': dirs,
           'mb_keep': mb(sorted(keep)), 'mb_review': mb(review), 'mb_drop': mb(drop)}
    with open(os.path.join(ROOT, SELF_OUT[0]), 'w', encoding='utf-8') as f:
        json.dump(out, f, ensure_ascii=False, indent=1)
    for k in ('files_total', 'keep', 'review_bare_only', 'drop', 'dirs',
              'mb_keep', 'mb_review', 'mb_drop'):
        v = out[k]
        print('%-16s %s' % (k, len(v) if isinstance(v, list) else v))
    return out


def verify(dropped_path):
    """删除后自检：dropped 里的名字若仍被任何语料文件精确引用 => 判据失守，须回滚。"""
    drop = set(x.strip() for x in open(dropped_path, encoding='utf-8').read().splitlines() if x.strip())
    names = set(os.listdir(ROOT)) | drop
    stems = {}
    refs = {}
    for p in walk_corpus():
        ex, ba = set(), set()
        scan(p, names, stems, ex, ba)
        for n in ex:
            if n in drop:
                refs.setdefault(n, []).append(p)
    bad = sorted(refs)
    print('dropped=%d still_referenced=%d' % (len(drop), len(bad)))
    for n in bad:
        print('  REGRESSION %s <- %s' % (n, refs[n][0]))
    return bad


if __name__ == '__main__':
    mode = sys.argv[1] if len(sys.argv) > 1 else 'classify'
    if mode == 'classify':
        classify()
    elif mode == 'verify':
        sys.exit(1 if verify(sys.argv[2]) else 0)
    else:
        raise SystemExit('mode: classify | verify <dropped.txt>')
