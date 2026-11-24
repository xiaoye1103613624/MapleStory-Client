@echo off
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
set "OUT=E:\project\MapleStory-Client\build_qs_funckey_drag"
if not exist "%OUT%" mkdir "%OUT%"
"%MSBUILD%" "E:\project\MapleStory-Client\MapleStory.sln" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /p:OutDir=%OUT%\ /m:1 /nologo /v:minimal /flp:logfile="E:\project\MapleStory-Client\build_qs_funckey_drag.log" 2>&1
echo EXITCODE=%ERRORLEVEL%
