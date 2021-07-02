@ECHO OFF

REM // build the ROM
call build %1


echo -------------------------------------------------------------
IF EXIST gabuilt.bin ( fc /b gabuilt.bin gaoriginal.bin
) ELSE echo gabuilt.bin does not exist, probably due to an assembly error

REM // clean up after us
IF EXIST ga.p del ga.p
IF EXIST ga.h del ga.h
IF EXIST gabuilt.bin del gabuilt.bin
IF EXIST gabuilt.prev.bin del gabuilt.prev.bin
IF EXIST ga.log ( IF "%1"=="-pe" del ga.log )

REM // if someone ran this from Windows Explorer, prevent the window from disappearing immediately
pause
