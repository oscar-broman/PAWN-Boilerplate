@echo off
cls

:: I had to do this to get compiling working on UNC paths (i.e. \\something\something).
for /f %%i in ("%0") do set BASE_PATH=%%~dpi

set WORKING_PATH=%CD%

:: Using "cd" doesn't work, but this does.
pushd %BASE_PATH%\..

compiler\bin\php.exe -n -d extension="./php-ext/php_openssl.dll" -f "%BASE_PATH%\pre-compiler\compile.php" -- "%WORKING_PATH%" "%BASE_PATH%" 

popd
pause