# Microsoft Developer Studio Project File - Name="Narratives" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Narratives - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Narratives.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Narratives.mak" CFG="Narratives - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Narratives - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Narratives - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Narratives - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Narratives - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
F90=fl32.exe
shaders=midl.exe
LIB32=xilink6.exe -lib


CPP=cwcl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Narratives_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"CKAll.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 CK2.lib VxMath.lib user32.lib /nologo /base:"0x25680000" /dll /pdb:none /machine:I386 /out:"..\..\..\BuildingBlocks\Narratives.dll"

!ELSEIF  "$(CFG)" == "Narratives - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
F90=fl32.exe
shaders=midl.exe
LIB32=xilink6.exe -lib


CPP=cwcl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Narratives_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CK2.lib VxMath.lib /nologo /base:"0x25680000" /dll /debug /machine:I386 /out:"..\..\..\BuildingBlocks\Narratives.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Narratives - Win32 ReleaseDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDebug"
# PROP BASE Intermediate_Dir "ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDebug"
# PROP Intermediate_Dir "ReleaseDebug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
F90=fl32.exe
shaders=midl.exe
LIB32=xilink6.exe -lib


CPP=cwcl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"CKAll.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"CKAll.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ck2.lib vxmath.lib user32.lib /nologo /base:"0x25680000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Narratives.dll"
# ADD LINK32 CK2.lib VxMath.lib user32.lib /nologo /base:"0x25680000" /dll /pdb:none /debug /machine:I386 /out:"..\..\..\BuildingBlocks\Narratives.dll"

!ENDIF 

# Begin Target

# Name "Narratives - Win32 Release"
# Name "Narratives - Win32 Debug"
# Name "Narratives - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Behaviors"

# PROP Default_Filter ""
# Begin Group "Object Management"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\AllDynamicDeleter.cpp"
# End Source File
# Begin Source File

SOURCE=".\ObjectCopier.cpp"
# End Source File
# Begin Source File

SOURCE=".\ObjectCreator.cpp"
# End Source File
# Begin Source File

SOURCE=".\ObjectDeleter.cpp"
# End Source File
# Begin Source File

SOURCE=".\ObjectLoader.cpp"
# End Source File
# Begin Source File

SOURCE=".\ObjectRenamer.cpp"
# End Source File
# Begin Source File

SOURCE=".\SoundLoader.cpp"
# End Source File
# Begin Source File

SOURCE=".\SpriteLoader.cpp"
# End Source File
# Begin Source File

SOURCE=".\TextureLoader.cpp"
# End Source File
# End Group
# Begin Group "States"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\EnterState.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\ExitState.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\IsInState.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\ReadState.cpp"
# End Source File
# Begin Source File

SOURCE=".\RestoreIC.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\SaveIC.cpp"
# End Source File
# Begin Source File

SOURCE=".\SaveState.cpp"
# End Source File
# End Group
# Begin Group "Scene Management"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\AddToScene.cpp"
# End Source File
# Begin Source File

SOURCE=".\GetCurrentScene.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\LaunchScene.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\RemoveFromScene.cpp"
# End Source File
# End Group
# Begin Group "Script Management"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\ActivateObject.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\ActivateScript.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\AttachScript.cpp"
# End Source File
# Begin Source File

SOURCE=".\CallScript.cpp"
# End Source File
# Begin Source File

SOURCE=".\DeactivateObject.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\DeactivateScript.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\executescript.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Narratives.cpp
# ADD CPP /Yc"CKAll.h"
# End Source File
# Begin Source File

SOURCE=.\Narratives.def
# End Source File
# Begin Source File

SOURCE=.\Narratives.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
