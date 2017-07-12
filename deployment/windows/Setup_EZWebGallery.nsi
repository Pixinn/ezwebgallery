
;/////////
; INCLUDES
;/////////
!include LogicLib.nsh       ; Use more familiar flow control
!include  .\version.nsh

;//////////////////
; GLOBALS VARS
;/////////////////
Var VCREDISTRETURNED


;--------------------------------

; The name of the installer
Name "EZWebGallery ${BUILD_DATE}"

; The file to write
OutFile "Setup_EZWebGallery_${BUILD_DATE}.exe"

; The default installation directory
InstallDir $PROGRAMFILES\EZWebGallery\${BUILD_DATE}

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;Some infos
VIProductVersion "${VERSION}.0.0"
VIAddVersionKey "ProductName" "EZWebGallery"
VIAddVersionKey "Comments" "The Free and easy web gallery generator."
VIAddVersionKey "CompanyName" "http://www.ezwebgallery.com"
VIAddVersionKey "LegalCopyright" "GPL v3"
VIAddVersionKey "FileVersion" "${BUILD_DATE}"

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

SetCompressor /SOLID /FINAL lzma
;--------------------------------

; Function .onInit
;  ;Ask to uninstall any prior version (TEMPORARY)
;  MessageBox MB_YESNO|MB_ICONQUESTION "Please uninstall any prior version of EZWebGallery.$\r$\n$\r$\nShall we continue?" IDYES NoAbort
;  abort:
;    Abort "Aborting installation to manually uninstall prior version."
;  NoAbort:
; FunctionEnd



; The stuff to install (required
Section "EZWebGallery"
  
  ;Required (read only)
  SectionIn RO

  ;Install VC2013 Redistribuable if not present
  Call CheckVCRedist
  ${If} $R0 != 1
    DetailPrint "Visual C++ 2013 redistributable not found."
    Call InstallVCRedist       
  ${Else}
    DetailPrint "Visual C++ 2013 redistributable already installed."
  ${EndIf}

  ;Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ;Files to copy
  SetOverwrite on
  File /r ..\..\..\distribution\windows\*
  
  ;Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\EZWebGallery_${BUILD_DATE} "Install_Dir" "$INSTDIR"
  WriteRegDWORD HKLM SOFTWARE\EZWebGallery_${BUILD_DATE} "Installed" 0x1
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery_${BUILD_DATE}" "DisplayName" "EZWebGallery_${BUILD_DATE}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery_${BUILD_DATE}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery_${BUILD_DATE}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery_${BUILD_DATE}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\EZWebGallery\${BUILD_DATE}"   ;The context of this constant (All Users or Current user) depends on the SetShellVarContext setting. The default is the current user.
  CreateShortCut "$SMPROGRAMS\EZWebGallery\${BUILD_DATE}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\EZWebGallery\${BUILD_DATE}\EZWebGallery.lnk" "$INSTDIR\EZWebGallery.exe" "" "$INSTDIR\EZWebGallery.exe" 0
SectionEnd

Section "Desktop Shortcut"
    SetShellVarContext all
    CreateShortCut "$DESKTOP\EZWebGallery.lnk" "$INSTDIR\EZWebGallery.exe" "" "$INSTDIR\EZWebGallery.exe" 0
SectionEnd




; Check for Visual Studio 2013 C++ redistributable
Function CheckVCRedist

   ; check for the key in registry
   ; http://blogs.msdn.com/b/astebner/archive/2010/05/05/10008146.aspx
   ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\DevDiv\vc\Servicing\12.0\RuntimeMinimum" "Install"
   ${If} $R0 == 1
        return        
   ${Else}
        StrCpy $R0 "-1"
   ${EndIf}
   
FunctionEnd

; install Visual Studio 2013 C++ redistributable
Function InstallVcRedist

    SetOutPath '$TEMP'
    SetOverwrite on    
    File "vcredist_x64.exe"
    ExecWait '"$TEMP\vcredist_x64.exe" /norestart /nq' $VCREDISTRETURNED
    DetailPrint "Visual C++ 2013 redistributable installer returned $VCREDISTRETURNED"
    Delete "$TEMP\vcredist_x64.exe"
    
    ${If} $VCREDISTRETURNED != 0
        Abort 'Abording: "Visual C++ 2013 redistributable" was not installed.'
    ${EndIf}
    
FunctionEnd



Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZWebGallery_${BUILD_DATE}"
  DeleteRegKey HKLM "SOFTWARE\EZWebGallery_${BUILD_DATE}"

  SetShellVarContext all
  
  ; Delete Files
  Delete "$INSTDIR\*.*"
  RMDir /r "$INSTDIR\bearer" ; /recursive
  RMDir /r "$INSTDIR\iconengines"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\platforms"
  RMDir /r "$INSTDIR\qtwebengine_locales"
  RMDir /r "$INSTDIR\data"
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
  Delete "$SMPROGRAMS\EZWebGallery\${BUILD_DATE}\*.*"
  Delete "$DESKTOP\EZWebGallery.lnk"

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\EZWebGallery\${BUILD_DATE}"
  RMDir    "$SMPROGRAMS\EZWebGallery\" ;will only delete the folder if it is empty
  RMDir "$INSTDIR" ;will only delete the folder if it is empty (user can have personnal skin inside)
  RMDir  "$INSTDIR\.." ;will only delete the folder if it is empty : MAY BE ON FILESYSTEM --> NEVER PUT /r !!!!!!

SectionEnd
