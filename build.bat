@ECHO OFF

REM // make sure we can write to the file gabuilt.bin
REM // also make a backup to gabuilt.prev.bin
IF NOT EXIST gabuilt.bin goto LABLNOCOPY
IF EXIST gabuilt.prev.bin del gabuilt.prev.bin
IF EXIST gabuilt.prev.bin goto LABLNOCOPY
move /Y gabuilt.bin gabuilt.prev.bin
IF EXIST gabuilt.bin goto LABLERROR3
:LABLNOCOPY

REM // delete some intermediate assembler output just in case
IF EXIST ga.p del ga.p
IF EXIST ga.p goto LABLERROR2
IF EXIST ga.h del ga.h
IF EXIST ga.h goto LABLERROR1

REM // clear the output window
REM cls

REM // run the assembler
REM // -xx shows the most detailed error output
REM // -E creates log file for errors/warnings
REM // -A gives us a small speedup
set AS_MSGPATH=AS/win32
set USEANSI=n

REM // allow the user to choose to output error messages to file by supplying the -logerrors parameter
IF "%1"=="-logerrors" ( "AS/win32/asw.exe" -xx -c -E -A ga.asm ) ELSE "AS/win32/asw.exe" -xx -c -E -A ga.asm

REM // if there were errors, a log file is produced
IF EXIST ga.log goto LABLERROR4

REM // combine the assembler output into a rom
IF EXIST ga.p "AS/win32/gap2bin" ga.p gabuilt.bin ga.h

REM // done -- pause if we seem to have failed, then exit
IF NOT EXIST ga.p goto LABLPAUSE
IF NOT EXIST gabuilt.bin goto LABLPAUSE
fixheader gabuilt.bin
exit /b
:LABLPAUSE

pause


exit /b

:LABLERROR1
echo Failed to build because write access to ga.h was denied.
pause


exit /b

:LABLERROR2
echo Failed to build because write access to ga.p was denied.
pause


exit /b

:LABLERROR3
echo Failed to build because write access to gabuilt.bin was denied.
pause

exit /b

:LABLERROR4
REM // display a noticeable message
echo.
echo ***************************************************************************
echo *                                                                         *
echo *   There were build errors/warnings. See ga.log for more details.   *
echo *                                                                         *
echo ***************************************************************************
echo.
pause

