rem ***************************************************************************
rem *** Make sure you are using the Visual Studio Developer Command Prompt  ***
rem ***************************************************************************
cl /EHsc /I. /Iinclude TextureDemo.cpp Texture.cpp lib/SquareMesh.cpp opengl32.lib lib/*.lib /link /subsystem:console

