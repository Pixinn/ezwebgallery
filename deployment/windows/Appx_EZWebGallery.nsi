
;/////////
; INCLUDES
;/////////
!include LogicLib.nsh       ; Use more familiar flow control
!include x64.nsh

!include .\version.nsh

;//////////////////
; GLOBALS VARS
;/////////////////

;--------------------------------

; The name of the installer
Name "EZWebGallery"

; The file to write
OutFile "Appx_EZWebGallery.exe"

; The default installation directory
InstallDir $PROGRAMFILES64\EZWebGallery


;Some infos
VIProductVersion "${VERSION}.0.0"
VIAddVersionKey "ProductName" "EZWebGallery"
VIAddVersionKey "Comments" "The Free and easy web gallery generator."
VIAddVersionKey "CompanyName" "http://www.ezwebgallery.com"
VIAddVersionKey "LegalCopyright" "GPL v3"
VIAddVersionKey "FileVersion" "${BUILD_DATE}"

;--------------------------------

; Silent mode for Desktop Bridge
function .onInit
    SetSilent silent
functionEnd

;Install pages
Page instfiles


SetCompressor /SOLID /FINAL lzma
;--------------------------------





; The stuff to install (required
Section "EZWebGallery"
  
  ;Required (read only)
  SectionIn RO

  ;Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ;Files to copy
  SetOverwrite on
  File /r ..\..\..\distribution\windows\*
  
  ;Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\EZWebGallery_${BUILD_DATE} "Install_Dir" "$INSTDIR"
  WriteRegDWORD HKLM SOFTWARE\EZWebGallery_${BUILD_DATE} "Installed" 0x1

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


