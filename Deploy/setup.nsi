!include "MUI2.nsh"

;--------------------------------
; General
	Name "3DCenter Filter Tester"
	OutFile "3DCenterFilterTesterSetup.exe"
	RequestExecutionLevel admin
	SetCompressor /SOLID lzma
 
;--------------------------------
; Modern UI Configuration 
  !define MUI_ABORTWARNING
  !define MUI_ICON "../Resources/icon.ico"
 
;--------------------------------
; Pages 
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
 
;--------------------------------
; Languages 
  !insertmacro MUI_LANGUAGE "English"


InstallDir "$PROGRAMFILES\3DCenter Filter Tester"
InstallDirRegKey HKCU "Software\3DCenter Filter Tester" "Install Directory"

Var DirectXSetupError
Var VCSetupError

Section "Filter Tester"
	SectionIn RO
	SetOutPath "$INSTDIR"	

	File "Filter Tester.exe"
	File "wxbase290u_vc_custom.dll"
	File "wxmsw290u_aui_vc_custom.dll"
	File "wxmsw290u_core_vc_custom.dll"
	File /r CompiledShaders
	File /r Resources
	File /r Textures

	WriteRegStr HKCU "Software\3DCenter Filter Tester" "Install Directory" $INSTDIR

	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\3DCenter Filter Tester" \
                 "DisplayName" "3DCenter Filter Tester 1.0"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\3DCenter Filter Tester" \
                 "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\3DCenter Filter Tester" \
                 "DisplayIcon" "$\"$INSTDIR\Filter Tester.exe$\""
SectionEnd

Section "Uninstall"
	Delete "$SMPROGRAMS\3DCenter Filter Tester\3DCenter Filter Tester.lnk" 
	Delete "$SMPROGRAMS\3DCenter Filter Tester\Uninstall 3DCenter Filter Tester.lnk"
	RMDir "$SMPROGRAMS\3DCenter Filter Tester"
	
	Delete "$INSTDIR\Filter Tester.exe"
	Delete "$INSTDIR\wxbase290u_vc_custom.dll"
	Delete "$INSTDIR\wxmsw290u_aui_vc_custom.dll"
	Delete "$INSTDIR\wxmsw290u_core_vc_custom.dll"
	
	Delete "$INSTDIR\Textures\aftester.dds"
	Delete "$INSTDIR\Textures\defaultcolored.dds"
	Delete "$INSTDIR\Textures\defaultnomips.dds"
	Delete "$INSTDIR\Textures\defaultnpot.dds"
	Delete "$INSTDIR\Textures\finechecker.dds"
	Delete "$INSTDIR\Textures\ground1.dds"
	Delete "$INSTDIR\Textures\ground2.dds"
	Delete "$INSTDIR\Textures\maxfreq.dds"
	RMDir "$INSTDIR\Textures"
	
	Delete "$INSTDIR\Resources\default.dds"
	Delete "$INSTDIR\Resources\font.dds"
	Delete "$INSTDIR\Resources\font.fnt"
	RMDir "$INSTDIR\Resources"
	
	Delete "$INSTDIR\CompiledShaders\ALUFilteringPS.pso"
	Delete "$INSTDIR\CompiledShaders\FontPS.pso"
	Delete "$INSTDIR\CompiledShaders\FontVS.vso"
	Delete "$INSTDIR\CompiledShaders\SeparatorPS.pso"
	Delete "$INSTDIR\CompiledShaders\SeparatorVS.vso"
	Delete "$INSTDIR\CompiledShaders\TMUFilteringPS.pso"
	Delete "$INSTDIR\CompiledShaders\VertexShader.vso"
	RMDir "$INSTDIR\CompiledShaders"

	Delete "$INSTDIR\Uninstall.exe"	
	
	RMDir "$INSTDIR"
	
	DeleteRegKey /ifempty HKCU "Software\3DCenter Filter Tester\Install Directory"
	DeleteRegKey /ifempty HKCU "Software\3DCenter Filter Tester\AUI"
	DeleteRegKey /ifempty HKCU "Software\3DCenter Filter Tester"
	
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\3DCenter Filter Tester"
SectionEnd

Section "Start Menu Entry" 
	CreateDirectory "$SMPROGRAMS\3DCenter Filter Tester"	
	CreateShortCut "$SMPROGRAMS\3DCenter Filter Tester\3DCenter Filter Tester.lnk" "$INSTDIR\Filter Tester.exe"
	CreateShortCut "$SMPROGRAMS\3DCenter Filter Tester\Uninstall 3DCenter Filter Tester.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Visual C++ Runtime"
	SetOutPath "$TEMP"
	
	File "vcredist_x86.exe"
	DetailPrint "Running Visual C++ 2008 Runtime Setup..."
	ExecWait '"$TEMP\vcredist_x86.exe" /q:a' $VCSetupError
	DetailPrint "Finished Visual C++ 2008 Runtime Setup"
	
	Delete "$TEMP\vcredist_x86.exe"
SectionEnd

Section "DirectX"
	SetOutPath "$TEMP"
	
	File "dxwebsetup.exe"
	DetailPrint "Running DirectX Setup..."
	ExecWait '"$TEMP\dxwebsetup.exe"' $DirectXSetupError
	DetailPrint "Finished DirectX Setup"
 
	Delete "$TEMP\dxwebsetup.exe"	
SectionEnd