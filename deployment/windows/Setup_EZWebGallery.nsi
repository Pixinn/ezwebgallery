
;/////////
; INCLUDES
;/////////
!include LogicLib.nsh       ; Use more familiar flow control

;//////////////////
; GLOBALS VARS
;/////////////////
Var VCREDISTRETURNED


;--------------------------------

; The name of the installer
Name "EZWebGallery"

; The file to write
OutFile "Setup_EZWebGallery.exe"

; The default installation directory
InstallDir $PROGRAMFILES\EZWebGallery

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

;Install pages
Page license
Page components
Page directory
Page instfiles

;Unistall pages
UninstPage uninstConfirm
UninstPage instfiles


LicenseData "..\common\GPL_v3.txt"
;--------------------------------

 Function .onInit
  ;Ask to uninstall any prior version (TEMPORARY)
  MessageBox MB_YESNO|MB_ICONQUESTION "Please uninstall any prior version of EZWebGallery.$\r$\n$\r$\nShall we continue?" IDYES NoAbort
  abort:
    Abort "Aborting installation to manually uninstall prior version."
  NoAbort:
 FunctionEnd



; The stuff to install (required
Section "EZWebGallery"
  
  ;Required (read only)
  SectionIn RO

  ;Install VC2010 Redistribuable if not present
  Call CheckVCRedist
  ${If} $R0 != 1
    DetailPrint "Visual C++ 2010 redistributable not found."
    Call InstallVCRedist       
  ${Else}
    DetailPrint "Visual C++ 2010 redistributable already installed."
  ${EndIf}

  ;Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ;Files to copy
  SetOverwrite on
  File /r ..\..\..\distribution\windows\*
  
  ;Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\EZWebGallery "Install_Dir" "$INSTDIR"
  WriteRegDWORD HKLM SOFTWARE\EZWebGallery "Installed" 0x1
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery" "DisplayName" "EZWebGallery"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\EZWebGallery"   ;The context of this constant (All Users or Current user) depends on the SetShellVarContext setting. The default is the current user.
  CreateShortCut "$SMPROGRAMS\EZWebGallery\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\EZWebGallery\EZWebGallery.lnk" "$INSTDIR\EZWebGallery.exe" "" "$INSTDIR\EZWebGallery.exe" 0
SectionEnd

Section "Desktop Shortcut"
    SetShellVarContext all
    CreateShortCut "$DESKTOP\EZWebGallery.lnk" "$INSTDIR\EZWebGallery.exe" "" "$INSTDIR\EZWebGallery.exe" 0
SectionEnd




; Check for Visual Studio 2010 C++ redistributable
Function CheckVCRedist

   ; check for the key in registry
   ; http://blogs.msdn.com/b/astebner/archive/2010/05/05/10008146.aspx
   ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" "Installed"
   ${If} $R0 == 1
        return        
   ${Else}
        StrCpy $R0 "-1"
   ${EndIf}
   
FunctionEnd

; install Visual Studio 2010 C++ redistributable
Function InstallVcRedist

    SetOutPath '$TEMP'
    SetOverwrite on    
    File "vcredist_x86.exe"
    ExecWait '"$TEMP\vcredist_x86.exe" /norestart /nq' $VCREDISTRETURNED
    DetailPrint "Visual C++ 2010 redistributable installer returned $VCREDISTRETURNED"
    Delete "$TEMP\vcredist_x86.exe"
    
    ${If} $VCREDISTRETURNED != 0
        Abort 'Abording: "Visual C++ 2010 redistributable" was not installed.'
    ${EndIf}
    
FunctionEnd



Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery"
  DeleteRegKey HKLM "SOFTWARE\EZWebGallery"

  SetShellVarContext all
  
  ; Delete Files
  Delete "$INSTDIR\*.*"
  RMDir /r "$INSTDIR\imageformats"; /recursive
  RMDir /r "$INSTDIR\res"
  ;non recursive because user could have personnal skins here
  Delete "$INSTDIR\skins\Autumn.skin"
  RMDir /r "$INSTDIR\skins\Autumn_files"
  Delete "$INSTDIR\skins\BlackBoard.skin"
  RMDir /r "$INSTDIR\skins\BlackBoard_files"
  Delete "$INSTDIR\skins\default.skin"
  RMDir /r "$INSTDIR\skins\default_files"
  Delete "$INSTDIR\skins\Exemple_Highlight.skin"
  RMDir /r "$INSTDIR\skins\Exemple_Highlight_files"
  Delete "$INSTDIR\skins\Moria.skin"
  RMDir /r "$INSTDIR\skins\Moria_files"
  Delete "$INSTDIR\skins\Orient.skin"
  RMDir /r "$INSTDIR\skins\Orient_files"
  Delete "$INSTDIR\skins\WhitePaper.skin"
  RMDir /r "$INSTDIR\skins\WhitePaper_files"
  Delete "$INSTDIR\skins\Winter.skin"
  RMDir /r "$INSTDIR\skins\Winter_files"
  RMDir "$INSTDIR\skins"
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\EZWebGallery\*.*"
  Delete "$DESKTOP\EZWebGallery.lnk"

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\EZWebGallery"
  RMDir "$INSTDIR"

SectionEnd

