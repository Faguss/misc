; NSIS Script for Fwatch 1.16 test version


;------Header---------------

!include "MUI2.nsh"
!include "TextFunc.nsh"
!include "WordFunc.nsh"
!include "nsDialogs.nsh"
!include "winmessages.nsh"
!include "logiclib.nsh"
!include "nsArray.nsh"

;SetCompress off
SetCompressor /solid lzma
ShowInstDetails show
RequestExecutionLevel admin
InstallDir $INSTDIR

!ifndef IPersistFile
!define IPersistFile {0000010b-0000-0000-c000-000000000046}
!endif
!ifndef CLSID_ShellLink
!define CLSID_ShellLink {00021401-0000-0000-C000-000000000046}
!define IID_IShellLinkA {000214EE-0000-0000-C000-000000000046}
!define IID_IShellLinkW {000214F9-0000-0000-C000-000000000046}
!define IShellLinkDataList {45e2b4ae-b1c3-11d0-b92f-00a0c90312e1}
	!ifdef NSIS_UNICODE
	!define IID_IShellLink ${IID_IShellLinkW}
	!else
	!define IID_IShellLink ${IID_IShellLinkA}
	!endif
!endif





;------Strings--------------

VIAddVersionKey "ProductName" "Fwatch Installer"
VIAddVersionKey "Comments" "Scripting extension for the Operation Flashpoint: Resistance 1.96 and ArmA: Cold War Assault 1.99"
VIAddVersionKey "CompanyName" "ofp-faguss.com"
VIAddVersionKey "LegalTrademarks" "Public Domain"
VIAddVersionKey "LegalCopyright" "Public Domain"
VIAddVersionKey "FileDescription" "Install Fwatch extension"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIProductVersion "1.0.0.0"





;------Variables------------

Var finishpagetext
Var parameters
Var runtext
Var exename

Var cfgName
Var AR_STRING
Var AR_STRING2
Var dialog
Var hwnd
Var radio1
Var radio2
Var radio3
Var radio4
Var radio5
Var radio6
Var FoundUserInfo
Var ModifiedHPP
Var key
Var value
Var dest
Var ECPRedux
Var Obsolete
Var HasBackup





;------Customize------------

Name "Fwatch"
OutFile "fwatch116beta_installer.exe"
!define MUI_ICON "img\orange-install.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "img\Installer_Welcome.bmp"

!define MUI_WELCOMEPAGE_TITLE "Fwatch 1.16 Beta"
!define MUI_WELCOMEPAGE_TEXT "This will install Fwatch for the$\n$\n$\tOperation Flashpoint: Resistance 1.96$\n$\tArmA: Cold War Assault 1.99$\n$\tArmA: Resistance 2.01$\n$\n$\nOFP Aspect Ratio pack 2.07 included"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "img\Installer_Header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_DIRECTORYPAGE_TEXT_TOP "Make sure this path leads to the game directory.$\n$\nGame must have been run at least once."
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $exename
!define MUI_FINISHPAGE_RUN_PARAMETERS $parameters
!define MUI_FINISHPAGE_RUN_TEXT $runtext
!define MUI_FINISHPAGE_LINK "Visit Website"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.ofp-faguss.com/fwatch116test"
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT $finishpagetext

!define MUI_FINISHPAGE_SHOWREADME ""
;!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
Page Custom AspectSelection
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!define MUI_LANGDLL_ALLLANGUAGES
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL




;------Sections------------

Section "Fwatch 1.16" SectionFwatch116
	DetailPrint "==== Installing Fwatch 1.16 ==== "
	SetOverwrite ifnewer
	SetOutPath "$INSTDIR\"
	File /r "Fwatch116\*"
	DetailPrint "   "
SectionEnd


