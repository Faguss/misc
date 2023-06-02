@ECHO off
REM Different versions of TexView2 share the same registry key for config path
REM This script will launch the old TexView2 with its own config and then revert changes afterwards

REM Set path to the old TexView2 folder manually or leave it empty for auto-detect
SET texview=



IF NOT "%texview%" == "" GOTO skipautodetect

REM Find old TexView2
FOR /F "tokens=2* skip=2" %%a IN ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\TexView 2" /v UninstallString') DO SET texview=%%b
IF "%texview%" == "" ECHO Couldn't find TexView2 && GOTO end
SET texview=%texview:~0,-13%

:skipautodetect
IF NOT "%texview:~-1%" == "\" SET texview=%texview%\



REM Backup the current TexView config path
SET original_path=
FOR /F "tokens=2* skip=2" %%a IN ('reg query HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\BIStudio\TextureConvert /v configPath') DO SET original_path=%%b
IF "%original_path%" == "" ECHO Can't find BIStudio\TextureConvert && GOTO end

REM Modifying the registry requires admin rights
fsutil dirty query %systemdrive% >nul
IF NOT %errorlevel% == 0 ECHO Right click on %~nx0 and select Run as administrator && GOTO end

ECHO Changing registry config path to %texview%TexConvert.cfg
REG ADD "HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\BIStudio\TextureConvert" /v "configPath" /t REG_SZ /d "%texview%TexConvert.cfg" /f
ECHO:

ECHO Launching TexView2. Waiting to finish
"%texview%TexView2.exe"
ECHO:

ECHO Bringing back path to %original_path%
REG ADD "HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\BIStudio\TextureConvert" /v "configPath" /t REG_SZ /d "%original_path%" /f



:end
ECHO:
PAUSE