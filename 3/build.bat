@echo off

REM cl -MT -nologo -Gm- -GR- -EHa- -O2 -Oi -WX -W4 -wd4201 -wd4100 -wd4189 ..\..\src\windows\win_platform.cpp /Ferun_around_game.exe /link /OPT:REF /SUBSYSTEM:WINDOWS user32.lib gdi32.lib opengl32.lib
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -FC -Z7  main.cpp /Feadvent.exe /link /OPT:REF /SUBSYSTEM:CONSOLE 

popd

