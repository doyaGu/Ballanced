# Microsoft Developer Studio Project File - Name="TT_InterfaceManager_RT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TT_InterfaceManager_RT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TT_InterfaceManager_RT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TT_InterfaceManager_RT.mak" CFG="TT_InterfaceManager_RT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TT_InterfaceManager_RT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TT_InterfaceManager_RT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TT_InterfaceManager_RT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_INTERFACEMANAGER_RT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_INTERFACEMANAGER_RT_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib user32.lib /nologo /dll /machine:I386 /out:"../../../BuildingBlocks/TT_InterfaceManager_RT.dll"

!ELSEIF  "$(CFG)" == "TT_InterfaceManager_RT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_INTERFACEMANAGER_RT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_INTERFACEMANAGER_RT_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib user32.lib /nologo /dll /debug /machine:I386 /out:"../../../BuildingBlocks/TT_InterfaceManager_RT.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TT_InterfaceManager_RT - Win32 Release"
# Name "TT_InterfaceManager_RT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChangeScreenMode.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateGameInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteGameInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\ExistsGameInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GetBooleanValueFromRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFloatValueFromRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGameBonus.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGameData.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGameID.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGameName.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGameScore.cpp
# End Source File
# Begin Source File

SOURCE=.\GetIntegerValueFromRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelBonus.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelData.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelID.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelName.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelReached.cpp
# End Source File
# Begin Source File

SOURCE=.\GetLevelScore.cpp
# End Source File
# Begin Source File

SOURCE=.\GetStringValueFromRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallArray.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\IsRookie.cpp
# End Source File
# Begin Source File

SOURCE=.\LimitFramerate.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDrivers.cpp
# End Source File
# Begin Source File

SOURCE=.\ListScreenModes.cpp
# End Source File
# Begin Source File

SOURCE=.\NemoArray.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerActive.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadArray.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCMOLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCMORestart.cpp
# End Source File
# Begin Source File

SOURCE=.\SendExitToSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\SetBooleanValueToRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SetFloatValueToRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SetGameBonus.cpp
# End Source File
# Begin Source File

SOURCE=.\SetGameData.cpp
# End Source File
# Begin Source File

SOURCE=.\SetGameID.cpp
# End Source File
# Begin Source File

SOURCE=.\SetGameName.cpp
# End Source File
# Begin Source File

SOURCE=.\SetGameScore.cpp
# End Source File
# Begin Source File

SOURCE=.\SetIntegerValueToRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelBonus.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelData.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelID.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelName.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelReached.cpp
# End Source File
# Begin Source File

SOURCE=.\SetLevelScore.cpp
# End Source File
# Begin Source File

SOURCE=.\SetRookie.cpp
# End Source File
# Begin Source File

SOURCE=.\SetStringValueToRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\SetTaskSwitchHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\TT_InterfaceManager_RT.cpp
# End Source File
# Begin Source File

SOURCE=.\TT_InterfaceManager_RT.def
# End Source File
# Begin Source File

SOURCE=.\UninstallArray.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowActivated.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteArray.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteRegistry.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ErrorProtocol.h
# End Source File
# Begin Source File

SOURCE=.\GameInfo.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceManager.h
# End Source File
# Begin Source File

SOURCE=.\NemoArray.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
