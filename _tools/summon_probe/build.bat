@echo off
call "D:\software\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\project\MapleStory-Client
cl /nologo /std:c++17 /utf-8 /O2 /EHsc /MD ^
  /I includes\NoLifeNx ^
  /I includes\glew-2.1.0\include\GL ^
  /I includes\freetype\include ^
  /I includes\stb ^
  /I includes\NoLifeNx\nlnx\includes\lz4_v1_8_2_win64\include ^
  /I tools\vendor\NoLifeWzToNx-master\includes\zlib\include ^
  /I tools\vendor\NoLifeWzToNx-master\includes\libsquish ^
  _tools\summon_probe\main.cpp ^
  includes\NoLifeNx\nlnx\nx.cpp ^
  includes\NoLifeNx\nlnx\node.cpp ^
  includes\NoLifeNx\nlnx\file.cpp ^
  includes\NoLifeNx\nlnx\bitmap.cpp ^
  includes\NoLifeNx\nlnx\audio.cpp ^
  Util\WzRuntime.cpp ^
  Util\WzFiles.cpp ^
  Util\Resource\Keyring.cpp ^
  _tools\summon_probe\probe_stubs.cpp ^
  tools\vendor\NoLifeWzToNx-master\Keys.cpp ^
  /link /LIBPATH:includes\NoLifeNx\nlnx\includes\lz4_v1_8_2_win64\dll ^
  /LIBPATH:tools\vendor\NoLifeWzToNx-master\includes\zlib\lib ^
  /LIBPATH:tools\vendor\NoLifeWzToNx-master\includes\libsquish\win64\Release ^
  zlib.lib libsquish.lib liblz4.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib iphlpapi.lib winmm.lib ^
  /OUT:_tools\summon_probe\summon_probe.exe
