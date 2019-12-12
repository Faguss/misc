; NSIS Script for OFP Aspect Ratio pack v2.07


;------Header---------------

!include "MUI2.nsh"
!include "TextFunc.nsh"
!include "WordFunc.nsh"
!include "nsDialogs.nsh"
!include "winmessages.nsh"
!include "logiclib.nsh"
!include "nsArray.nsh"

SetCompressor /solid lzma
;SetCompress off
ShowInstDetails show
RequestExecutionLevel admin
;BrandingText "NSIS script by Faguss (ofp-faguss.com)"
InstallDir $INSTDIR
;InstallDirRegKey HKLM "Software\Codemasters\Operation Flashpoint" "MAIN"





;------Strings--------------

VIAddVersionKey "ProductName" "OFP Aspect Ratio Installer"
VIAddVersionKey "Comments" "Update screen format for the Operation Flashpoint: Resistance 1.96 and ArmA: Cold War Assault 1.99"
VIAddVersionKey "CompanyName" "ofp-faguss.com"
VIAddVersionKey "LegalTrademarks" "Public Domain"
VIAddVersionKey "LegalCopyright" "Public Domain"
VIAddVersionKey "FileDescription" "Install OFP Aspect Ratio package"
VIAddVersionKey "FileVersion" "2.0.7.0"
VIProductVersion "2.0.7.0"





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

Name "OFP Aspect Ratio"
OutFile "ofp_aspect_ratio.exe"
!define MUI_ICON "img\orange-install.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "img\Installer_Welcome.bmp"

!define MUI_WELCOMEPAGE_TITLE "OFP Aspect Ratio v2.07"
!define MUI_WELCOMEPAGE_TEXT "This will update screen format for the$\n$\n$\tOperation Flashpoint: Resistance 1.96$\n$\tArmA: Cold War Assault 1.99$\n$\n$\tArmA: Resistance 2.01$\n$\n$\nFwatch 1.16 included."
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "img\Installer_Header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_DIRECTORYPAGE_TEXT_TOP "Make sure this path leads to the game directory.$\n$\nGame must have been run at least once.$\n$\nInstaller will replace resource.cpp and revert config.cpp (from the older version)"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $exename
!define MUI_FINISHPAGE_RUN_PARAMETERS $parameters
!define MUI_FINISHPAGE_RUN_TEXT $runtext
!define MUI_FINISHPAGE_LINK "On-line readme"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.ofp-faguss.com/files/ofp_aspect_ratio.pdf"
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT $finishpagetext

!define MUI_FINISHPAGE_SHOWREADME ""
;!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
Page Custom AspectSelection
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!define MUI_LANGDLL_ALLLANGUAGES
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL





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
	
	IfFileExists "$DESKTOP\$9" +2 0
	CreateShortCut "$DESKTOP\$9" "$INSTDIR\$exename" "$parameters"
FunctionEnd


; Install directory must have configuration file
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
	${NSD_CreateLabel} 0 0 100% 20% "Aspect ratio was set to $AR_STRING2"
	
	end:
	StrCmp $cfgName "ArmAResistance.cfg" 0 +2
	${NSD_CreateLabel} 0 80% 100% 20% "ArmA: Resistance 2.01 detected"
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




;------Sections------------

