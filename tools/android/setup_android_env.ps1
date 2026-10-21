<#
.SYNOPSIS
    One-shot installer for the MapleStory-Client Android build toolchain.
    Installs everything under E:\Android (does NOT touch C:). Idempotent.

    Produces:
      E:\Android\jdk17                       (Temurin JDK 17)
      E:\Android\android-sdk                  (SDK root: platform-tools, emulator, build-tools, ndk, system-images)
      E:\Android\deps                        (placeholder for cross-built libs, filled by build_android_deps.ps1)

    After this runs, these env vars are valid for the rest of the session:
      $env:JAVA_HOME, $env:ANDROID_HOME, $env:ANDROID_NDK, $env:ANDROID_SDK_ROOT
#>

$ErrorActionPreference = 'Stop'

$AndroidRoot   = 'E:\Android'
$SdkRoot       = Join-Path $AndroidRoot 'android-sdk'
$JdkDir        = Join-Path $AndroidRoot 'jdk17'
$CmdlineZip    = Join-Path $AndroidRoot 'cmdline-tools-win.zip'
$WorkDir       = Join-Path $AndroidRoot 'work'
$LogFile       = Join-Path $AndroidRoot 'setup_android_env.log'

# Pinned package versions (adjust if a version is unavailable; sdkmanager --list shows options).
$CmdlineVersion   = '11076708'          # commandlinetools-win-11076708_latest.zip
$NdkVersion       = '27.0.12077973'     # r27
$BuildToolsVersion= '34.0.0'
$PlatformApi      = '24'                # android-24 (matches android/CMakeLists.txt ANDROID_PLATFORM)
$SystemImage      = "system-images;android-$PlatformApi;default;x86_64"   # x86_64 for fast emulator on x86_64 host
# NOTE: x86_64 chosen for the AVD so it runs natively on this Windows x86_64 PC.
#       For a real arm64 device, pass -Abi arm64-v8a to build_android*.ps1.

function Log($msg) {
    $ts = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
    $line = "[$ts] $msg"
    Add-Content -Path $LogFile -Value $line
    Write-Host $line
}

function Ensure-Dir($p) {
    if (-not (Test-Path $p)) { New-Item -ItemType Directory -Path $p -Force | Out-Null }
}

function Download-File($url, $out) {
    Log "Downloading: $url"
    Log "  -> $out"
    $tmp = $out + '.part'
    if (Test-Path $tmp) { Remove-Item $tmp -Force }
    # Use curl.exe: Invoke-WebRequest stalls on large dl.google.com binaries in this
    # sandbox (verified ~10MB/s via curl vs. hang via Invoke-WebRequest).
    & curl.exe -L --retry 3 --retry-delay 2 -o $tmp $url
    if ($LASTEXITCODE -ne 0) { throw "curl failed ($LASTEXITCODE) for $url" }
    Move-Item -Path $tmp -Destination $out -Force
    if (-not (Test-Path $out) -or (Get-Item $out).Length -lt 1MB) {
        throw "Download failed or too small: $out"
    }
    Log "  downloaded $([math]::Round((Get-Item $out).Length/1MB,1)) MB"
}

# ---------------------------------------------------------------------------
Ensure-Dir $AndroidRoot
Ensure-Dir $WorkDir

Log "=== Android toolchain setup start (root=$AndroidRoot) ==="

# ---- 1. JDK 17 -------------------------------------------------------------
if (Test-Path (Join-Path $JdkDir 'bin\java.exe')) {
    Log "[1/6] JDK17 already present, skip."
} else {
    Log "[1/6] Installing Temurin JDK 17 ..."
    $jdkZip = Join-Path $WorkDir 'jdk17.zip'
    # Adoptium API returns a redirect to the actual asset; WebClient follows it.
    $jdkUrl = 'https://api.adoptium.net/v3/binary/latest/17/ga/windows/x64/jdk/hotspot/normal/eclipse'
    Download-File $jdkUrl $jdkZip
    # The zip has a single top-level folder like 'jdk-17.x.x+xx'. Extract to temp then flatten.
    $jdkTmp = Join-Path $WorkDir 'jdk17_tmp'
    Ensure-Dir $jdkTmp
    Expand-Archive -Path $jdkZip -DestinationPath $jdkTmp -Force
    $inner = Get-ChildItem $jdkTmp -Directory | Select-Object -First 1
    if (-not $inner) { throw 'JDK zip did not contain a top-level folder' }
    if (Test-Path $JdkDir) { Remove-Item $JdkDir -Recurse -Force }
    Move-Item -Path $inner.FullName -Destination $JdkDir -Force
    Remove-Item $jdkTmp -Recurse -Force
    Log "  JDK17 -> $JdkDir"
}

# ---- 2. Android cmdline-tools ---------------------------------------------
$Sdkmanager = Join-Path $SdkRoot 'cmdline-tools\latest\bin\sdkmanager.bat'
if (Test-Path $Sdkmanager) {
    Log "[2/6] cmdline-tools already present, skip."
} else {
    Log "[2/6] Installing Android cmdline-tools ..."
    Download-File "https://dl.google.com/android/repository/commandlinetools-win-$CmdlineVersion`_latest.zip" $CmdlineZip
    $ctTmp = Join-Path $WorkDir 'cmdline_tmp'
    Ensure-Dir $ctTmp
    Expand-Archive -Path $CmdlineZip -DestinationPath $ctTmp -Force
    # Move inner 'cmdline-tools' -> $SdkRoot/cmdline-tools/latest
    $ctInner = Join-Path $ctTmp 'cmdline-tools'
    $ctDest  = Join-Path $SdkRoot 'cmdline-tools\latest'
    Ensure-Dir $ctDest
    Get-ChildItem $ctInner | Move-Item -Destination $ctDest -Force
    Remove-Item $ctTmp -Recurse -Force
    Remove-Item $CmdlineZip -Force
    Log "  cmdline-tools -> $ctDest"
}