Section "OFP Aspect Ratio 2.07" SectionOFPAR
	DetailPrint "==== Installing OFP Aspect Ratio 2.07 ==== "
	
	; Display select aspect ratio
	DetailPrint "Ratio set to $AR_STRING"
	
	SetOverwrite ifnewer

	
	
	; Check if "users" directory exists
	IfFileExists $INSTDIR\Users 0 +2
		Goto users
		MessageBox MB_OK "Couldn't find $\"Users$\" directory"
		DetailPrint "Installer needs Users\<name>\userinfo.cfg so it can adjust FOV settings"
		Abort
		
	; Check all the sub-folders in the "users" directory
	; Change configuration files
	users:
		StrCpy $FoundUserInfo "false"
		FindFirst $0 $1 $INSTDIR\Users\*
		loop:
			StrCmp $1 "" done
			StrCmp $1 "." findnext
			StrCmp $1 ".." findnext
			IfFileExists $INSTDIR\Users\$1\userinfo.cfg 0 findnext
				Call ChangeUserInfo
			findnext:
				FindNext $0 $1
				Goto loop
	done:
	FindClose $0
	
	; If no configuration file was changed
	${If} $FoundUserInfo == "false"
		MessageBox MB_OK "Couldn't find $\"userinfo.cfg$\" file"
		DetailPrint "Installer needs Users\<name>\userinfo.cfg so it can adjust FOV settings"
		Abort
	${EndIf}

	
	
	; Copy config file
	SetOutPath "$INSTDIR\"
	File "OFPAspectRatio\Aspect_Ratio.hpp"
	
	; Change aspect ratio settings in the config file
	StrCpy $ModifiedHPP "false"
	${LineFind} $INSTDIR\Aspect_Ratio.hpp "" "1:-1" AspectRatioHPP

	
	
	; Create list of mods
	nsArray::Split modfolders @airwolf|@ECP|@ffsx85|@ffsx2007|@GRAA|@JF|@MCM-SLX|@OFrP_Mod|@wgl5|@WW2_MP|@ww4mod25|bin|finmod|Res|x_vte|ffur2006|CSLA|@TuPaS |
	
	; Replace configs and resources in the modfolders
	${ForEachIn} modfolders $key $value
		StrCpy $dest $value
		
		; Before making backup for ECP determine if it's original or redux
		${If} $value == "@ECP"
			StrCpy $ECPRedux "false"
			${LineFind} $INSTDIR\@ECP\Bin\Config.cpp "/NUL" "1:100" ECPReduxSearch
		${EndIf}
		
		; Exception for CWA 1.99
		${If} $value == "bin"
			StrCmp "$cfgName" "ColdWarAssault.cfg" set_dest_to_root 0
			StrCmp "$cfgName" "ArmAResistance.cfg" set_dest_to_root Skip
			
			set_dest_to_root:
			StrCpy $dest "."
		${EndIf}
		
		; Make resource backup if it doesn't exist
		IfFileExists $INSTDIR\$dest\bin\_resource_backup.cpp +2 0
			Rename $INSTDIR\$dest\bin\resource.cpp $INSTDIR\$dest\bin\_resource_backup.cpp

		; Revert to the config backup if it exists
		StrCpy $HasBackup "false"
		StrCpy $Obsolete "false"
		 
		IfFileExists $INSTDIR\$dest\bin\_config_backup.cpp 0 +2
			StrCpy $HasBackup "true"
			
		IfFileExists $INSTDIR\$dest\bin\config.bin 0 +2
			StrCpy $HasBackup "true"
		
		${LineFind} $INSTDIR\$dest\Bin\Config.cpp "/NUL" "1:100" ConfigCheck
		
		${If} $Obsolete == "true"
		${AndIf} $HasBackup == "true"
			DetailPrint "---- Reverting $INSTDIR\$dest\bin\config.cpp"
			Rename $INSTDIR\$dest\bin\config.cpp $INSTDIR\$dest\bin\_config_obsolete.cpp
			Rename $INSTDIR\$dest\bin\_config_backup.cpp $INSTDIR\$dest\bin\config.cpp
		${EndIf}
	
		Skip:
	${Next}	
	

	
	; Copy files	
	IfFileExists $INSTDIR\@airwolf 0 +3
	SetOutPath "$INSTDIR\@airwolf\bin\"
	File "OFPAspectRatio\@airwolf\Resource.cpp"

	; Copy files for the correct ECP version
	IfFileExists $INSTDIR\@ECP 0 skipECP
	SetOutPath "$INSTDIR\@ECP\bin\"
	${If} $ECPRedux == "false"
		DetailPrint "Detected original ECP"
		File "OFPAspectRatio\@ECP\Resource.cpp"
	${Else}
		DetailPrint "Detected ECP Redux"
		File "OFPAspectRatio\@ECPR\Resource.cpp"
	${EndIf}
	skipECP:
	
	IfFileExists $INSTDIR\@ffsx85 0 +3
	SetOutPath "$INSTDIR\@ffsx85\bin\"		
	File "OFPAspectRatio\@ffsx85\Resource.cpp"
	
	IfFileExists $INSTDIR\@ffsx2007 0 +3
	SetOutPath "$INSTDIR\@ffsx2007\bin\"		
	File "OFPAspectRatio\@ffsx2007\Resource.cpp"
	
	IfFileExists $INSTDIR\@GRAA 0 +3
	SetOutPath "$INSTDIR\@GRAA\bin\"		
	File "OFPAspectRatio\@GRAA\Resource.cpp"
	
	IfFileExists $INSTDIR\@JF 0 +3
	SetOutPath "$INSTDIR\@JF\bin\"		
	File "OFPAspectRatio\@JF\Resource.cpp"

	IfFileExists $INSTDIR\@MCM-SLX 0 +3
	SetOutPath "$INSTDIR\@MCM-SLX\bin\"		
	File "OFPAspectRatio\@MCM-SLX\Resource.cpp"

	IfFileExists $INSTDIR\@OFrP_Mod 0 +3
	SetOutPath "$INSTDIR\@OFrP_Mod\bin\"		
	File "OFPAspectRatio\@OFrP_Mod\Resource.cpp"
	
	IfFileExists $INSTDIR\@wgl5 0 +3
	SetOutPath "$INSTDIR\@wgl5\bin\"		
	File "OFPAspectRatio\@wgl5\Resource.cpp"
	
	IfFileExists $INSTDIR\@WW2_MP 0 +3
	SetOutPath "$INSTDIR\@WW2_MP\bin\"		
	File "OFPAspectRatio\@WW2_MP\Resource.cpp"

	IfFileExists $INSTDIR\@ww4mod25 0 +3
	SetOutPath "$INSTDIR\@ww4mod25\bin\"		
	File "OFPAspectRatio\@ww4mod25\Resource.cpp"

	; CWA
	${If} "$cfgName" == "ColdWarAssault.cfg"
		SetOutPath "$INSTDIR\bin\"
		File "OFPAspectRatio\1.99\Resource.cpp"
	${EndIf}
	
	; 2.01
	${If} "$cfgName" == "ArmAResistance.cfg"
		SetOutPath "$INSTDIR\bin\"
		File "OFPAspectRatio\2.01\Resource.cpp"
	${EndIf}

	IfFileExists $INSTDIR\finmod 0 +3
	SetOutPath "$INSTDIR\finmod\bin\"		
	File "OFPAspectRatio\finmod\Resource.cpp"

	${If} "$cfgName" == "flashpoint.cfg"
		IfFileExists $INSTDIR\Res 0 +3
		SetOutPath "$INSTDIR\Res\bin\"
		File "OFPAspectRatio\1.96\Resource.cpp"
	${Else}
		SetOutPath "$INSTDIR\Res\bin\"
		Rename resource.cpp resource_disabled.cpp
	${EndIf}

	IfFileExists $INSTDIR\x_vte 0 +3
	SetOutPath "$INSTDIR\x_vte\bin\"		
	File "OFPAspectRatio\x_vte\Resource.cpp"

	IfFileExists $INSTDIR\ffur2006 0 +3
	SetOutPath "$INSTDIR\ffur2006\bin\"		
	File "OFPAspectRatio\ffur2006\Resource.cpp"

	IfFileExists $INSTDIR\@TuPaS 0 +3
	SetOutPath "$INSTDIR\@TuPaS\bin\"
	File "OFPAspectRatio\@TuPaS\Resource.cpp"

	IfFileExists $INSTDIR\CSLA 0 +3
	SetOutPath "$INSTDIR\CSLA\bin\"
	File "OFPAspectRatio\CSLA\Resource.cpp"
	
	DetailPrint "   "
