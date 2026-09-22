@echo off
rem ============================================================================
rem  编译 MapleStory.sln（Release / x64）
rem
rem  用法：tools\build_smoke.bat
rem  说明：
rem    - 脚本以**自身所在目录**推导仓库根，因此可从任意工作目录调用，
rem      不再依赖硬编码的绝对路径。
rem    - 构建日志固定写到仓库根 build_log.txt（已在 .gitignore 中忽略）。
rem    - PlatformToolset 显式固定为 v142，与本仓库既有可用构建保持一致；
rem      如需改用项目默认工具集，删掉该参数即可。
rem  退出：打印 EXITCODE=<MSBuild 退出码>，0 表示成功。
rem ============================================================================
setlocal

pushd "%~dp0.."
set "ROOT=%CD%"
set "LOG=%ROOT%\build_log.txt"

rem 优先使用本机已验证可用的 MSBuild；缺失时再用 vswhere 兜底查找
rem （vswhere 随 VS2017+ 安装器提供，覆盖常规安装与 BuildTools）。
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

"%MSBUILD%" "%ROOT%\MapleStory.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /m /nologo /v:minimal > "%LOG%" 2>&1
echo EXITCODE=%ERRORLEVEL%

popd
