@ECHO OFF
echo .
echo . 
echo . 
echo *********************************** 
echo *** COMPILING EMULATE BIN BOOT  ***
echo *********************************** 
echo .
echo . 
echo . 
SET COMPILER_DIR=..\..\..\Compiler
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%COMPILER_DIR%\WINDOWS\bin;%PATH%
del "SD\satiator-rings\emulate.bin"
cd satiator-rings-emulate
copy libiapetus.a "%COMPILER_DIR%\WINDOWS\sh-elf\lib\libiapetus.a"
rm makefile
copy makefile_windows makefile
make clean
mkdir out
make
type ip.bin emulate_code.bin >> out/emulate.bin
echo "created emulate.bin"
move emulate_code.bin out/emulate_code.bin
move emulate.elf out/emulate.elf
rm makefile
copy makefile_linux makefile
cd ..
copy "satiator-rings-emulate\out\emulate.bin" "SD\satiator-rings\emulate.bin"

echo .
echo .
echo .
echo *********************************** 
echo ***  COMPILING SATIATOR RINGS   *** 
echo *********************************** 
echo .
echo .
echo .

compile