SectionEnd


Section "Update Check" SectionAnims
	DetailPrint "==== Installing Update Check ==== "
	
	; Create list of mods
	nsArray::Split modfolders @ECP|ffur2006|@ffsx85|@ffsx2007|@GRAA|@JF|@OFrP_Mod|@wgl5|@WW2_MP|@ww4mod25|dta|finmod|Res|x_vte|CSLA|@PRMOD|L.O.M.D.E|swissmod|@TuPaS |
	
	; Replace configs and resources in the modfolders
	${ForEachIn} modfolders $key $value
		StrCpy $dest $value
		
		; Before making backup for ECP determine if it's original or redux
		${If} $value == "@ECP"
			StrCpy $ECPRedux "false"
			${LineFind} $INSTDIR\@ECP\Bin\Config.cpp "/NUL" "1:100" ECPReduxSearch
		${EndIf}
		
		; Exception for CWA 1.99
		${If} $value == "dta"
			StrCmp "$cfgName" "ColdWarAssault.cfg" 0 Skip
				StrCpy $dest "."
		${EndIf}
		
		; Make backup if it doesn't exist
		IfFileExists $INSTDIR\$dest\dta\_anims_backup.pbx +2 0
			Rename $INSTDIR\$dest\dta\anims.pbo $INSTDIR\$dest\dta\_anims_backup.pbx
		Skip:
	${Next}	
	

	SetOverwrite ifnewer
	
	; Copy files for the correct ECP version
	IfFileExists $INSTDIR\@ECP 0 skipECP
	SetOutPath "$INSTDIR\@ECP\dta\"
	${If} $ECPRedux == "false"
		File "anims\ECP\anims.pbo"
	${Else}
		File "anims\ECP Redux\anims.pbo"
	${EndIf}
	skipECP:

	IfFileExists $INSTDIR\ffur2006 0 +3
	SetOutPath "$INSTDIR\ffur2006\dta\"		
	File "anims\FFUR 2006\anims.pbo"
	
	IfFileExists $INSTDIR\@ffsx85 0 +3
	SetOutPath "$INSTDIR\@ffsx85\dta\"		
	File "anims\FFUR 2008\anims.pbo"
	
	IfFileExists $INSTDIR\@ffsx2007 0 +3
	SetOutPath "$INSTDIR\@ffsx2007\dta\"		
	File "anims\FFUR 2007\anims.pbo"
	
	IfFileExists $INSTDIR\@GRAA 0 +3
	SetOutPath "$INSTDIR\@GRAA\dta\"		
	File "anims\GRAA\anims.pbo"
	
	IfFileExists $INSTDIR\@JF 0 +3
	SetOutPath "$INSTDIR\@JF\dta\"		
	File "anims\Jungle Fever\anims.pbo"
	
	IfFileExists $INSTDIR\@OFrP_Mod 0 +3
	SetOutPath "$INSTDIR\@OFrP_Mod\dta\"		
	File "anims\Operation Frenchpoint\anims.pbo"
	

	IfFileExists $INSTDIR\@wgl5 0 +3
	SetOutPath "$INSTDIR\@wgl5\dta\"		
	File "anims\WGL\anims.pbo"
	
	IfFileExists $INSTDIR\@WW2_MP 0 +3
	SetOutPath "$INSTDIR\@WW2_MP\dta\"		
	File "anims\WW2_MP\anims.pbo"
	
	IfFileExists $INSTDIR\@ww4mod25 0 +3
	SetOutPath "$INSTDIR\@ww4mod25\dta\"		
	File "anims\WW4mod25\anims.pbo"
	
	; CWA
	${If} "$cfgName" == "ColdWarAssault.cfg"
		SetOutPath "$INSTDIR\dta\"
		File "anims\ArmA Cold War Assault 1.99\anims.pbo"
	${EndIf}
	
	; OFP
	${If} "$cfgName" == "flashpoint.cfg"
		SetOutPath "$INSTDIR\Res\dta\"		
		File "anims\OFP Resistance 1.96\anims.pbo"
	${EndIf}
	
	; ARMA:R
	${If} "$cfgName" == "ArmAResistance.cfg"
		SetOutPath "$INSTDIR\dta\"
		File "anims\Arma Resistance 2.01\anims.pbo"
	${EndIf}	

	IfFileExists $INSTDIR\finmod 0 +3
	SetOutPath "$INSTDIR\finmod\dta\"		
	File "anims\FDF\anims.pbo"
	
	IfFileExists $INSTDIR\x_vte 0 +3
	SetOutPath "$INSTDIR\x_vte\dta\"		
	File "anims\VTE\anims.pbo"
	
	IfFileExists $INSTDIR\CSLA 0 +3
	SetOutPath "$INSTDIR\CSLA\dta\"		
	File "anims\CSLA\anims.pbo"
	
	IfFileExists $INSTDIR\L.O.M.D.E 0 +3
	SetOutPath "$INSTDIR\L.O.M.D.E\dta\"		
	File "anims\Legend of Madness\anims.pbo"
	
	IfFileExists $INSTDIR\@PRMOD 0 +3
	SetOutPath "$INSTDIR\@PRMOD\dta\"		
	File "anims\PRMOD\anims.pbo"
	
	IfFileExists $INSTDIR\swissmod 0 +3
	SetOutPath "$INSTDIR\swissmod\dta\"		
	File "anims\Swissmod\anims.pbo"
	
	IfFileExists $INSTDIR\@TuPaS 0 +3
	SetOutPath "$INSTDIR\@TuPaS\dta\"		
	File "anims\Tupas\anims.pbo"
	
	DetailPrint "   "
