# Microsoft Developer Studio Project File - Name="Lights" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Lights - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lights.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lights.mak" CFG="Lights - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lights - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Lights - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Lights - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0


CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lights - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\lights\Release"
# PROP Intermediate_Dir "\TempBuild\25\lights\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Lights_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25400000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Lights.dll" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"

!ELSEIF  "$(CFG)" == "Lights - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\TempBuild\25\Lights"
# PROP Intermediate_Dir "\TempBuild\25\Lights"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Lights_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"precomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ck2.lib VxMath.lib /nologo /base:"0x25400000" /dll /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Lights.dll" /pdbtype:sept /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"


!ELSEIF  "$(CFG)" == "Lights - Win32 ReleaseDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Lights\ReleaseDebug"
# PROP BASE Intermediate_Dir "Lights\ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Lights\ReleaseDebug"
# PROP Intermediate_Dir "\TempBuild\25\Lights\ReleaseDebug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Ob2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ck2.lib vxmath.lib /nologo /base:"0x25400000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Lights.dll"
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25400000" /dll /pdb:none /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Lights.dll"

!ENDIF 

# Begin Target

# Name "Lights - Win32 Release"
# Name "Lights - Win32 Debug"
# Name "Lights - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Behaviors"

# PROP Default_Filter ""
# Begin Group "Basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\SetColorLight.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetConstantAttenuation.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetFalloff.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetLightTarget.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetLinearAttenuation.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetQuadraticAttenuation.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetRangeLight.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetSpecularFlag.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTypeLight.cpp"
# End Source File
# End Group
# Begin Group "FX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\DisplayOmniLight.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\FlashColor.cpp"
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Lights.cpp
# End Source File
# Begin Source File

SOURCE=.\Lights.def
# End Source File
# Begin Source File

SOURCE=.\Lights.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\precomp.h"
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# End Group
# End Target
# End Project
