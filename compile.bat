@ECHO Off
SET COMPILER_DIR=..\..\Compiler
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%COMPILER_DIR%\WINDOWS\bin;%PATH%
rm -rf autoboot.iso
make re
ren game.iso autoboot.iso
copy autoboot.iso SD/autoboot.iso
pause
clean