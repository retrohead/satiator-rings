@ECHO Off
SET EMULATOR_DIR=..\..\Emulators

if exist autoboot.iso (
"%EMULATOR_DIR%\yabause\yabause.exe" -a -i autoboot.iso
) else (
echo Please compile first !
)