SectionEnd


Section "Better HUD" SectionHUD
	DetailPrint "==== Installing Better HUD ==== "
	
	SetOverwrite ifnewer
	
	${If} "$cfgName" == "flashpoint.cfg"
		SetOutPath "$INSTDIR\Res\bin\"
	${Else}
		SetOutPath "$INSTDIR\Bin\"
	${EndIf}
	
	File "hud\config_fwatch_hud.cfg"
	DetailPrint "   "
SectionEnd


Section "-installation end"	
	; Default values for string variables
	StrCpy $finishpagetext ""
	StrCpy $parameters " -nosplash "
	StrCpy $runtext "Launch game with the Fwatch"
	StrCpy $exename "fwatch.exe"
	
	; Check game executable ===========================================

		; Check if this is Steam version
		StrCmp $cfgName "ArmAResistance.cfg" notfound 0
		${WordFind} $INSTDIR "SteamApps\common" "E+1{" $R0
		IfErrors notfound found
			found:
				StrCpy $parameters "-steam $parameters"
				StrCpy $runtext "Launch game with Steam"
				GoTo allDone
			notfound:
			
	  IfFileExists "$INSTDIR\flashpointresistance.exe" allDone 0
	  IfFileExists "$INSTDIR\ofp.exe" allDone 0
	  IfFileExists "$INSTDIR\flashpointbeta.exe" allDone 0
	  IfFileExists "$INSTDIR\operationflashpoint.exe" allDone 0
	  IfFileExists "$INSTDIR\operationflashpointbeta.exe" allDone 0
	  IfFileExists "$INSTDIR\ColdWarAssault.exe" allDone 0
	  IfFileExists "$INSTDIR\ArmAResistance.exe" allDone 0
		StrCpy $parameters "-nolaunch $parameters "
		StrCpy $runtext "Launch Fwatch"
		StrCpy $finishpagetext "$finishpagetext$\n$\nYou will have to start the game manually."

	  allDone:
	  ;if empty string then change it to 'all done'
	  StrCmp $finishpagetext "" +1 +2
		StrCpy $finishpagetext "All Done"
		
	SetOutPath "$INSTDIR\"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionFwatch116} "Adds fwatch.exe, fwatch.dll and $\"fwatch$\" folder"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionOFPAR} "Resource.cpp replacement for vanilla and mods. Enables widescreen and $\"MODS$\" option in the main menu"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionAnims} "anims.pbo replacement for vanilla and mods. Enables automatic update check in the main menu when you launch the game"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionHUD} "Adds config_fwatch_hud.cfg (to the $\"bin$\" folder) so that chat and action menu have more rows. Customizable"
