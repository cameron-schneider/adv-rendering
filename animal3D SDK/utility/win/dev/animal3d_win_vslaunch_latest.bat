:: Visual Studio Launcher (internal, locate and launch latest version)
:: By Daniel S. Buckstein
@echo off

:: locate devenv
call "%~dp0/animal3d_win_vslocate.bat"

set "tmp_sln=%~1"
set "tmp_slndir=%animal3d_sdk%project\VisualStudio\_SLN\"
set "tmp_slnpath=%tmp_slndir%%tmp_sln%\%tmp_sln%.sln"

:: default paths and variables
for %%I in ("%vsdevenv%") do (
	set a3vstoolsdir="%%~dpI..\Tools\"
	set a3vslaunchpath="%%~dpI.\devenv"
)
set a3platformtoolset=v142
set a3sdkversion=10.0

start "%vsdevenv%" "%tmp_slnpath%"

:: unset variables
set "tmp_slnpath="
set "tmp_slndir="
set "tmp_sln="
