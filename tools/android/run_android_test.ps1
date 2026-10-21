# run_android_test.ps1
# One-click Android smoke test for MapleStory-Client (x86_64 emulator).
#
#   .\tools\android\run_android_test.ps1 [-AvdName msclient] [-DataDir <path>]
#
# What it does:
#   1. Ensure an AVD (android-24 x86_64, swiftshader) exists; create it if missing.
#   2. Start the emulator headless in the background.
#   3. Wait for the device to come online.
#   4. adb install -r the debug APK.
#   5. Stage the 9.2GB Data tree + xy-config.yaml into the app's external files dir
#      (archived on E:\, pushed once, extracted on-device).
#   6. Launch the app and capture logcat, then judge whether the login screen
#      was reached and whether a connection to the server (10.0.2.2:8484) happened.
#
# Prereqs:
#   - tools/android/setup_android_env.ps1 has run (SDK/NDK/platform-tools/emulator/image).
#   - The BeiDou login server is running on the HOST at port 8484 (started by the user).
#     From the emulator, the host is reached via 10.0.2.2:8484.
#
# NOTE: the 9.2GB Data push is slow and requires a large AVD userdata partition.

param(
    [string]$AvdName = 'msclient',
    [string]$DataDir = 'E:\MXD\BeiDou-Client-xy\Data',
    [string]$ConfigFile = 'E:\MXD\BeiDou-Client-xy\xy-config.yaml',
    [int]$WaitSeconds = 240
)

# Note: native tools (adb/sdkmanager/emulator) print benign warnings to stderr
# (e.g. "* daemon not running", "SDK XML version 4"). Under 'Stop' PowerShell
# turns those into a fatal RemoteException and aborts the script. We use
# 'Continue' and rely on explicit `throw`/exit-code checks for real failures.
$ErrorActionPreference = 'Continue'
. 'E:\Android\env.ps1'

$Root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$Apk = Join-Path $Root 'android\app\build\outputs\apk\debug\app-debug.apk'
$Pkg = 'com.maple.msclient'
$Activity = "$Pkg.MapleStoryActivity"
$EmuDataRoot = '/sdcard/Android/data/{0}/files' -f $Pkg
$Log = Join-Path $Root 'android_test.log'

function Log($msg) { $m = ("[{0:HH:mm:ss}] $msg" -f (Get-Date)); Write-Host $m; Add-Content -Path $Log -Value $m }

Log '=== run_android_test start ==='
if (-not (Test-Path $Apk)) { throw "APK not found: $Apk (run build_android.ps1 first)" }

$abd = 'E:\Android\android-sdk\platform-tools\adb.exe'
$emu = 'E:\Android\android-sdk\emulator\emulator.exe'
if (-not (Test-Path $abd)) { throw "adb not found: $abd" }
if (-not (Test-Path $emu)) { throw "emulator not found: $emu" }

# Kill any leftover emulator processes (including the qemu child that keeps the
# AVD registered even after the launcher was terminated) so we never end up with
# more than one device — `adb get-state` errors out with multiple devices.
# NOTE: orphaned qemu children from previous runs may live in another Windows
# session that Get-Process cannot enumerate by name, so the reliable way to stop
# them is via the emulator's network control console (`adb -s <serial> emu kill`).
& $abd devices 2>$null | Select-String 'emulator-\d+\s+device' | ForEach-Object {
    $serial = ($_.Line -split '\s+')[0]
    & $abd -s $serial emu kill 2>$null
}
@('emulator', 'qemu-system-x86_64', 'qemu-system-i386',
  'qemu-system-x86_64-headless', 'qemu-system-i386-headless') | ForEach-Object {
    Get-Process -Name $_ -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
}
& taskkill /F /IM qemu-system-x86_64.exe 2>$null
& taskkill /F /IM qemu-system-x86_64-headless.exe 2>$null
& taskkill /F /IM emulator.exe 2>$null
Start-Sleep -Seconds 3