Section

	; Default values for string variables
	StrCpy $finishpagetext ""
	StrCpy $parameters " -nosplash -gamespy=master.ofpisnotdead.com -sortmissions"
	StrCpy $runtext "Launch game with the Fwatch"
	StrCpy $exename "fwatch.exe"
  
  
	; Display select aspect ratio
	DetailPrint "Ratio set to $AR_STRING"

	
	
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
	File "Aspect_Ratio.hpp"
	
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
	File "@airwolf\Resource.cpp"

	; Copy files for the correct ECP version
	IfFileExists $INSTDIR\@ECP 0 skipECP
	SetOutPath "$INSTDIR\@ECP\bin\"
	${If} $ECPRedux == "false"
		DetailPrint "Detected original ECP"
		File "@ECP\Resource.cpp"
	${Else}
		DetailPrint "Detected ECP Redux"
		File "@ECPR\Resource.cpp"
	${EndIf}
	skipECP:
	
	IfFileExists $INSTDIR\@ffsx85 0 +3
	SetOutPath "$INSTDIR\@ffsx85\bin\"		
	File "@ffsx85\Resource.cpp"
	
	IfFileExists $INSTDIR\@ffsx2007 0 +3
	SetOutPath "$INSTDIR\@ffsx2007\bin\"		
	File "@ffsx2007\Resource.cpp"
	
	IfFileExists $INSTDIR\@GRAA 0 +3
	SetOutPath "$INSTDIR\@GRAA\bin\"		
	File "@GRAA\Resource.cpp"
	
	IfFileExists $INSTDIR\@JF 0 +3
	SetOutPath "$INSTDIR\@JF\bin\"		
	File "@JF\Resource.cpp"

	IfFileExists $INSTDIR\@MCM-SLX 0 +3
	SetOutPath "$INSTDIR\@MCM-SLX\bin\"		
	File "@MCM-SLX\Resource.cpp"

	IfFileExists $INSTDIR\@OFrP_Mod 0 +3
	SetOutPath "$INSTDIR\@OFrP_Mod\bin\"		
	File "@OFrP_Mod\Resource.cpp"
	
	IfFileExists $INSTDIR\@wgl5 0 +3
	SetOutPath "$INSTDIR\@wgl5\bin\"		
	File "@wgl5\Resource.cpp"
	
	IfFileExists $INSTDIR\@WW2_MP 0 +3
	SetOutPath "$INSTDIR\@WW2_MP\bin\"		
	File "@WW2_MP\Resource.cpp"

	IfFileExists $INSTDIR\@ww4mod25 0 +3
	SetOutPath "$INSTDIR\@ww4mod25\bin\"		
	File "@ww4mod25\Resource.cpp"

	; CWA
	${If} "$cfgName" == "ColdWarAssault.cfg"
		SetOutPath "$INSTDIR\bin\"
		File "bin\Resource.cpp"
	${EndIf}
	
	; 2.01
	${If} "$cfgName" == "ArmAResistance.cfg"
		SetOutPath "$INSTDIR\bin\"
		File "bin201\Resource.cpp"
	${EndIf}

	IfFileExists $INSTDIR\finmod 0 +3
	SetOutPath "$INSTDIR\finmod\bin\"		
	File "finmod\Resource.cpp"

	IfFileExists $INSTDIR\Res 0 +3
	SetOutPath "$INSTDIR\Res\bin\"		
	File "Res\Resource.cpp"

	IfFileExists $INSTDIR\x_vte 0 +3
	SetOutPath "$INSTDIR\x_vte\bin\"		
	File "x_vte\Resource.cpp"

	IfFileExists $INSTDIR\ffur2006 0 +3
	SetOutPath "$INSTDIR\ffur2006\bin\"		
	File "ffur2006\Resource.cpp"

	IfFileExists $INSTDIR\@TuPaS 0 +3
	SetOutPath "$INSTDIR\@TuPaS\bin\"
	File "@TuPaS\Resource.cpp"

	IfFileExists $INSTDIR\CSLA 0 +3
	SetOutPath "$INSTDIR\CSLA\bin\"
	File "CSLA\Resource.cpp"
	
	
	
	
	
; Install Fwatch if it's missing or if it's old version ===========

	DetailPrint "   "
	DetailPrint "==== Installing Fwatch ==== "
	
	SetOverwrite ifnewer
	SetOutPath "$INSTDIR\"
	File /r "data\*"
		
; Check game executable ===========================================

	; Check if this is Steam version
	StrCmp $cfgName "ArmAResistance.cfg" notfound 0
	${WordFind} $INSTDIR "SteamApps\common" "E+1{" $R0
	IfErrors notfound found
		found:
			StrCpy $parameters "$parameters -steam"
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
    StrCpy $parameters "$parameters -nolaunch "
    StrCpy $runtext "Launch Fwatch"
    StrCpy $finishpagetext "$finishpagetext$\n$\nYou will have to start the game manually."

  allDone:
  ;if empty string then change it to 'all done'
  StrCmp $finishpagetext "" +1 +2
    StrCpy $finishpagetext "All Done"
	

	SetOutPath "$INSTDIR\"
	
SectionEnd