!insertmacro MUI_FUNCTION_DESCRIPTION_END





;------Functions------------

; This reads CWA reg key if OFP is not present
Function .onInit
	StrCpy $AR_STRING ""
	StrCpy $cfgName ""
	
	${If} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 65790" "InstallLocation"
	${EndIf}
	
	${If} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "SOFTWARE\Bohemia Interactive Studio\ColdWarAssault" "MAIN"
	${EndIf}

	${If} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "Software\Codemasters\Operation Flashpoint" "MAIN"
	${EndIf}

	${If} $INSTDIR == ""
		StrCpy $INSTDIR "D:\Games\Operation Flashpoint"
	${EndIf}
FunctionEnd


; This creates a shortcut
Function finishpageaction
    SetOutPath "$INSTDIR\"
	StrCpy $9 "Fwatch.lnk"
	
	IfFileExists "$DESKTOP\$9" +4 0
	CreateShortCut "$DESKTOP\$9" "$INSTDIR\$exename" "$parameters"
	push "$DESKTOP\$9"
	call ShellLinkSetRunAs
FunctionEnd


; Install directory must contain game configuration file
Function .onVerifyInstDir
	IfFileExists $INSTDIR\ArmAResistance.cfg 0 +3
	StrCpy $cfgName "ArmAResistance.cfg"
	Goto checkDone

	IfFileExists $INSTDIR\ColdWarAssault.cfg 0 +3
	StrCpy $cfgName "ColdWarAssault.cfg"
	Goto checkDone
	
    IfFileExists $INSTDIR\flashpoint.cfg 0 +3
	StrCpy $cfgName "flashpoint.cfg"
	Goto checkDone
	Abort		
		
	checkDone:
