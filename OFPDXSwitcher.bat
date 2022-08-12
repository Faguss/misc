REM OFPDXSwitcher from d3d8to9/DXVK to Kegetys' DXDLL and back
REM by Faguss (ofp-faguss.com) 2022.06.26
REM https://forums.bohemia.net/forums/topic/232454-d3d8to9-the-ultimate-ofpacwa-performance-booster
REM http://ofpr.info.paradoxstudio.uk/6900-kegetys-special-fx.html
REM Place d3d8to9/DXVK files in the game directory and DXDLL in the parent directory

@echo off
IF EXIST d3d9.dll GOTO uninstall_vulcan


:install_vulcan
echo changing to vulcan

ren d3d8.dll d3d8_keg.dll
move d3d8_keg.dll ..
move ..\d3d8.dll .
move ..\d3d9.dll .
move ..\d3d10.dll .
move ..\d3d10_1.dll .
move ..\d3d10core.dll .
move ..\d3d11.dll .
move ..\dxgi.dll .
goto end


:uninstall_vulcan
echo changing to DXDLL

move d3d8.dll ..
move d3d9.dll ..
move d3d10.dll ..
move d3d10_1.dll ..
move d3d10core.dll ..
move d3d11.dll ..
move dxgi.dll ..
move ..\d3d8_keg.dll .
ren d3d8_keg.dll d3d8.dll
goto end


:end
REM echo done
REM pause
