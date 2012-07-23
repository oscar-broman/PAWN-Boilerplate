@echo off
cls

for /f %%i in ("%0") do set BASE_PATH=%%~dpi

set WORKING_PATH=%CD%

pushd %BASE_PATH%\..

for %%X in (git.exe) do (set FOUND=%%~$PATH:X)

if not defined FOUND (
	if exist "C:\Program Files\Git\bin\git.exe" (
	    set GIT="C:\Program Files\Git\bin\git"
	) else (
		if exist "C:\Program Files (x86)\Git\bin\git.exe" (
	 	   set GIT="C:\Program Files (x86)\Git\bin\git"
		) else (
			echo Unable to find Git. Make sure it's installed.
			
			exit
		)
	)
) else (
	set GIT=git
)

%GIT% checkout master
%GIT% pull
%GIT% submodule update --init --recursive
%GIT% pull --recurse-submodules

popd
pause