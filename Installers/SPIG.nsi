; NSIS installation script for the Set-Pos-In-Game script for OFP


;------Header---------------

!include "MUI2.nsh"
!include "logiclib.nsh"
!include "WordFunc.nsh"

SetCompressor /solid lzma
ShowInstDetails show
RequestExecutionLevel admin
InstallDir $INSTDIR



;------Strings--------------

VIAddVersionKey "ProductName" "SPIG Installer"
VIAddVersionKey "Comments" "Script for the Operation Flashpoint: Resistance 1.96 and ArmA: Cold War Assault 1.99"
VIAddVersionKey "CompanyName" "ofp-faguss.com"
VIAddVersionKey "LegalTrademarks" "Public Domain"
VIAddVersionKey "LegalCopyright" "Public Domain"
VIAddVersionKey "FileDescription" "Install SPIG script for OFP"
VIAddVersionKey "FileVersion" "1.5.1.0"
VIProductVersion "1.5.1.0"




;------Variables------------

Var finishpagetext
Var parameters
Var runtext
Var exename
Var cfgName

;------Customize------------

Name "SPIG"
OutFile "set-pos-in-game.exe"
!define MUI_ICON "img\orange-install.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "img\Installer_Welcome.bmp"

!define MUI_WELCOMEPAGE_TITLE "Set-Pos-In-Game v1.51"
!define MUI_WELCOMEPAGE_TEXT "This will install Set-Pos-In-Game script for the$\n$\n$\tOperation Flashpoint: Resistance 1.96$\n$\tArmA: Cold War Assault 1.99$\n$\tArmA: Resistance 2.01$\n$\n$\nFwatch 1.16 included$\n$\n$\nofp-faguss.com/set-pos-in-game"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "img\Installer_Header2.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_DIRECTORYPAGE_TEXT_TOP "Make sure this path leads to the game directory.$\n$\nGame should have been run at least once."
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $exename
!define MUI_FINISHPAGE_RUN_PARAMETERS $parameters
!define MUI_FINISHPAGE_RUN_TEXT $runtext
!define MUI_FINISHPAGE_LINK "On-line readme"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.ofp-faguss.com/files/set-pos-in-game.pdf"
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT $finishpagetext

!define MUI_FINISHPAGE_SHOWREADME ""
;!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!define MUI_LANGDLL_ALLLANGUAGES
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL


;------Functions------------

; This reads CWA reg key if OFP is not present
Function .onInit
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


Section 
	SetOverwrite ifnewer
	
	; Set string variables for the end
	StrCpy $finishpagetext ""
	StrCpy $parameters " -nosplash -mod=Set-Pos-In-Game"
	StrCpy $runtext "Launch game with the Fwatch"
	StrCpy $exename "fwatch.exe"

	; Copy SPIG
	SetOutPath "$INSTDIR\Set-Pos-In-Game\"
	File /r "Set-Pos-In-Game\*"
  
	; Copy Fwatch
	SetOutPath "$INSTDIR\"
	File /r "Fwatch116\*"

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
		StrCpy $parameters "$parameters -nolaunch "
		StrCpy $runtext "Launch Fwatch"
		StrCpy $finishpagetext "$finishpagetext$\n$\nYou will have to start the game manually."

	allDone:
	;if empty string then change it to 'all done'
	StrCmp $finishpagetext "" +1 +2
		StrCpy $finishpagetext "All Done"
	
	SetOutPath "$INSTDIR\"
SectionEnd