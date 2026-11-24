@echo off
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
set "OUT=E:\project\MapleStory-Client\build-weather-time-hud-2026-11"
if not exist "%OUT%" mkdir "%OUT%"
"%MSBUILD%" "%~dp0MapleStory.sln" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142 /p:OutDir=%OUT%\ /m:1 /nologo /v:minimal /flp:logfile="%~dp0build_weather_time_hud.log" 2>&1
echo BUILD_EXITCODE=%ERRORLEVEL%
