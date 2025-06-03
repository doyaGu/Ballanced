# Microsoft Developer Studio Project File - Name="SoundBehavior" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SoundBehavior - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SoundBehavior.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SoundBehavior.mak" CFG="SoundBehavior - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SoundBehavior - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SoundBehavior - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SoundBehavior - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0


CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SoundBehavior - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Sound\Release"
# PROP Intermediate_Dir "\TempBuild\25\Sound\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SoundBehavior_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\\devserver\dev\xasdk-3.2.8c\include" /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ck2.lib winmm.lib vxmath.lib /nologo /base:"0x25700000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Sounds.dll" /libpath:"\\devserver\dev\xasdk-3.2.8c\lib\dynamic" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"

!ELSEIF  "$(CFG)" == "SoundBehavior - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\TempBuild\25\Sound"
# PROP Intermediate_Dir "\TempBuild\25\Sound"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SoundBehavior_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "\\devserver\dev\xasdk-3.2.8c\include" /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"CKALL.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ck2.lib VxMath.lib winmm.lib xaudio.lib /nologo /base:"0x25700000" /dll /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Sounds.dll" /pdbtype:sept /libpath:"\\devserver\dev\xasdk-3.2.8c\lib\static\mtdlld" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"


!ELSEIF  "$(CFG)" == "SoundBehavior - Win32 ReleaseDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SoundBehavior\ReleaseDebug"
# PROP BASE Intermediate_Dir "SoundBehavior\ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\SoundBehavior\ReleaseDebug"
# PROP Intermediate_Dir "\TempBuild\25\SoundBehavior\ReleaseDebug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Ob2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ck2.lib winmm.lib vxmath.lib /nologo /base:"0x25700000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Sounds.dll"
# ADD LINK32 ck2.lib winmm.lib vxmath.lib /nologo /base:"0x25700000" /dll /pdb:none /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Sounds.dll"

!ENDIF 

# Begin Target

# Name "SoundBehavior - Win32 Release"
# Name "SoundBehavior - Win32 Debug"
# Name "SoundBehavior - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Behaviors"

# PROP Default_Filter ""
# Begin Group "3D Properties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\positionsound.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetConeValues.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\setdopplerscale.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetRange.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\eqcontrol.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=".\behaviors src\FrequencyControl.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\PanningControl.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\VolumeControl.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\playsoundinstance.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\WavePlay.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Properties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\setpriority.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\setrendermode.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\fadein.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\fadeout.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetListener.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SoundManagerSetup.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Audio CD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\CDPlay2.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Source File

SOURCE=".\behaviors src\ACD.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\SoundBehavior.cpp
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=.\SoundBehavior.def
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"CKALL.h"
# End Source File
# End Group
# Begin Source File

SOURCE=".\behaviors src\ACD.h"
# End Source File
# Begin Source File

SOURCE=.\SoundBehavior.rc
# End Source File
# End Target
# End Project