FunctionEnd


; Detect aspect ratio automatically or provide manual seleciton
Function AspectSelection
	${IfNot} ${SectionIsSelected} ${SectionOFPAR}
		Goto message
	${EndIf}
	
	; Read resolution from the configuration file
	${ConfigRead} "$INSTDIR\$cfgName" "Resolution_W" $R0
	${ConfigRead} "$INSTDIR\$cfgName" "Resolution_H" $R1

	; Get rid of extra characters so it's only a number
	${StrFilter} "$R0" "1" "" "" $R0
	${StrFilter} "$R1" "1" "" "" $R1

	; Divide width by height
	FloatOp::Autor $0
	FloatOp::D $R0 $R1 $0

	; Assign aspect ratio based on the result
	StrCmp $0 "1.33333" 0 +3
		StrCpy $AR_STRING "4_3"
		StrCpy $AR_STRING2 "4:3"
	StrCmp $0 "1.25" 0 +3
		StrCpy $AR_STRING "5_4"
		StrCpy $AR_STRING2 "5:4"
	StrCmp $0 "1.6" 0 +3
		StrCpy $AR_STRING "16_10"
		StrCpy $AR_STRING2 "16:10"
	StrCmp $0 "1.77778" 0 +3
		StrCpy $AR_STRING "16_9"
		StrCpy $AR_STRING2 "16:9"
	StrCmp $0 "1.66667" 0 +3
		StrCpy $AR_STRING "15_9"
		StrCpy $AR_STRING2 "15:9"
	StrCmp $0 "2.37037" 0 +3
		StrCpy $AR_STRING "21_9"
		StrCpy $AR_STRING2 "21:9"
		
	; If nothing was assigned then continue; otherwise skip the rest of the page
	StrCmp $AR_STRING "" selection message
		
		
	; Create dialog with radio buttons
	selection:
	nsDialogs::Create 1018
		Pop $dialog
	${NSD_CreateRadioButton} 2% 10% 40% 6% "4:3"
		Pop $radio1
		${NSD_AddStyle} $radio1 ${WS_GROUP}
		${NSD_OnClick} $radio1 RadioChange
		${NSD_SetState} $radio1 ${BST_CHECKED}
	${NSD_CreateRadioButton} 2% 20% 40% 6% "5:4"
		Pop $radio2
		${NSD_AddStyle} $radio2 ${WS_GROUP}
		${NSD_OnClick} $radio2 RadioChange
	${NSD_CreateRadioButton} 2% 30% 40% 6% "16:10"
		Pop $radio3
		${NSD_AddStyle} $radio3 ${WS_GROUP}
		${NSD_OnClick} $radio3 RadioChange
	${NSD_CreateRadioButton} 2% 40% 40% 6% "16:9"
		Pop $radio4
		${NSD_AddStyle} $radio4 ${WS_GROUP}
		${NSD_OnClick} $radio4 RadioChange
	${NSD_CreateRadioButton} 2% 50% 40% 6% "15:9"
		Pop $radio5
		${NSD_AddStyle} $radio5 ${WS_GROUP}
		${NSD_OnClick} $radio5 RadioChange
	${NSD_CreateRadioButton} 2% 60% 40% 6% "21:9"
		Pop $radio6
		${NSD_AddStyle} $radio6 ${WS_GROUP}
		${NSD_OnClick} $radio6 RadioChange
	${NSD_CreateLabel} 0 0 100% 20% "Select your screen's aspect ratio:"
	Goto end
	
	message:
	nsDialogs::Create 1018
		Pop $dialog
	${IfNot} ${SectionIsSelected} ${SectionOFPAR}
		StrCpy $R7 "Aspect ratio was not set"
	${Else}
		StrCpy $R7 "Aspect ratio was set to $AR_STRING2"
	${EndIf}
	${NSD_CreateLabel} 0 0 100% 20% $R7
	
	end:
	${If} "$cfgName" == "ArmAResistance.cfg"
		StrCpy $R8 "ArmA: Resistance 2.01 detected"
	${ElseIf} "$cfgName" == "ColdWarAssault.cfg"
		StrCpy $R8 "ArmA: Cold War Assault 1.99 detected"
	${Else}
		StrCpy $R8 "OFP: Resistance 1.96 detected"
	${EndIf}
	
	${NSD_CreateLabel} 0 80% 100% 20% $R8
        Pop $hwnd
	nsDialogs::Show
