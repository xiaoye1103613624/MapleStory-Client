@echo off
call "D:\software\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
if errorlevel 1 call "D:\software\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
cd /d E:\project\MapleStory-Client\.agents\worktrees\channel-20page-verify
set CL=/std:c++17 /FS /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
set MSB="D:\software\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
set OUT=C:\Users\11036\AppData\Local\Temp\term_verify\build_channel20
set INT=C:\Users\11036\AppData\Local\Temp\term_verify\build_channel20\obj
if not exist "%OUT%" mkdir "%OUT%"
if not exist "%INT%" mkdir "%INT%"
%MSB% MapleStory.sln /p:Configuration=Release /p:Platform=x64 /p:MultiprocessorCompilation=false /p:OutDir=%OUT%\ /p:IntDir=%INT%\ /p:PostBuildEventUseInBuild=false /v:minimal > C:\Users\11036\AppData\Local\Temp\term_verify\build_channel20.log 2>&1
echo BUILD_EXITCODE=%ERRORLEVEL%
