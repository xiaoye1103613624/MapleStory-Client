@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.29.30133 >nul 2>&1
pushd "%~dp0.."
set "LIB=includes\NoLifeNx\nlnx\x64\Release\NoLifeNx.lib"
for %%o in (node file bitmap audio nx) do (
  lib "%LIB%" /remove:"x64\obj-Release\%%o.obj"
  lib "%LIB%" /remove:"includes\NoLifeNx\nlnx\obj_rebuild\%%o.obj"
)
echo NLNX_LIB_CLEANED
popd
exit /b 0
