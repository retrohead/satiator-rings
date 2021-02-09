@ECHO Off
SET COMPILER_DIR=..\..\Compiler
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%COMPILER_DIR%\WINDOWS\bin;%PATH%
make re
@ECHO Off
SET EMULATOR_DIR=..\..\Emulators

if exist game.iso (
"%EMULATOR_DIR%\yabause\yabause.exe" -a -i game.iso
) else (
echo Please compile first !
)
