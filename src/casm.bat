@echo off
set source=%~1
set name=%~n1
set size=%~z1
set ext=%~x1
if "%~1" == "" goto Comp
if not exist %source% goto Notex
if %size% == 0 goto Empty
if not %ext% == .c goto Extension
if not exist arm-none-eabi-gcc.exe goto MissAss
arm-none-eabi-gcc -mthumb -mthumb-interwork -S -o %name%.asm %name%.c
if %errorlevel% == 0 goto Assemly
goto End

:Empty
echo Impossibile assemblare "%name%", file vuoto.
goto :eof

:Notex
echo Impossibile assemblare "%name%", file inesistente.
goto :eof

:Extension
echo Estensione del file errata.
goto :eof

:MissBLexe
echo casm_fix.exe mancante.
goto :eof

:Missasex
echo Assemblatore as.exe mancante.
goto :eof

:Missobj
echo File objcopy.exe mancante.
goto :eof

:MissThumb
echo File thumb.bat mancante.
goto :eof

:Assemly
if not exist arm-none-eabi-gcc.exe goto MissBLexe
bl.exe %name%.asm
if not exist arm-none-eabi-gcc.exe goto Missasexe
if not exist arm-none-eabi-gcc.exe goto Missobj
if not exist arm-none-eabi-gcc.exe goto MissThumb
call thumb.bat %name%.asm
goto :eof

:Comp
echo Assemblatore da C ad ASM, scritto da Andrea
echo Output Assemblatore: [file.asm]
echo.
:End
pause
