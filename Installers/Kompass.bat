REM  QBFC Project Options Begin
REM  HasVersionInfo: Yes
REM  Companyname: Comrades in Arms
REM  Productname: 
REM  Filedescription: Compass Installer
REM  Copyrights: 
REM  Trademarks: 
REM  Originalname: 
REM  Comments: 
REM  Productversion:  1. 0. 0. 0
REM  Fileversion:  1. 0. 0. 0
REM  Internalname: 
REM  Appicon: D:\Dokumenty\kompas\favicon.ico
REM  Embeddedfile: D:\Dokumenty\kompas\cpbo.exe
REM  Embeddedfile: D:\Dokumenty\kompas\kompas.p3d
REM  Embeddedfile: D:\Dokumenty\kompas\kompas_budik3.paa
REM  QBFC Project Options End

@echo off
cls
echo.
echo Comrades in Arms [CiA] compass updates

set myfiles=%myfiles%\

FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\Software\Codemasters\Operation Flashpoint" /v Main') DO SET PATH=%%B
:PathCheck
if not %PATH:~-1%==\ set PATH=%PATH%\
set PATH=%PATH%res\dta\

if not exist "%PATH%data.pbo" goto error
if not exist "%PATH%data3d.pbo" goto error
if not exist "%PATH%hwtl\data.pbo" goto error
if not exist "%PATH%hwtl\data3d.pbo" goto error
goto Confirm
:error
echo ERROR: Couldn't find OFP files
echo.
set PATH=
set /p PATH=Write path to OFP root folder:  
goto PathCheck

:Confirm
echo.
echo Before proceeding, please make sure that you have
echo at least 100MB free space on drive %PATH:~0,1%.
echo.
echo.

echo Press any key to continue or ctrl-c to abort.
pause >NUL

cls
echo Processing data.pbo...
echo.

"%myfiles%cpbo" -e "%PATH%data.pbo" "%PATH%data"
del /f /q "%PATH%data\kompas_budik3.paa"
copy "%myfiles%kompas_budik3.paa" "%PATH%data\" >NUL
del /f /q "%PATH%data.pbo"
"%myfiles%cpbo" -p "%PATH%data" "%PATH%data.pbo"
echo Please wait...
rmdir /q /s "%PATH%data"



cls
echo Processing hwtl\data.pbo...
echo.


"%myfiles%cpbo" -e "%PATH%hwtl\data.pbo" "%PATH%hwtl\data"
del /f /q "%PATH%hwtl\data\kompas_budik3.paa"
copy "%myfiles%kompas_budik3.paa" "%PATH%hwtl\data" >NUL
del /f /q "%PATH%hwtl\data.pbo"
"%myfiles%cpbo" -p "%PATH%hwtl\data" "%PATH%hwtl\data.pbo"
echo Please wait...
rmdir /q /s "%PATH%hwtl\data"


cls
echo Processing data3d.pbo...
echo.

"%myfiles%cpbo" -e "%PATH%data3d.pbo" "%PATH%data3d"
if exist "%PATH%data3d\kompas.p3d.srpc" del /f /q "%PATH%data3d\kompas.p3d.srpc"
copy "%myfiles%kompas.p3d" "%PATH%data3d\kompas.p3d.srpc" >NUL
del /f /q "%PATH%data3d.pbo"
"%myfiles%cpbo" -p "%PATH%data3d" "%PATH%data3d.pbo"
echo Please wait...
rmdir /q /s "%PATH%data3d"


cls
echo Processing hwtl\data3d.pbo...
echo.

"%myfiles%cpbo" -e "%PATH%hwtl\data3d.pbo" "%PATH%hwtl\data3d"
if exist "%PATH%hwtl\data3d\kompas.p3d.srpc" del /f /q "%PATH%hwtl\data3d\kompas.p3d.srpc"
copy "%myfiles%kompas.p3d" "%PATH%hwtl\data3d\kompas.p3d" >NUL
del /f /q "%PATH%hwtl\data3d.pbo"
"%myfiles%cpbo" -p "%PATH%hwtl\data3d" "%PATH%hwtl\data3d.pbo"
echo Please wait...
rmdir /q /s "%PATH%hwtl\data3d"


cls
echo.
echo All done.
echo The data files were successfully changed.

:end
pause