FunctionEnd


; Handle user clicks on the dialog page
Function RadioChange
	Pop $1
	${If} $1 == $radio1
		StrCpy $AR_STRING "4_3"
		${NSD_SetState} $radio2 ${BST_UNCHECKED}
		${NSD_SetState} $radio3 ${BST_UNCHECKED}
		${NSD_SetState} $radio4 ${BST_UNCHECKED}
		${NSD_SetState} $radio5 ${BST_UNCHECKED}
		${NSD_SetState} $radio6 ${BST_UNCHECKED}
	${EndIf}
	${If} $1 == $radio2
		StrCpy $AR_STRING "5_4"
		${NSD_SetState} $radio1 ${BST_UNCHECKED}
		${NSD_SetState} $radio3 ${BST_UNCHECKED}
		${NSD_SetState} $radio4 ${BST_UNCHECKED}
		${NSD_SetState} $radio5 ${BST_UNCHECKED}
		${NSD_SetState} $radio6 ${BST_UNCHECKED}
	${EndIf}
	${If} $1 == $radio3
		StrCpy $AR_STRING "16_10"
		${NSD_SetState} $radio1 ${BST_UNCHECKED}
		${NSD_SetState} $radio2 ${BST_UNCHECKED}
		${NSD_SetState} $radio4 ${BST_UNCHECKED}
		${NSD_SetState} $radio5 ${BST_UNCHECKED}
		${NSD_SetState} $radio6 ${BST_UNCHECKED}
	${EndIf}
	${If} $1 == $radio4
		StrCpy $AR_STRING "16_9"
		${NSD_SetState} $radio1 ${BST_UNCHECKED}
		${NSD_SetState} $radio2 ${BST_UNCHECKED}
		${NSD_SetState} $radio3 ${BST_UNCHECKED}
		${NSD_SetState} $radio5 ${BST_UNCHECKED}
		${NSD_SetState} $radio6 ${BST_UNCHECKED}
	${EndIf}
	${If} $1 == $radio5
		StrCpy $AR_STRING "15_9"
		${NSD_SetState} $radio1 ${BST_UNCHECKED}
		${NSD_SetState} $radio2 ${BST_UNCHECKED}
		${NSD_SetState} $radio3 ${BST_UNCHECKED}
		${NSD_SetState} $radio4 ${BST_UNCHECKED}
		${NSD_SetState} $radio6 ${BST_UNCHECKED}
	${EndIf}
	${If} $1 == $radio6
		StrCpy $AR_STRING "21_9"
		${NSD_SetState} $radio1 ${BST_UNCHECKED}
		${NSD_SetState} $radio2 ${BST_UNCHECKED}
		${NSD_SetState} $radio3 ${BST_UNCHECKED}
		${NSD_SetState} $radio4 ${BST_UNCHECKED}
		${NSD_SetState} $radio5 ${BST_UNCHECKED}
	${EndIf}
FunctionEnd


