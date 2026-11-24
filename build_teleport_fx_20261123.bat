@echo off
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
set "OUT=E:\project\MapleStory-Client\build-teleport-fx-20261123"
if not exist "%OUT%" mkdir "%OUT%"
"%MSBUILD%" "%~dp0MapleStory.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /p:OutDir=%OUT%\\ /m:1 /nologo /v:minimal /flp:logfile="%~dp0build_teleport_fx_20261123.log" 2>&1
echo EXITCODE=%ERRORLEVEL%
