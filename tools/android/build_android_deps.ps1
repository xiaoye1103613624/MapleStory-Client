<#
.SYNOPSIS
    Cross-compile the native dependencies needed by MapleStory-Client for Android.
    Produces static libs in E:\Android\deps\<lib>\<abi>\{lib,include}.

    Requires: tools/android/setup_android_env.ps1 already run (E:\Android\env.ps1 present).

    Default ABI = x86_64 (matches the AVD system image). Pass -Abi arm64-v8a for devices.
#>
[CmdletBinding()]
param(
    [string[]]$Abi = @('x86_64')
)

$ErrorActionPreference = 'Stop'
. "E:\Android\env.ps1"   # loads JAVA_HOME / ANDROID_NDK / ANDROID_HOME / PATH

$CMake     = "C:\Program Files\CMake\bin\cmake.exe"
if (-not (Test-Path $CMake)) { $CMake = 'cmake' }
$Ninja     = "C:\Users\11036\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe\ninja.exe"
if (-not (Test-Path $Ninja)) { $Ninja = 'ninja' }
$Toolchain = Join-Path $env:ANDROID_NDK 'build\cmake\android.toolchain.cmake'
$Platform  = 'android-24'
$SrcRoot   = 'E:\Android\src'
$DepsRoot  = 'E:\Android\deps'
$BuildRoot = 'E:\Android\build'
$LogFile   = 'E:\Android\build_android_deps.log'
$ClientRoot= 'E:\project\MapleStory-Client'

function Log($m){ $t=Get-Date -Format 'HH:mm:ss'; $l="[$t] $m"; Add-Content -Path $LogFile -Value $l; Write-Host $l }

function Invoke-CmakeConfigure($src, $build, $extra) {
    $argList = [System.Collections.Generic.List[string]]::new()
    $argList.Add('-S'); $argList.Add($src)
    $argList.Add('-B'); $argList.Add($build)
    $argList.Add('-G'); $argList.Add('Ninja')
    $argList.Add("-DCMAKE_MAKE_PROGRAM=$Ninja")
    $argList.Add("-DCMAKE_TOOLCHAIN_FILE=$Toolchain")
    $argList.Add("-DANDROID_ABI=$Abi")
    $argList.Add("-DANDROID_PLATFORM=$Platform")
    $argList.Add('-DCMAKE_BUILD_TYPE=Release')
    foreach ($e in $extra) { $argList.Add($e) }
    Log "  configure: $src"
    $log = Join-Path $BuildRoot 'cmake_cfg.log'
    # PowerShell 5.1 promotes a native command's stderr (e.g. cmake warnings) into a
    # terminating NativeCommandError under -ErrorAction Stop. Temporarily relax the
    # preference for the call; real failures are still caught via $LASTEXITCODE.
    $prev = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try { & $CMake @argList *> $log }
    finally { $ErrorActionPreference = $prev }
    if ($LASTEXITCODE -ne 0) { throw "cmake configure failed for $src`n$(Get-Content $log -Tail 30)" }
}

function Invoke-CmakeBuildInstall($build, $install, $target='', [switch]$SkipInstall) {
    $log = Join-Path $BuildRoot 'cmake_build.log'
    $prev = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        if ($target) {
            & $CMake --build $build --target $target *> $log
        } else {
            & $CMake --build $build *> $log
        }
        if ($LASTEXITCODE -ne 0) { throw "cmake build failed: $build`n$(Get-Content $log -Tail 30)" }
        if ($SkipInstall) {
            Log "  (skipping cmake install for $build; artifacts copied by Normalize-Lib / already present)"
        } else {
            & $CMake --install $build --prefix $install *>> $log
            if ($LASTEXITCODE -ne 0) { throw "cmake install failed: $build`n$(Get-Content $log -Tail 30)" }
        }
    }
    finally { $ErrorActionPreference = $prev }
}

