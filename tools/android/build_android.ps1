# build_android.ps1
# Package the MapleStory-Client into an Android debug APK for the x86_64 emulator.
#
#   .\tools\android\build_android.ps1 [-Abi x86_64] [-BuildType Release]
#
# Steps:
#   1. Cross-compile libmain.so (reuses build_android_client.ps1).
#   2. Copy libmain.so into the Gradle project's jniLibs/<abi>.
#   3. Make sure a Gradle 8.x (JDK 17) is available.
#   4. gradle assembleDebug  ->  app/build/outputs/apk/debug/app-debug.apk
#
# Requirements: tools/android/setup_android_env.ps1 must have run once
# (JDK17 + Android SDK + NDK + platform-tools under E:\Android).

param(
    [string]$Abi = 'x86_64',
    [string]$BuildType = 'Release'
)

$ErrorActionPreference = 'Stop'
. 'E:\Android\env.ps1'

$Root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)   # MapleStory-Client
$AndroidPrj = Join-Path $Root 'android'
$ClientBuild = Join-Path $Root 'build-android'
$ApkOut = Join-Path $AndroidPrj 'app\build\outputs\apk\debug'

function Log($msg) { Write-Host ("[{0:HH:mm:ss}] $msg" -f (Get-Date)) }

# ---- 1. Build libmain.so ---------------------------------------------------
Log "=== build_android: cross-compile libmain.so ($Abi / $BuildType) ==="
& "$PSScriptRoot\build_android_client.ps1" -Abi $Abi -BuildType $BuildType
if ($LASTEXITCODE -ne 0) { throw 'libmain.so build failed' }

$libmain = Join-Path $ClientBuild 'libmain.so'
if (-not (Test-Path $libmain)) { throw "libmain.so not found at $libmain" }

# ---- 2. Copy into jniLibs --------------------------------------------------
$jniDir = Join-Path $AndroidPrj "app\src\main\jniLibs\$Abi"
New-Item -ItemType Directory -Path $jniDir -Force | Out-Null
Copy-Item $libmain (Join-Path $jniDir 'libmain.so') -Force
Log "copied libmain.so -> $jniDir"

# ---- 3. Ensure Gradle ------------------------------------------------------
$gradleExe = $null
if (Get-Command gradle -ErrorAction SilentlyContinue) {
    $gradleExe = 'gradle'
} elseif (Test-Path 'E:\Android\gradle\bin\gradle.bat') {
    $gradleExe = 'E:\Android\gradle\bin\gradle.bat'
} else {
    # Download Gradle 8.9 distribution once.
    $GradleVer = '8.9'
    $GradleZip = 'E:\Android\gradle-8.9-bin.zip'
    $GradleHome = 'E:\Android\gradle-8.9'
    if (-not (Test-Path "$GradleHome\bin\gradle.bat")) {
        Log "downloading Gradle $GradleVer ..."
        $url = "https://services.gradle.org/distributions/gradle-$GradleVer-bin.zip"
        Invoke-WebRequest -Uri $url -OutFile $GradleZip -UseBasicParsing
        Expand-Archive -Path $GradleZip -DestinationPath 'E:\Android' -Force
    }
    $gradleExe = Join-Path $GradleHome 'bin\gradle.bat'
}
Log "using gradle: $gradleExe"

# ---- 4. Assemble debug APK ------------------------------------------------
Log "=== gradle assembleDebug ==="
Push-Location $AndroidPrj
try {
    & $gradleExe assembleDebug --console=plain
    if ($LASTEXITCODE -ne 0) { throw 'gradle assembleDebug failed' }
} finally { Pop-Location }

$apk = Join-Path $ApkOut 'app-debug.apk'
if (Test-Path $apk) {
    Log "APK produced: $apk ($([math]::Round((Get-Item $apk).Length/1MB, 1)) MB)"
} else {
    throw "APK not found after build: $apk"
}
Log '=== build_android complete ==='
