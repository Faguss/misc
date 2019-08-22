; NSIS Script for Fwatch installation v1.15


;------Header---------------

!include "MUI2.nsh"
!include "logiclib.nsh"
!include "nsArray.nsh"
!include "TextFunc.nsh"
!include "WordFunc.nsh"

;SetCompress off
SetCompressor /solid lzma
ShowInstDetails nevershow
RequestExecutionLevel user
;BrandingText "NSIS script by Faguss (ofp-faguss.com)"
InstallDir $INSTDIR
;InstallDirRegKey HKLM "Software\Codemasters\Operation Flashpoint" "MAIN"




;------Strings--------------

VIAddVersionKey "ProductName" "Fwatch Installer"
VIAddVersionKey "Comments" "Script Extender for Operation Flashpoint: Resistance 1.96 and ArmA: Cold War Assault 1.99"
VIAddVersionKey "CompanyName" "ofp-faguss.com"
VIAddVersionKey "LegalTrademarks" "Public Domain"
VIAddVersionKey "LegalCopyright" "Public Domain"
VIAddVersionKey "FileDescription" "Install Fwatch"
VIAddVersionKey "FileVersion" "1.1.5.0"
VIProductVersion "1.1.5.0"




;------Variables------------

Var finishpagetext
Var parameters
Var runtext
Var exename
Var cfgName
Var ECPRedux
Var key
Var value
Var dest
Var demosCopied


;------Customize------------

Name "Fwatch"
OutFile "fwatch_install.exe"
!define MUI_ICON "img\orange-install.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "img\Installer_Welcome.bmp"

!define MUI_WELCOMEPAGE_TITLE "Fwatch 1.15"
!define MUI_WELCOMEPAGE_TEXT "This will install Fwatch v1.15 for$\n$\n$\tOperation Flashpoint: Resistance 1.96$\n$\tor$\n$\tArmA: Cold War Assault 1.99$\n$\n$\n"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "img\Installer_Header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_DIRECTORYPAGE_TEXT_TOP "Make sure this path leads to the game directory.$\n$\nGame must have been run at least once."
;!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $exename
!define MUI_FINISHPAGE_RUN_PARAMETERS $parameters
!define MUI_FINISHPAGE_RUN_TEXT $runtext
!define MUI_FINISHPAGE_LINK "See changelog"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.ofp-faguss.com/fwatch/changelog/1.15"
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT $finishpagetext

!define MUI_COMPONENTSPAGE_TEXT_TOP "This installer also contains extras (normally downloaded separately). Files newer than those in the installer won't be replaced."
!define MUI_COMPONENTSPAGE_TEXT_COMPLIST "Select optional features:"

!define MUI_FINISHPAGE_SHOWREADME ""
;!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!define MUI_LANGDLL_ALLLANGUAGES
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL


;------Functions------------

; Read game path from registry
Function .onInit
	StrCpy $cfgName "flashpoint.cfg"

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

; Install directory must have configuration file
Function .onVerifyInstDir
    IfFileExists $INSTDIR\flashpoint.cfg +4 0
		IfFileExists $INSTDIR\ColdWarAssault.cfg +2 0
			Abort
			StrCpy $cfgName "ColdWarAssault.cfg"
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








;------Sections------------

Section

	StrCpy $demosCopied ""

; Set some string variables =======================================

  StrCpy $finishpagetext ""
  StrCpy $parameters " -nosplash -gamespy=master.ofpisnotdead.com"
  StrCpy $runtext "Launch game with the Fwatch"
  StrCpy $exename "fwatch.exe"

; Install Fwatch if it's missing or if it's old version ===========

	;AddSize 1024
	SetOverwrite ifnewer
	SetOutPath "$INSTDIR\"
	File /r "data\*"
	
	${If} ${FileExists} "$INSTDIR\flashpoint.cfg"
		File "fwatch.exe"
	${EndIf}
	
	${If} ${FileExists} "$INSTDIR\coldwarassault.cfg"
		File "fwatchCWA.exe"
		StrCpy $exename "fwatchCWA.exe"
	${EndIf}

	
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
    StrCpy $parameters "$parameters -nolaunch"
    StrCpy $runtext "Launch Fwatch"
    StrCpy $finishpagetext "You'll have to start the game manually."

  allDone:
  ;if empty string then change it to 'all done'
  StrCmp $finishpagetext "" +1 +2
    StrCpy $finishpagetext "All Done"
	
	SetOutPath "$INSTDIR\"

SectionEnd


Section "anims.pbo replacement" SectionAnims

	; Create list of mods
	nsArray::Split modfolders @ECP|ffur2006|@ffsx85|@ffsx2007|@GRAA|@JF|@OFrP_Mod|@wgl5|@WW2_MP|@ww4mod25|dta|finmod|Res|x_vte |
	
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

	IfFileExists $INSTDIR\finmod 0 +3
	SetOutPath "$INSTDIR\finmod\dta\"		
	File "anims\FDF\anims.pbo"
	
	IfFileExists $INSTDIR\Res 0 +3
	SetOutPath "$INSTDIR\Res\dta\"		
	File "anims\OFP Resistance 1.96\anims.pbo"
	
	IfFileExists $INSTDIR\x_vte 0 +3
	SetOutPath "$INSTDIR\x_vte\dta\"		
	File "anims\VTE\anims.pbo"
	
	SetOutPath "$INSTDIR\"
	
