robocopy Graphics\Debug Debug /lev:1
robocopy Graphics Debug *.glsl /lev:1
robocopy Graphics Debug *.txt /lev:1
robocopy Graphics Debug *.ttf /lev:1
robocopy Graphics\SDL2\SDL2_ttf-2.0.14\lib\x86 Debug *.dll /lev:1
robocopy Graphics\SDL2\sdl2_image-2.0.1\lib\x86 Debug *.dll /lev:1
xcopy Graphics\gl\bin\release\Win32\glew32.dll Debug /y
xcopy Graphics\fbx\libfbxsdk.dll Debug /y
xcopy Graphics\SDL2\lib\SDL2.dll Debug /y
rmdir Debug\ArtAssets /s /q
mkdir Debug\ArtAssets
robocopy Graphics\ArtAssets Debug\ArtAssets /e