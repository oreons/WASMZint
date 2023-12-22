set "CWD=%~dp0"
set "CWD=%CWD:\=/%"

REM Build dependencies
cd deps
call build_deps.bat
set "CWD=%~dp0"
set "CWD=%CWD:\=/%"
cd %CWD%

call build_nodeps.bat