function Normalize-Lib($dir, $names) {
    foreach ($n in $names) {
        $f = Get-ChildItem -Path $dir -Recurse -Filter $n -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($f) {
            $dst = Join-Path $dir $n
            if ($f.FullName -ne (Resolve-Path $dst -ErrorAction SilentlyContinue)) {
                Copy-Item $f.FullName $dst -Force
            }
            return $n
        }
    }
    throw "static lib not found under $dir (looked for $($names -join ', '))"
}

Log "=== build_android_deps start (ABI=$($Abi -join ',')) ==="

# ---- 0. Fetch sources not bundled in the repo -------------------------------
$SdlSrc = Join-Path $SrcRoot 'SDL-release-3.2.4'
if (-not (Test-Path (Join-Path $SdlSrc 'CMakeLists.txt'))) { throw "SDL3 source missing at $SdlSrc" }

$FtSrc = Join-Path $SrcRoot 'freetype-2.13.3'
if (-not (Test-Path (Join-Path $FtSrc 'CMakeLists.txt'))) {
    $ftZip = Join-Path $SrcRoot 'freetype.tar.gz'
    if (-not (Test-Path $ftZip) -or ((Get-Item $ftZip).Length -lt 1MB)) {
        if (Test-Path $ftZip) { Remove-Item $ftZip -Force }
        Log 'downloading freetype source (Savannah mirror)'
        & curl.exe -L --retry 5 -o $ftZip 'https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.gz' 2>$null
    }
    tar -xzf $ftZip -C $SrcRoot 2>$null
    if (-not (Test-Path (Join-Path $FtSrc 'CMakeLists.txt'))) { throw 'freetype extract failed' }
}

$Lz4Src = Join-Path $SrcRoot 'lz4-1.10.0'
if (-not (Test-Path (Join-Path $Lz4Src 'lib\lz4.c'))) {
    $lzZip = Join-Path $SrcRoot 'lz4.tar.gz'
    if (-not (Test-Path $lzZip) -or ((Get-Item $lzZip).Length -lt 100KB)) {
        if (Test-Path $lzZip) { Remove-Item $lzZip -Force }
        Log 'downloading lz4 source (tarball)'
        & curl.exe -L --retry 5 -o $lzZip 'https://github.com/lz4/lz4/archive/refs/tags/v1.10.0.tar.gz' 2>$null
    }
    tar -xzf $lzZip -C $SrcRoot 2>$null
    if (-not (Test-Path (Join-Path $Lz4Src 'lib\lz4.c'))) { throw 'lz4 extract failed' }
}

