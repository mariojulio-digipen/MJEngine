robocopy Graphics\Release Release /lev:1
robocopy Graphics Release *.glsl /lev:1
robocopy Graphics Release *.txt /lev:1
robocopy Graphics Release *.ttf /lev:1
robocopy Graphics\SDL2\SDL2_ttf-2.0.14\lib\x86 Release *.dll /lev:1
robocopy Graphics\SDL2\sdl2_image-2.0.1\lib\x86 Release *.dll /lev:1
xcopy Graphics\gl\bin\release\Win32\glew32.dll Release /y
xcopy Graphics\fbx\libfbxsdk.dll Release /y
xcopy Graphics\SDL2\lib\SDL2.dll Release /y
rmdir Release\ArtAssets /s /q
mkdir Release\ArtAssets
robocopy Graphics\ArtAssets Release\ArtAssets /e