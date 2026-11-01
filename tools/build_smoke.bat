@echo off
rem ============================================================================
rem  Build MapleStory.sln (Release / x64) + protocol gate
rem
rem  Usage: tools\build_smoke.bat
rem  Notes:
rem    - The repo root is derived from this script's own location, so it can be
rem      called from any working directory (no hardcoded absolute path).
rem    - Build log always goes to <repo root>\build_log.txt (git-ignored).
rem    - PlatformToolset is pinned to v142, matching the toolchain verified to
rem      work in this repo; drop the parameter to use the project default.
rem    - Comments are ASCII on purpose: cmd.exe reads .bat with the OEM code
rem      page (cp936 here), and UTF-8 Chinese in a rem line gets decoded into
rem      byte soup that cmd then tries to *execute*. Keep this file ASCII.
rem  Exit: prints EXITCODE=<MSBuild exit code>; 0 means success.
rem        EXITCODE=2 means the protocol gate blocked the build.
rem ============================================================================
setlocal

pushd "%~dp0.."
set "ROOT=%CD%"
set "LOG=%ROOT%\build_log.txt"

rem Prefer the MSBuild verified on this machine, then fall back to vswhere
rem (ships with the VS2017+ installer, covers regular installs and BuildTools).
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%MSBUILD%" (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2^>nul`) do set "MSBUILD=%%i"
    )
)

if not exist "%MSBUILD%" (
    echo MSBUILD_NOT_FOUND
    popd
    exit /b 1
)

rem ---------------------------------------------------------------------------
rem Protocol gate: docs/features/s9-protocol-align/protocol/opcode-matrix.json
rem must still agree with both sources.  gen_opcode_matrix.py --check exit code:
rem   1 = protocol facts contradict the sources  -> block the build
rem   2 = matrix not generated yet               -> notify only, do not block
rem   0 = consistent (or only line citations moved)
rem A missing python is treated the same way as rc=2: the gate must never be
rem the reason a machine with a different setup cannot build.
rem ---------------------------------------------------------------------------
where python >nul 2>nul
if errorlevel 1 goto :gate_skip
python "%ROOT%\tools\gen_opcode_matrix.py" --check
set "RC=%ERRORLEVEL%"
if "%RC%"=="1" goto :gate_fail
if not "%RC%"=="0" goto :gate_skip
echo GATE OK
goto :build

:gate_skip
echo GATE SKIP: python or matrix unavailable - protocol gate not enforced this run
goto :build

:gate_fail
echo GATE FAIL: opcode matrix contradicts s9 / client sources
echo EXITCODE=2
popd
exit /b 2

:build
"%MSBUILD%" "%ROOT%\MapleStory.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /m /nologo /v:minimal > "%LOG%" 2>&1
echo EXITCODE=%ERRORLEVEL%

popd