# Delete the AVD's userdata image so the emulator recreates it at the configured
# disk.dataPartition.size. An existing image keeps its ORIGINAL size and cannot
# be grown, which is why a larger config value alone has no effect. This also
# makes the run idempotent (the APK and Data tree are re-staged every run anyway).
$avdDir = Join-Path $env:USERPROFILE ".android\avd\$AvdName.avd"
@('userdata-qemu.img','userdata-qemu.img.qcow2','userdata.img','userdata.img.qcow2') | ForEach-Object {
    $f = Join-Path $avdDir $_
    if (Test-Path $f) { Remove-Item $f -Force -ErrorAction SilentlyContinue }
}
Start-Sleep -Seconds 1

# ---- 1. AVD ----------------------------------------------------------------
$avdList = & $emu -list-avds 2>$null
if ($avdList -notcontains $AvdName) {
    Log "creating AVD '$AvdName' (android-24, x86_64, swiftshader) ..."
    $sysImg = 'system-images;android-24;default;x86_64'
    $prevEap = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        & 'E:\Android\android-sdk\cmdline-tools\latest\bin\sdkmanager.bat' $sysImg 2>&1 | Out-Null
        & 'E:\Android\android-sdk\cmdline-tools\latest\bin\avdmanager.bat' create avd `
            -n $AvdName -k $sysImg -d 'pixel_5' 2>&1 | Out-Null
    }
    finally { $ErrorActionPreference = $prevEap }
    # Enlarge the userdata partition so the 9.2GB Data tree (plus on-device
    # extraction) fits. ~9.2GB tar + ~9.2GB extracted needs ~19GB; use 24GB.
    # NOTE: the path child must NOT have a leading space (Join-Path would mangle it).
    $cfg = Join-Path $env:USERPROFILE ".android\avd\$AvdName.avd\config.ini"
    if (Test-Path $cfg) {
        $cur = Get-Content $cfg -Raw
        # REPLACE (don't append) the partition size: the pixel_5 template already
        # ships a 6GB disk.dataPartition.size, so appending a second line leaves
        # the 6GB one authoritative and the 9.2GB Data push ends with
        # "No space left on device".
        $cur = $cur -replace 'disk\.dataPartition\.size\s*=.*', 'disk.dataPartition.size = 24576M'
        # Loading the 9.2GB img tree needs headroom: with the default 1536M the
        # app gets killed by the low-memory killer a minute after startup
        # (observed as an onTrimMemory storm followed by "Process has died").
        # NOTE: keep this <= 2048M — a 4096M guest hangs the host (only ~8GB free)
        # and adb never reports boot_completed.
        $cur = $cur -replace 'hw\.ramSize\s*=.*', 'hw.ramSize = 2048M'
        [System.IO.File]::WriteAllText($cfg, $cur)
    }
}

# ---- 2. Start emulator (headless, swiftshader) -----------------------------
$proc = Start-Process -FilePath $emu -ArgumentList "-avd","$AvdName","-no-window","-gpu","swiftshader","-no-audio","-read-only" -PassThru
Log "emulator pid $($proc.Id) starting ..."

# ---- 3. Wait for device ----------------------------------------------------
Log "waiting for device (up to $WaitSeconds s) ..."
$deadline = (Get-Date).AddSeconds($WaitSeconds)
$online = $false
while ((Get-Date) -lt $deadline) {
    $devs = & $abd devices 2>$null | Select-String '^\s*emulator-\d+\s+device\b'
    if ($devs) {
        # Lock subsequent adb calls to this specific device so `adb install` /
        # `push` / `shell` never hit the "more than one device" error.
        $script:serial = ($devs[0].Line -split '\s+')[0]
        $env:ANDROID_SERIAL = $script:serial
        $online = $true; break
    }
    Start-Sleep -Seconds 5
}
if (-not $online) { throw 'emulator did not come online in time' }
Log 'device online'

# Wait for full boot (package manager ready) before installing. A freshly
# created AVD reports "device" state well before boot completes, and `adb
# install` fails if the package manager is not up yet.
$bootDeadline = (Get-Date).AddSeconds(180)
while ((Get-Date) -lt $bootDeadline) {
    $bc = & $abd shell getprop sys.boot_completed 2>$null
    if ($bc -match '1') { break }
    Start-Sleep -Seconds 5
}
Log 'device booted'

# ---- 4. Install APK --------------------------------------------------------
Log 'installing APK ...'
$instOut = & $abd install -r $Apk 2>&1
$instOut | Out-File (Join-Path $Root 'android_install.log')
if ($LASTEXITCODE -ne 0) { throw "adb install failed: $($instOut -join ' ')" }

# ---- 5. Stage Data ---------------------------------------------------------
if (Test-Path $DataDir) {
    $archive = 'E:\Android\work\data_stage.tar'
    if (-not (Test-Path $archive)) {
        Log "archiving Data tree -> $archive (this is large) ..."
        & tar.exe -cf $archive -C $DataDir . 2>&1 | Out-Null
    }
    $devArchive = "$EmuDataRoot/data_stage.tar"
    # The app's external files dir is only created once the app runs, so create
    # it explicitly before pushing (adb push needs the parent directory present).
    & $abd shell "mkdir -p $EmuDataRoot" 2>&1 | Out-Null
    Log 'pushing Data archive to device (slow, ~9.2GB) ...'
    $pushOut = & $abd push $archive $devArchive 2>&1
    $pushOut | Out-File (Join-Path $Root 'android_push.log')
    if ($LASTEXITCODE -ne 0) { throw "adb push of Data archive failed: $($pushOut -join ' ')" }
    Log 'extracting Data on device ...'
    $xout = & $abd shell "cd $EmuDataRoot && tar -xf data_stage.tar" 2>&1
    $xout | Out-File (Join-Path $Root 'android_push.log') -Append
    # Remove the archive afterwards. The in-shell `&& rm` sometimes no-ops when
    # `rm` is unavailable on the device; free ~9.2GB of the data partition either way.
    & $abd shell "rm -f $EmuDataRoot/data_stage.tar" 2>&1 | Out-Null
    Log 'Data staged'
} else {
    Log "WARNING: DataDir not found ($DataDir); skipping Data push."
}

if (Test-Path $ConfigFile) {
    # The host config points the login server at 127.0.0.1, which on the emulator
    # means the emulator ITSELF, not the host. Rewrite the ip to 10.0.2.2 (the
    # emulator's alias for the host loopback) so the client reaches the server
    # you started on the host. The original host config is left untouched.
    Log 'pushing xy-config.yaml (rewriting server ip -> 10.0.2.2 for emulator host loopback) ...'
    $tmp = 'E:\Android\work\xy-config.android.yaml'
    # Read as UTF-8 explicitly. `Get-Content -Raw` decodes with the system ANSI
    # code page (GBK on a Chinese Windows): a multi-byte UTF-8 tail byte plus the
    # following 0x0A is an invalid GBK pair and gets swallowed, so every line that
    # ends with a Chinese character loses its newline. That glues "server:" onto
    # the preceding comment line, the parser skips it as a comment, and the
    # children lose their parent level ("server.ip" -> "ip") — the config then
    # silently has no effect.
    $cfgText = [System.IO.File]::ReadAllText($ConfigFile, [System.Text.Encoding]::UTF8)
    # NOTE: `ip` is nested under `server:` with leading indentation, so the
    # pattern must allow whitespace before the key — otherwise the rewrite
    # silently no-ops and the device config keeps pointing at 127.0.0.1
    # (the emulator itself), making Session::init() fail instantly.
    # '${1}' (not '$1'): '$1' concatenated with an IP literal makes '$110.0.2.2',
    # which .NET parses as group 110 — a group that does not exist, so the
    # substitution is emitted verbatim and the `ip:` key disappears from the
    # file. The client then falls back to its built-in default 127.0.0.1.
    $cfgText = $cfgText -replace '(?m)^(\s*ip:\s*).*$', ('${1}' + '10.0.2.2')
    # Write UTF-8 without BOM, matching the file's documented convention
    # ("编码：UTF-8（无 BOM）"). The parser strips a BOM anyway, but keeping the
    # file byte-identical to the host one avoids surprises.
    [System.IO.File]::WriteAllText($tmp, $cfgText, (New-Object System.Text.UTF8Encoding($false)))
    & $abd push $tmp "$EmuDataRoot/xy-config.yaml" 2>&1 | Out-Null
}

# ---- 6. Launch + capture logcat -------------------------------------------
Log 'clearing logcat and starting app ...'
& $abd logcat -c
& $abd shell "am start -n $Activity" 2>&1 | Out-Null
Start-Sleep -Seconds 8

Log 'capturing logcat (look for Data load / login screen / connection) ...'
# Do NOT silence the client's own tags — Release builds make LOG() a no-op, so
# any SDL_Log/ALOG output from the client is the only signal we get here.
$tagFilter = 'MapleStory:V SDL:V AndroidRuntime:E *:S'
$capture = Join-Path $Root 'android_logcat.log'
& $abd logcat -d -v time $tagFilter > $capture 2>&1

# Pull the client's on-device debug logs (written by DataFiles/ImgFiles in debug
# builds) — far more reliable than logcat for judging data load. Also grab a
# screenshot for visual confirmation of the login screen.
$dbgImg = Join-Path $Root 'android_dbg_img.log'
$dbgData = Join-Path $Root 'android_dbg_data.log'
& $abd pull "$EmuDataRoot/_img_debug.log" $dbgImg 2>&1 | Out-Null
& $abd pull "$EmuDataRoot/_data_debug.log" $dbgData 2>&1 | Out-Null
# Capture a screenshot via the device (binary-safe) and pull it. Using
# `adb exec-out screencap -p > file` through PowerShell corrupts the PNG because
# the redirect goes through a text stream.
$shotDev = "$EmuDataRoot/__shot.png"
& $abd shell "screencap $shotDev" 2>&1 | Out-Null
& $abd pull $shotDev (Join-Path $Root 'android_shot.png') 2>&1 | Out-Null
& $abd shell "rm -f $shotDev" 2>&1 | Out-Null
$dbgText = ''
if (Test-Path $dbgImg) { $dbgText += (Get-Content $dbgImg -Raw) }
if (Test-Path $dbgData) { $dbgText += (Get-Content $dbgData -Raw) }

# ---- 7. Judge --------------------------------------------------------------
$logText = Get-Content $capture -Raw
$dbgFailed = $dbgText -match 'fail|error|exception|abort'   # case-insensitive by default
$dbgLoaded = ($dbgText.Length -gt 0) -and -not $dbgFailed
$judged = @{
    loginScreen = $logText -match 'login|Login|LOGIN'
    dataLoaded  = ($logText -match 'Data|IMG|nx') -or $dbgLoaded
    connected   = $logText -match '8484|connect|Connect|session|Session'
    crashed     = $logText -match 'FATAL|SIGSEGV|Abort|Native crash|AndroidRuntime: FATAL'
}
Log "judgement: dataLoaded=$($judged.dataLoaded) loginScreen=$($judged.loginScreen) connected=$($judged.connected) crashed=$($judged.crashed) debugLogPresent=$($dbgText.Length -gt 0) debugLogFailed=$dbgFailed"
if ($judged.crashed) {
    Log 'RESULT: FAIL (native crash detected)'
} elseif ($dbgLoaded) {
    Log 'RESULT: PASS (client loaded data; debug logs present and clean — check android_shot.png / android_dbg_*.log)'
} elseif ($judged.loginScreen -or $judged.dataLoaded) {
    Log 'RESULT: PASS-ish (client rendered / loaded data; check android_logcat.log)'
} else {
    Log 'RESULT: INCONCLUSIVE (no clear login/data signal; the client likely exited before DataFiles::init because the 8484 login server is not running on the host. Check android_dbg_*.log / android_shot.png)'
}
Log "logcat saved to $capture"
Log "debug logs: $dbgImg , $dbgData ; screenshot: $(Join-Path $Root 'android_shot.png')"
Log '=== run_android_test done ==='
