@echo off
rem 原地重建 NoLifeNx.lib 内的 5 个 nlnx 对象（node/file/bitmap/audio/nx），
rem 使其针对当前 file_impl.hpp（含 wz 字段）重新编译，修复 img 后端 get_child
rem 因旧 _file_data 布局缺失 wz 分支而 AV 的问题。dmqd*.o（Sound/Music 等）保留。
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.29.30133 >nul 2>&1
pushd "%~dp0.."
set "OUT=includes\NoLifeNx\nlnx\obj_rebuild"
if not exist "%OUT%" mkdir "%OUT%"
cl /c /utf-8 /MD /GL /EHsc /std:c++17 /O2 /DNDEBUG /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING /I includes\NoLifeNx /I includes\NoLifeNx\nlnx "/Iincludes\NoLifeNx\nlnx\includes\lz4_v1_8_2_win64\include" "/Itools\vendor\NoLifeWzToNx-master\includes\zlib\include" "/Itools\vendor\NoLifeWzToNx-master\includes\libsquish" includes\NoLifeNx\nlnx\node.cpp includes\NoLifeNx\nlnx\file.cpp includes\NoLifeNx\nlnx\bitmap.cpp includes\NoLifeNx\nlnx\audio.cpp includes\NoLifeNx\nlnx\nx.cpp /Fo%OUT%\
if errorlevel 1 (echo CL_FAILED & popd & exit /b 1)
for %%o in (node file bitmap audio nx) do lib "includes\NoLifeNx\nlnx\x64\Release\NoLifeNx.lib" /remove:"includes\NoLifeNx\nlnx\obj_rebuild\%%o.obj"
if errorlevel 1 (echo LIBREMOVE_FAILED & popd & exit /b 1)
lib "includes\NoLifeNx\nlnx\x64\Release\NoLifeNx.lib" %OUT%\node.obj %OUT%\file.obj %OUT%\bitmap.obj %OUT%\audio.obj %OUT%\nx.obj
if errorlevel 1 (echo LIBAPPEND_FAILED & popd & exit /b 1)
echo NLNX_LIB_REBUILT
popd
exit /b 0
