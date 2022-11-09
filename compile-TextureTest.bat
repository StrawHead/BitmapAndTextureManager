rem ***************************************************************************
rem *** Make sure you are using the Visual Studio Developer Command Prompt  ***
rem ***************************************************************************
cl /EHsc /I. /Iinclude TextureTest.cpp Texture.cpp lib/Affine.lib 

rem ***************************************************************************
rem     First, try:
rem         TextureTest
rem     and compare your output ('TextureTest.bmp') with 'TextureTest-default.bmp'
rem
rem     Second, try:
rem         TextureTest painted.bmp
rem     and compare your output with 'TextureTest-nondefault.bmp'
