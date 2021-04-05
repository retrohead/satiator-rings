@ECHO Off
del autoboot.iso
SET COMPILER_DIR=..\..\Compiler
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%COMPILER_DIR%\WINDOWS\bin;%PATH%
make re
ren game.iso autoboot.iso
pause
clean