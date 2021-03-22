@ECHO Off
del satiator-rings.iso
SET COMPILER_DIR=..\..\Compiler
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%COMPILER_DIR%\WINDOWS\bin;%PATH%
make re
ren game.iso satiator-rings.iso
pause
clean