# ---- 3. SDK packages via sdkmanager ---------------------------------------
$env:JAVA_HOME = $JdkDir
$env:ANDROID_HOME = $SdkRoot
$env:ANDROID_SDK_ROOT = $SdkRoot
# sdkmanager reads java from JAVA_HOME

function SdkManager {
    param([string[]]$pkgs)
    # Pass the package list as a flat string array (one element per package) so
    # Start-Process -ArgumentList does not wrap it into a nested array.
    $p = Start-Process -FilePath $Sdkmanager -ArgumentList $pkgs `
        -NoNewWindow -Wait -PassThru -RedirectStandardOutput (Join-Path $WorkDir 'sdk_out.txt') `
        -RedirectStandardError (Join-Path $WorkDir 'sdk_err.txt')
    if ($p.ExitCode -ne 0) {
        $err = Get-Content (Join-Path $WorkDir 'sdk_err.txt') -Raw -ErrorAction SilentlyContinue
        throw "sdkmanager failed ($($p.ExitCode)): $err"
    }
}

Log "[3/6] Accepting SDK licenses ..."
# Pipe many 'y' lines (one per license prompt) so all licenses are accepted.
$yes = Join-Path $WorkDir 'yes.txt'
(1..50 | ForEach-Object { 'y' }) | Set-Content -Path $yes
$licProc = Start-Process -FilePath $Sdkmanager -ArgumentList '--licenses' -NoNewWindow -Wait -PassThru `
    -RedirectStandardInput $yes -RedirectStandardOutput (Join-Path $WorkDir 'lic_out.txt') `
    -RedirectStandardError (Join-Path $WorkDir 'lic_err.txt')
Log "  licenses exit=$($licProc.ExitCode)"

Log "[3/6] Installing SDK packages (this is the big download: platform-tools, emulator, platform, build-tools, ndk, system-image) ..."
$pkgs = @(
    'platform-tools'
    'emulator'
    "platforms;android-$PlatformApi"
    "build-tools;$BuildToolsVersion"
    "ndk;$NdkVersion"
    $SystemImage
)
SdkManager -pkgs $pkgs
Log "  SDK packages installed."

# ---- 4. NDK path ----------------------------------------------------------
$NdkDir = Join-Path $SdkRoot "ndk\$NdkVersion"
if (-not (Test-Path (Join-Path $NdkDir 'build/cmake/android.toolchain.cmake'))) {
    throw "NDK not found at expected path: $NdkDir"
}
$env:ANDROID_NDK = $NdkDir
Log "[4/6] NDK at $NdkDir"

# ---- 5. Self-check --------------------------------------------------------
Log "[5/6] Self-check ..."
# Use cmd /c so java's stderr (version goes to stderr) is captured as text and
# does NOT trigger PowerShell's NativeCommandError promotion under -ErrorAction Stop.
$javaCmd = '"{0}" -version 2>&1' -f (Join-Path $JdkDir 'bin\java.exe')
$javaOk = cmd /c $javaCmd
$adb = Join-Path $SdkRoot 'platform-tools\adb.exe'
$emu = Join-Path $SdkRoot 'emulator\emulator.exe'
$clang = Join-Path $NdkDir 'toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe'
$fail = $false
if (-not ($javaOk -match '17\.')) { Log "  [FAIL] java version not 17"; $fail=$true } else { Log "  [OK] java 17" }
if (-not (Test-Path $adb)) { Log '  [FAIL] adb missing'; $fail=$true } else { Log '  [OK] adb' }
if (-not (Test-Path $emu)) { Log '  [FAIL] emulator missing'; $fail=$true } else { Log '  [OK] emulator' }
if (-not (Test-Path $clang)) { Log '  [FAIL] ndk clang missing'; $fail=$true } else { Log '  [OK] ndk clang' }

if ($fail) { throw 'Self-check failed. See log above / ' + $LogFile }

# ---- 6. Write env helper --------------------------------------------------
Log "[6/6] Writing env helper script ..."
$envHelper = Join-Path $AndroidRoot 'env.ps1'
@"
# Source this in any PowerShell session to use the Android toolchain:
#   . E:\Android\env.ps1
`$env:JAVA_HOME        = '$JdkDir'
`$env:ANDROID_HOME     = '$SdkRoot'
`$env:ANDROID_SDK_ROOT = '$SdkRoot'
`$env:ANDROID_NDK      = '$NdkDir'
`$env:PATH             = "$(Join-Path $JdkDir 'bin');$(Join-Path $SdkRoot 'platform-tools');$(Join-Path $SdkRoot 'cmdline-tools\latest\bin');`$env:PATH"
Write-Host 'Android toolchain env loaded from E:\Android'
"@ | Set-Content -Path $envHelper

Ensure-Dir (Join-Path $AndroidRoot 'deps')

Log "=== Setup complete. Load env with: . $envHelper ==="
Log "Next step: tools/android/build_android_deps.ps1"
