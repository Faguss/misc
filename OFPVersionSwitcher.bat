REM OFPVersionSwitcher from 1.99 to 2.01 and back
REM by Faguss (ofp-faguss.com) 2020.10.17
REM Place 2.01 files in a separate directory (default: !tools\201). 
REM Create an empty directory to backup 1.99 files (default: !tools\199)

@echo off
set path199=!tools\199
set path201=!tools\201
IF EXIST ArmAResistance.exe GOTO uninstall


:install
echo changing to 2.01

move bin\CONFIG.bin %path199%
move bin\Resource.cpp %path199%
move bin\STRINGTABLE.csv %path199%
move dta\anims.pbo %path199%
move dta\DTAEXT.pbo %path199%
move dta\Fonts.pbo %path199%
move dta\scripts.pbo %path199%

move %path201%\ArmAResistance.cfg .
move %path201%\ArmAResistance.exe .
move %path201%\ArmAResistance_Server.exe .
move %path201%\ArmAResistancePreferences.exe .
move %path201%\ARPreferences.csv .
move %path201%\addons\BISCamel.pbo addons
move %path201%\addons\Res.pbo addons
move %path201%\addons\ResLangExt.pbo addons
move %path201%\addons\voiceRH.pbo addons

move %path201%\bin\config.bin bin
move %path201%\bin\Resource.cpp bin
move %path201%\bin\STRINGTABLE.csv bin
move %path201%\dta\anims.pbo dta
move %path201%\dta\DTAEXT.pbo dta
move %path201%\dta\Fonts.pbo dta
move %path201%\dta\scripts.pbo dta
goto end


:uninstall
echo changing to 1.99

move bin\CONFIG.bin %path201%\bin
move bin\Resource.cpp %path201%\bin
move bin\STRINGTABLE.csv %path201%\bin
move dta\anims.pbo %path201%\dta
move dta\DTAEXT.pbo %path201%\dta
move dta\Fonts.pbo %path201%\dta
move dta\scripts.pbo %path201%\dta

move ArmAResistance.cfg %path201%
move ArmAResistance.exe %path201%
move ArmAResistance_Server.exe %path201%
move ArmAResistancePreferences.exe %path201%
move ARPreferences.csv  %path201%
move addons\BISCamel.pbo %path201%\addons
move addons\Res.pbo %path201%\addons
move addons\ResLangExt.pbo %path201%\addons
move addons\voiceRH.pbo %path201%\addons

move %path199%\CONFIG.bin bin
move %path199%\Resource.cpp bin
move %path199%\STRINGTABLE.csv bin
move %path199%\anims.pbo dta
move %path199%\DTAEXT.pbo dta
move %path199%\Fonts.pbo dta
move %path199%\scripts.pbo dta
goto end


:end
REM echo done
REM pause
