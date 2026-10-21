<#
.SYNOPSIS
    Cross-compile the MapleStory-Client native library (libmain.so) for Android
    using the NDK, after tools/android/build_android_deps.ps1 has produced the
    prebuilt dependency tree under E:\Android\deps.

    The result is E:\project\MapleStory-Client\build-android\libmain.so
    (or build-android\<abi>\libmain.so depending on NDK output layout).
#>
[CmdletBinding()]
param(
    [string]$Abi = 'x86_64',
    [string]$BuildType = 'Release'
)

$ErrorActionPreference = 'Stop'
. "E:\Android\env.ps1"

$CMake   = "C:\Program Files\CMake\bin\cmake.exe"
if (-not (Test-Path $CMake)) { $CMake = 'cmake' }
$Ninja   = "C:\Users\11036\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe\ninja.exe"
if (-not (Test-Path $Ninja)) { $Ninja = 'ninja' }
$Toolchain = Join-Path $env:ANDROID_NDK 'build\cmake\android.toolchain.cmake'
$ClientRoot = 'E:\project\MapleStory-Client'
$BuildDir = Join-Path $ClientRoot 'build-android'
$LogFile = 'E:\Android\client_build.log'

function Log($m){ $t=Get-Date -Format 'HH:mm:ss'; $l="[$t] $m"; Add-Content -Path $LogFile -Value $l; Write-Host $l }

Log "=== build_android_client start (ABI=$Abi) ==="
if (-not (Test-Path $Toolchain)) { throw "NDK toolchain missing: $Toolchain" }

# ---- configure ----
$cfgArgs = @(
    "-S", $ClientRoot,
    "-B", $BuildDir,
    "-G", "Ninja",
    "-DCMAKE_MAKE_PROGRAM=$Ninja",
    "-DCMAKE_TOOLCHAIN_FILE=$Toolchain",
    "-DANDROID_ABI=$Abi",
    "-DANDROID_PLATFORM=android-24",
    "-DCMAKE_BUILD_TYPE=$BuildType"
)
Log "configure: cmake $($cfgArgs -join ' ')"
$prev = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
try { & $CMake @cfgArgs *> (Join-Path $ClientRoot 'cfg.log') }
finally { $ErrorActionPreference = $prev }
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed`n$(Get-Content (Join-Path $ClientRoot 'cfg.log') -Tail 40)" }
Log "configure done"

# ---- build ----
Log "build: cmake --build $BuildDir"
$ErrorActionPreference = 'Continue'
try { & $CMake --build $BuildDir *> (Join-Path $ClientRoot 'build.log') }
finally { $ErrorActionPreference = 'Stop' }
if ($LASTEXITCODE -ne 0) { throw "cmake build failed`n$(Get-Content (Join-Path $ClientRoot 'build.log') -Tail 50)" }
Log "build done"
$so = Get-ChildItem $BuildDir -Recurse -Filter 'libmain.so' -ErrorAction SilentlyContinue | Select-Object -First 1
if ($so) { Log "produced $($so.FullName)" } else { Log "WARNING: libmain.so not found under $BuildDir" }
Log "=== build_android_client complete ==="
