; NSIS Script for Flashpoint Cutscene Maker installation v1.3


;------Header---------------

!include "MUI2.nsh"
!include "logiclib.nsh"
!include "WordFunc.nsh"

SetCompressor /solid lzma
ShowInstDetails nevershow
RequestExecutionLevel user
;BrandingText "NSIS script by Faguss (ofp-faguss.com)"
InstallDir $INSTDIR
;InstallDirRegKey HKLM "Software\Codemasters\Operation Flashpoint" "MAIN"



;------Strings--------------

VIAddVersionKey "ProductName" "FCM Installer"
VIAddVersionKey "Comments" "Script for Operation Flashpoint: Resistance 1.96 and ArmA: Cold War Assault 1.99"
VIAddVersionKey "CompanyName" "ofp-faguss.com"
VIAddVersionKey "LegalTrademarks" "Public Domain"
VIAddVersionKey "LegalCopyright" "Public Domain"
VIAddVersionKey "FileDescription" "Install FCM script"
VIAddVersionKey "FileVersion" "1.1.2.0"
VIProductVersion "1.1.2.0"




;------Variables------------

Var finishpagetext
Var parameters
Var runtext
Var exename


;------Customize------------

Name "FCM"
OutFile "flashpoint_cutscene_maker.exe"
!define MUI_ICON "img\orange-install.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "img\Installer_Welcome.bmp"

!define MUI_WELCOMEPAGE_TITLE "Flashpoint Cutscene Maker v1.12"
!define MUI_WELCOMEPAGE_TEXT "This will install Cutscene Maker for$\n$\n$\tOperation Flashpoint: Resistance 1.96$\n$\tArmA: Cold War Assault 1.99$\n$\n$\nFwatch 1.15 included."
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "img\Installer_Header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_DIRECTORYPAGE_TEXT_TOP "Make sure this path leads to the game directory.$\n$\nGame must have been run at least once."
;!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $exename
!define MUI_FINISHPAGE_RUN_PARAMETERS $parameters
!define MUI_FINISHPAGE_RUN_TEXT $runtext
!define MUI_FINISHPAGE_LINK "On-line readme"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.ofp-faguss.com/files/flashpoint_cutscene_maker.pdf"
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
	
	IfFileExists $INSTDIR\flashpoint.cfg 0 +2
		StrCpy $9 "Fwatch OFP.lnk"
	IfFileExists $INSTDIR\ColdWarAssault.cfg 0 +2
		StrCpy $9 "Fwatch CWA.lnk"
	
	IfFileExists "$DESKTOP\$9" +2 0
	CreateShortCut "$DESKTOP\$9" "$INSTDIR\$exename" "$parameters"
FunctionEnd


;------Sections------------

Section 

; Set some string variables =======================================

  StrCpy $finishpagetext ""
  StrCpy $parameters " -nosplash -gamespy=master.ofpisnotdead.com"
  StrCpy $runtext "Launch game with the Fwatch"
  StrCpy $exename "fwatch.exe"




; Copy FCM ========================================================

  SetOutPath "$INSTDIR\FlashpointCutsceneMaker\"
  File /r "FlashpointCutsceneMaker\*"
 
  

  
; Copy FCM demo mission ===========================================

	SetOutPath "$INSTDIR"
	SetOverwrite ifnewer
	; If there's no "users" directory then don't copy anything
	IfFileExists "$INSTDIR\Users\*.*" getUser endDemos


	
	; Read key from registry about the current user
	getUser:
	StrCpy $7 ""
	IfFileExists $INSTDIR\flashpoint.cfg 0 +2
		ReadRegStr $7 HKCU "Software\Codemasters\Operation Flashpoint" "Player Name"
	IfFileExists $INSTDIR\ColdWarAssault.cfg 0 +2
		ReadRegStr $7 HKCU "Software\Bohemia Interactive Studio\ColdWarAssault" "Player Name"
	
	${If} $7 != ""
		IfFileExists "$INSTDIR\Users\$7\*.*" userDemos 0
		StrCpy $7 ""
	${EndIf}
	
	
	; If no key then find first folder
	FindFirst $0 $1 "$INSTDIR\Users\*"
	loop:
		DetailPrint $1
		StrCmp $1 "" done
		StrCmp $1 "." next
		StrCmp $1 ".." next
		DetailPrint "check: $1"
		IfFileExists "$INSTDIR\Users\$1\*.*" 0 next
			StrCpy $7 $1
			DetailPrint "found: $7"
			Goto done
		next:
		FindNext $0 $1
	Goto loop
	done:
	FindClose $0

	
	; No folder
	StrCmp $7 "" endDemos
  

	userDemos:
		CreateDirectory "$INSTDIR\Users\$7\missions"
		SetOutPath "$INSTDIR\Users\$7\missions\"
		File /r "missions\*"
		StrCpy $finishpagetext "$finishpagetext$\n$\nOpen FCM_demo in Mission Editor"
	
	endDemos:
	SetOutPath "$INSTDIR\"

	
	

  

  
; Install Fwatch if it's missing or if it's old version ===========

	SetOverwrite ifnewer
	SetOutPath "$INSTDIR\"
	File /r "data\*"
	
  IfFileExists $INSTDIR\flashpoint.cfg 0 +2
	File "fwatch.exe"
  IfFileExists $INSTDIR\ColdWarAssault.cfg 0 +3
  	File "fwatchCWA.exe"
	StrCpy $exename "fwatchCWA.exe"
	
	
	
	
; Check game executable ===========================================

	; Check if this is Steam version
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
    StrCpy $parameters "$parameters -nolaunch "
    StrCpy $runtext "Launch Fwatch"
    StrCpy $finishpagetext "$finishpagetext$\n$\nYou will have to start the game manually."

  allDone:
  ;if empty string then change it to 'all done'
  StrCmp $finishpagetext "" +1 +2
    StrCpy $finishpagetext "All Done"
	

	SetOutPath "$INSTDIR\"
	
SectionEnd