SectionEnd



Section "Fwatch Race" SectionRace

	SetOverwrite ifnewer
	SetOutPath "$INSTDIR\Missions\"
	File "Fwatch_Race2.Noe.pbo"
	SetOutPath "$INSTDIR\"
	
SectionEnd



Section /o "Command demos" SectionDemo

	SetOutPath "$INSTDIR"
	SetOverwrite ifnewer
	
	; If there's no "users" directory then copy to SP and MP missions
	IfFileExists "$INSTDIR\Users\*.*" getUser 0
		nouser:
		CreateDirectory "$INSTDIR\Missions\Fwatch Demos"
		SetOutPath "$INSTDIR\Missions\Fwatch Demos\"
		File /r "demo\*"
		SetOutPath "$INSTDIR\"
		File /r "demoMP\*"
		StrCmp $demosCopied "" 0 endDemos
		StrCpy $demosCopied "true"
		StrCpy $finishpagetext "$finishpagetext$\n$\nDemos were copied to Missions\ and MPMissions\"
	Goto endDemos


	
	; Read key from registry about the current user
	getUser:
	StrCpy $7 ""
	${If} $cfgName == "ColdWarAssault.cfg"
		ReadRegStr $7 HKCU "Software\Bohemia Interactive Studio\ColdWarAssault" "Player Name"
	${Else}
		ReadRegStr $7 HKCU "Software\Codemasters\Operation Flashpoint" "Player Name"
	${EndIf}
	
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
	StrCmp $7 "" nouser
  

	userDemos:
		CreateDirectory "$INSTDIR\Users\$7\missions"
		SetOutPath "$INSTDIR\Users\$7\missions\"
		File /r "demo\*"
		SetOutPath "$INSTDIR\Users\$7\"
		File /r "demoMP\*"	
		StrCmp $demosCopied "" 0 endDemos
		StrCpy $demosCopied "true"
		StrCpy $finishpagetext "$finishpagetext$\n$\nDemos were copied to Users\$7\"
	
	endDemos:
	SetOutPath "$INSTDIR\"

SectionEnd



Section /o "Practical examples" SectionPrac

	SetOverwrite ifnewer
	
	; If there's no "users" directory then copy to SP and MP missions
	IfFileExists "$INSTDIR\Users\*.*" getUser 0
		nouser:
		CreateDirectory "$INSTDIR\Missions\Fwatch Practical"
		SetOutPath "$INSTDIR\Missions\Fwatch Practical\"
		File /r "demoP\*"
		SetOutPath "$INSTDIR\"
		File /r "demoPMP\*"
		SetOutPath "$INSTDIR\addons\"
		File /r "addons\*"
		StrCmp $demosCopied "" 0 endDemos
		StrCpy $demosCopied "true"
		StrCpy $finishpagetext "$finishpagetext$\n$\nDemos were copied to Missions\ and MPMissions\"
	Goto endDemos


	
	; Read key from registry about the current user
	getUser:
	StrCpy $7 ""
	${If} $cfgName == "ColdWarAssault.cfg"
		ReadRegStr $7 HKCU "Software\Bohemia Interactive Studio\ColdWarAssault" "Player Name"
	${Else}
		ReadRegStr $7 HKCU "Software\Codemasters\Operation Flashpoint" "Player Name"
	${EndIf}
	
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
	StrCmp $7 "" nouser
  

	userDemos:
		CreateDirectory "$INSTDIR\Users\$7\missions"
		SetOutPath "$INSTDIR\Users\$7\missions\"
		File /r "demoP\*"
		SetOutPath "$INSTDIR\Users\$7\"
		File /r "demoPMP\*"	
		SetOutPath "$INSTDIR\addons\"
		File /r "addons\*"
		StrCmp $demosCopied "" 0 endDemos
		StrCpy $demosCopied "true"
		StrCpy $finishpagetext "$finishpagetext$\n$\nDemos were copied to Users\$7\"
	
	endDemos:
	SetOutPath "$INSTDIR\"

SectionEnd

;Language strings
;LangString DESC_SecDummy ${LANG_ENGLISH} "Main"
LangString DESC_SectionTwo ${LANG_ENGLISH} "anims.pbo"
LangString DESC_SectionRace ${LANG_ENGLISH} "Fwatch_Race2.Noe.pbo"
LangString DESC_SectionDemo ${LANG_ENGLISH} "demo missions"
LangString DESC_SectionPrac ${LANG_ENGLISH} "practical examples"
  
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;	!insertmacro MUI_DESCRIPTION_TEXT ${SectionMain} "Fwatch required files"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionAnims} "Indicates if Fwatch is enabled and checks for new version$\n$\nCreates backup"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionRace} "Single player mission by Kegetys"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionDemo} "111 example missions demonstrating Fwatch commands"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionPrac} "29 practical example missions showing how Fwatch commands could be used"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
