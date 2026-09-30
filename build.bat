@echo off
setlocal
pushd "%~dp0"
if not exist build mkdir build
gcc -std=c11 -Wall -Wextra -Werror lab9.c -o build/patient-queue.exe
if errorlevel 1 goto fail
echo Built build\patient-queue.exe
popd
exit /b 0
:fail
popd
exit /b 1