foreach ($a in $Abi) {
    $Abi = $a
    Log "===== ABI $a ====="

    # ---- SDL3 (static) ----
    $inst = Join-Path $DepsRoot "sdl3\$a"
    Invoke-CmakeConfigure $SdlSrc (Join-Path $BuildRoot "sdl3-$a") @(
        '-DSDL_SHARED=OFF','-DSDL_STATIC=ON','-DSDL_TESTS=OFF',
        '-DSDL_TEST_LIBRARY=OFF','-DSDL_EXAMPLES=OFF','-DSDL_UNINSTALL=OFF'
    )
    # Only build the static C library. SDL3's CMake otherwise also compiles its
    # bundled Android Java wrapper (org/libsdl/app), which targets newer Android
    # APIs than our android-24 platform jar and fails javac. We copy that Java
    # layer into the Gradle project separately, so it is not needed here.
    Invoke-CmakeBuildInstall (Join-Path $BuildRoot "sdl3-$a") $inst 'SDL3-static' -SkipInstall
    # The static lib was built directly (Java skipped); copy it into the install lib dir.
    $sdlLib = [System.IO.Path]::Combine($BuildRoot, "sdl3-$a", 'libSDL3.a')
    if (-not (Test-Path $sdlLib)) { $sdlLib = [System.IO.Path]::Combine($BuildRoot, "sdl3-$a", 'lib', 'libSDL3.a') }
    $libDir = [System.IO.Path]::Combine($inst, 'lib')
    New-Item -ItemType Directory -Path $libDir -Force | Out-Null
    Copy-Item $sdlLib (Join-Path $libDir 'libSDL3.a') -Force
    Normalize-Lib $libDir @('libSDL3.a','libSDL3-static.a','SDL3.lib')

    # ---- freetype (static, minimal deps) ----
    $inst = Join-Path $DepsRoot "freetype\$a"
    Invoke-CmakeConfigure $FtSrc (Join-Path $BuildRoot "ft-$a") @(
        '-DBUILD_SHARED_LIBS=OFF','-DFT_DISABLE_ZLIB=ON','-DFT_DISABLE_BZIP2=ON',
        '-DFT_DISABLE_PNG=ON','-DFT_DISABLE_HARFBUZZ=ON','-DFT_DISABLE_BROTLI=ON'
    )
    Invoke-CmakeBuildInstall (Join-Path $BuildRoot "ft-$a") $inst
    Normalize-Lib (Join-Path $inst 'lib') @('libfreetype.a','freetype.lib')

    # ---- lz4 (static, custom target to avoid lz4's old bundled CMake) ----
    $Lz4CMake = Join-Path $BuildRoot "lz4-cmake-$a"
    if (-not (Test-Path $Lz4CMake)) { New-Item -ItemType Directory -Path $Lz4CMake -Force | Out-Null }
    $lz4SrcFs = $Lz4Src.Replace('\', '/')
    $lz4CMakeFile = Join-Path $Lz4CMake 'CMakeLists.txt'
    $lz4Txt = "cmake_minimum_required(VERSION 3.16)`r`n" +
        "project(lz4 C)`r`n" +
        "add_library(lz4 STATIC `"$lz4SrcFs/lib/lz4.c`" `"$lz4SrcFs/lib/lz4hc.c`" `"$lz4SrcFs/lib/xxhash.c`")`r`n" +
        "target_include_directories(lz4 PUBLIC `"$lz4SrcFs/lib`")`r`n" +
        "install(TARGETS lz4 ARCHIVE DESTINATION lib)`r`n"
    Set-Content -Path $lz4CMakeFile -Value $lz4Txt
    $inst = Join-Path $DepsRoot "lz4\$a"
    Invoke-CmakeConfigure $Lz4CMake (Join-Path $BuildRoot "lz4-$a") @()
    Invoke-CmakeBuildInstall (Join-Path $BuildRoot "lz4-$a") $inst
    Normalize-Lib (Join-Path $inst 'lib') @('liblz4.a','lz4.lib')

    # ---- libsquish (repo source, custom static lib) ----
    $SqSrc = Join-Path $ClientRoot 'tools\vendor\NoLifeWzToNx-master\includes\libsquish'
    $SqCMake = Join-Path $BuildRoot "squish-cmake-$a"
    if (-not (Test-Path $SqCMake)) { New-Item -ItemType Directory -Path $SqCMake -Force | Out-Null }
    $sqSrcFs = $SqSrc.Replace('\', '/')
    $sqCMakeFile = Join-Path $SqCMake 'CMakeLists.txt'
    $sqTxt = "cmake_minimum_required(VERSION 3.16)`r`n" +
        "project(squish CXX)`r`n" +
        "set(CMAKE_CXX_STANDARD 17)`r`n" +
        "file(GLOB SQ_SRC `"$sqSrcFs/*.cpp`")`r`n" +
        "add_library(squish STATIC $( '${SQ_SRC}' ))`r`n" +
        "target_include_directories(squish PUBLIC `"$sqSrcFs`")`r`n" +
        "install(TARGETS squish ARCHIVE DESTINATION lib)`r`n"
    Set-Content -Path $sqCMakeFile -Value $sqTxt
    $inst = Join-Path $DepsRoot "squish\$a"
    Invoke-CmakeConfigure $SqCMake (Join-Path $BuildRoot "squish-$a") @()
    Invoke-CmakeBuildInstall (Join-Path $BuildRoot "squish-$a") $inst
    Normalize-Lib (Join-Path $inst 'lib') @('libsquish.a','squish.lib')

    Log "===== ABI $a done ====="
}

Log '=== build_android_deps complete. Libs under $DepsRoot ==='
Get-ChildItem $DepsRoot -Recurse -Filter *.a | Select-Object FullName | Format-Table -AutoSize
