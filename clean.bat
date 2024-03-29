@ECHO Off
SET COMPILER_DIR=..\..\Compiler
SET JO_ENGINE_SRC_DIR=../../jo_engine
SET PATH=%COMPILER_DIR%\WINDOWS\Other Utilities;%PATH%

rm -f ./cd/0.bin
rm -f *.o
rm -f ./libsatiator/*.o
rm -f ./Save-Game-Copier/*.o
rm -f ./Save-Game-Copier/backends/*.o
rm -f ./satiator/*.o
rm -f ./satiator/iapetus/disc_format*.o
rm -f ./satiator/iapetus/*.o
rm -f ./satiator/iapetus/cd/*.o
rm -f ./satiator/iapetus/peripherals/*.o
rm -f ./satiator/iapetus/sh2/*.o
rm -f ./satiator/iapetus/video/*.o
rm -f ./states/*.o
rm -f %JO_ENGINE_SRC_DIR%/*.o
rm -f ./*.bin
rm -f ./*.coff
rm -f ./*.elf
rm -f ./*.map
rm -f ./*.iso
rm -f ./*.cue

cd satiator-rings-emulate
clean

ECHO Done.
