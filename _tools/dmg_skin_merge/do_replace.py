import shutil, os, datetime, sys

deploy = r'E:\MXD\BeiDou-Client-xy\Data\Effect\DamageSkin.img'
merged = r'E:\MXD\BeiDou-Client-xy\Data\Effect\DamageSkin.merged.img'

def log(m):
    print(m, flush=True)

log(f"deploy exists={os.path.exists(deploy)} size={os.path.getsize(deploy) if os.path.exists(deploy) else 'NA'}")
log(f"merged exists={os.path.exists(merged)} size={os.path.getsize(merged) if os.path.exists(merged) else 'NA'}")

try:
    ts = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
    bak = deploy + '.bak_' + ts
    shutil.copy2(deploy, bak)
    log(f"backup created: {bak} size={os.path.getsize(bak)}")
    shutil.copy2(merged, deploy)
    log(f"deploy replaced: size={os.path.getsize(deploy)}")
    # sanity: deploy should now equal merged size
    if os.path.getsize(deploy) == os.path.getsize(merged):
        log("REPLACE OK: deploy == merged size")
    else:
        log("REPLACE MISMATCH")
except Exception as e:
    log(f"ERROR: {e}")
    sys.exit(1)
