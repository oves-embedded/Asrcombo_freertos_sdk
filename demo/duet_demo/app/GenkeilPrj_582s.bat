@echo off
set curf=%~p0%
set curf=%curf:~0,-1%
:getfolder
FOR /F "tokens=1,* delims=\/" %%i in ("%curf%") do (
    if not "%%j"=="" (
        set curf=%%j
        goto getfolder
    )
)

setlocal EnableDelayedExpansion

set input_string=%cd%
set substring=demo\duet_demo\app
set root_path=!input_string:%substring%=!
if not %root_path%==%input_string% (
    set root_path=!root_path:~0,-%substring:str=0%!
    set result=%root_path%
	)
echo %input_string%
set demoName=duet_demo
set ic_type=5822t
echo Demo: !demoName!
echo Path: %result%
echo duet demo

..\..\..\build\tools\genkeil.exe  -demo duet_demo  -sdk_path !result! -ic_type 5822s

