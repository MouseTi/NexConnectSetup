; NexConnect Setup - Downloader Installer with NSIS
; This creates a single EXE that downloads and installs NexConnect

!define APP_NAME "NexConnect"
!define APP_VERSION "1.0.0"
!define PUBLISHER "NexConnect"
!define INSTALL_DIR "$LOCALAPPDATA\NexConnect"
!define DOWNLOAD_URL "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0"

Name "${APP_NAME} Setup"
OutFile "NexConnect-Setup.exe"
InstallDir "${INSTALL_DIR}"
RequestExecutionLevel user
ShowInstDetails show

!include "MUI2.nsh"

!define MUI_ICON "src\iconapp\nex-logo.ico"
!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\NexConnect.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch NexConnect Now"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "Download and Install" SEC01
    SetOutPath "$INSTDIR"
    
    DetailPrint "Creating installation directory..."
    CreateDirectory "$INSTDIR"
    
    DetailPrint "Downloading NexConnect.exe..."
    NSISdl::download "${DOWNLOAD_URL}/NexConnect.exe" "$INSTDIR\NexConnect.exe"
    Pop $0
    StrCmp $0 "success" download_dll
        MessageBox MB_OK|MB_ICONSTOP "Download failed: $0"
        Abort
    
    download_dll:
    DetailPrint "Downloading nexus_runtime.dll..."
    NSISdl::download "${DOWNLOAD_URL}/nexus_runtime.dll" "$INSTDIR\nexus_runtime.dll"
    Pop $0
    StrCmp $0 "success" create_shortcuts
        MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: Failed to download DLL: $0"
    
    create_shortcuts:
    DetailPrint "Creating shortcuts..."
    
    ; Desktop shortcut
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\NexConnect.exe" "" "$INSTDIR\NexConnect.exe" 0
    
    ; Start menu shortcuts
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\NexConnect.exe"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Registry for Add/Remove Programs
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayIcon" "$INSTDIR\NexConnect.exe"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "${PUBLISHER}"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "InstallLocation" "$INSTDIR"
    
    DetailPrint "Installation complete!"
SectionEnd

Section "Uninstall"
    ; Stop any running instances
    DetailPrint "Removing files..."
    
    ; Remove files
    Delete "$INSTDIR\NexConnect.exe"
    Delete "$INSTDIR\nexus_runtime.dll"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\*.log"
    Delete "$INSTDIR\Uninstall.exe"
    
    ; Remove directories
    RMDir /r "$INSTDIR\cache"
    RMDir /r "$INSTDIR\logs"
    RMDir "$INSTDIR"
    
    ; Remove shortcuts
    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    
    ; Remove registry keys
    DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    
    DetailPrint "Uninstallation complete!"
SectionEnd

Function .onInstSuccess
    MessageBox MB_YESNO "${APP_NAME} has been installed successfully!$\n$\nDo you want to launch it now?" IDYES launch IDNO end
    launch:
        Exec "$INSTDIR\NexConnect.exe"
    end:
FunctionEnd
