@echo off
cls

:: I had to do this to get compiling working on UNC paths (i.e. \\something\something).
for /f %%i in ("%0") do set BASE_PATH=%%~dpi

:: Using "cd" doesn't work, but this does.
pushd %BASE_PATH%\..

compiler\bin\php.exe -f %BASE_PATH%\pre-compiler\compile.php -- UNC=%BASE_PATH%

popd
pause