; Modify entries in userinfo.cfg based on selected ratio
Function ChangeUserInfo

	; Default values (4:3)
	StrCpy $R1 "0.75"
	StrCpy $R2 "1"
	StrCpy $R3 "0"
	StrCpy $R4 "0"
	StrCpy $R5 "1"
	StrCpy $R6 "1"
	
	; Change values if different ratio
	${If} $AR_STRING == "5_4"
		StrCpy $R1 "0.8"
		StrCpy $R2 "1"
		StrCpy $R3 "0"
		StrCpy $R4 "0.03125"
		StrCpy $R5 "1"
		StrCpy $R6 "0.96875"
	${EndIf}
	
	${If} $AR_STRING == "16_10"
		StrCpy $R1 "0.75"
		StrCpy $R2 "1.2"
		StrCpy $R3 "0.083333"
		StrCpy $R4 "0"
		StrCpy $R5 "0.916667"
		StrCpy $R6 "1"
	${EndIf}
	
	${If} $AR_STRING == "16_9"
		StrCpy $R1 "0.75"
		StrCpy $R2 "1.333333"
		StrCpy $R3 "0.125"
		StrCpy $R4 "0"
		StrCpy $R5 "0.875"
		StrCpy $R6 "1"
	${EndIf}
	
	${If} $AR_STRING == "15_9"
		StrCpy $R1 "0.75"
		StrCpy $R2 "1.25"
		StrCpy $R3 "0.1"
		StrCpy $R4 "0"
		StrCpy $R5 "0.9"
		StrCpy $R6 "1"
	${EndIf}
	
	${If} $AR_STRING == "21_9"
		StrCpy $R1 "0.75"
		StrCpy $R2 "1.777778"
		StrCpy $R3 "0.21875"
		StrCpy $R4 "0"
		StrCpy $R5 "0.78125"
		StrCpy $R6 "1"
	${EndIf}
	
	; Write settings
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "fovTop" "=$R1;" $2
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "fovLeft" "=$R2;" $3
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "uiTopLeftX" "=$R3;" $4
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "uiTopLeftY" "=$R4;" $5
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "uiBottomRightX" "=$R5;" $6
	${ConfigWrite} $INSTDIR\Users\$1\userinfo.cfg "uiBottomRightY" "=$R6;" $7
	
	; Display results
	DetailPrint "User $1:"
	DetailPrint "     fovTop: $2"
	DetailPrint "     fovLeft: $3"
	DetailPrint "     uiTopLeftX: $4"
	DetailPrint "     uiTopLeftY: $5"
	DetailPrint "     uiBottomRightX: $6"
	DetailPrint "     uiBottomRightY: $7"
	
	StrCpy $FoundUserInfo "true"
FunctionEnd


; Change macro definition in Aspect_Ratio.hpp
Function AspectRatioHPP
	
	${If} $ModifiedHPP == "false"
		${WordFind} $R9 "#define aspect_ratio_" "E+1{" $R0
		IfErrors notfound found
		found:
			StrCpy $R9 "#define aspect_ratio_$AR_STRING$\r$\n"
			StrCpy $ModifiedHPP "true"
		notfound:
	${EndIf}

	Push $0
FunctionEnd


; Identify ECP version (original or redux)
Function ECPReduxSearch

	${If} $ECPRedux == "false"
		${WordFind} $R9 "whitegold" "E+1{" $R0
		IfErrors notfound found
		found:
			StrCpy $ECPRedux "true"
		notfound:
	${EndIf}
	
	Push $0
FunctionEnd


; Identify config.cpp
Function ConfigCheck

	${If} $Obsolete == "false"
		${WordFind} $R9 "Faguss" "E+1{" $R0
		IfErrors notfound found
		found:
			StrCpy $Obsolete "true"
		notfound:
	${EndIf}
	
	Push $0
FunctionEnd


; Set shortcut to run as administrator
Function ShellLinkSetRunAs
System::Store S
pop $9
System::Call "ole32::CoCreateInstance(g'${CLSID_ShellLink}',i0,i1,g'${IID_IShellLink}',*i.r1)i.r0"
${If} $0 = 0
	System::Call "$1->0(g'${IPersistFile}',*i.r2)i.r0" ;QI
	${If} $0 = 0
		System::Call "$2->5(w '$9',i 0)i.r0" ;Load
		${If} $0 = 0
			System::Call "$1->0(g'${IShellLinkDataList}',*i.r3)i.r0" ;QI
			${If} $0 = 0
				System::Call "$3->6(*i.r4)i.r0" ;GetFlags
				${If} $0 = 0
					System::Call "$3->7(i $4|0x2000)i.r0" ;SetFlags ;SLDF_RUNAS_USER
					${If} $0 = 0
						System::Call "$2->6(w '$9',i1)i.r0" ;Save
					${EndIf}
				${EndIf}
				System::Call "$3->2()" ;Release
			${EndIf}
		System::Call "$2->2()" ;Release
		${EndIf}
	${EndIf}
	System::Call "$1->2()" ;Release
${EndIf}
push $0
System::Store L
FunctionEnd
