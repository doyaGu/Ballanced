# Microsoft Developer Studio Project File - Name="Visuals" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Visuals - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Visuals.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Visuals.mak" CFG="Visuals - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Visuals - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Visuals - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Visuals - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0


CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Visuals - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Visuals\Release"
# PROP Intermediate_Dir "\TempBuild\25\Visuals\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Visuals_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ck2.lib vxmath.lib user32.lib gdi32.lib /nologo /base:"0x25780000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Visuals.dll" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"

!ELSEIF  "$(CFG)" == "Visuals - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\TempBuild\25\visuals"
# PROP Intermediate_Dir "\TempBuild\25\visuals"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Visuals_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /Gi /GX /ZI /Od /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"CKALL.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ck2.lib VxMath.lib /nologo /dll /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Visuals.dll" /pdbtype:sept /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"


!ELSEIF  "$(CFG)" == "Visuals - Win32 ReleaseDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Visuals\ReleaseDebug"
# PROP BASE Intermediate_Dir "Visuals\ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Visuals\ReleaseDebug"
# PROP Intermediate_Dir "\TempBuild\25\Visuals\ReleaseDebug"
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
# ADD BASE LINK32 ck2.lib vxmath.lib user32.lib gdi32.lib /nologo /base:"0x25780000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Visuals.dll"
# ADD LINK32 ck2.lib vxmath.lib user32.lib gdi32.lib /nologo /base:"0x25780000" /dll /pdb:none /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Visuals.dll"

!ENDIF 

# Begin Target

# Name "Visuals - Win32 Release"
# Name "Visuals - Win32 Debug"
# Name "Visuals - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Show-Hide"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\Hide.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Hide2d.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\HierarchyHide.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Show.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Show2d.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ShowBoundingBox.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ShowMouseCursor.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Obsolete"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\DisplayScore.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Fps.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "FX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\Blink.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GlobalBlur.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\MakeTransparent.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\MarkSystem.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\MeshLighting.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\MotionBlur.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\PlanarFilter.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\PlanarReflection.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RenderCurve.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetRenderOptions.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetZBuf.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetZOrder.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SolidTrail.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TextureRender.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\UseZinfo.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "2D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\changespriteslot.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Display2dSprite.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\DisplayProgressionBar.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Set2DMaterial.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Set2DParent.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Set2DPosition.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetColorKey2d.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetRectangles.cpp"
# End Source File
# End Group
# Begin Group "Sprite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\Set3DSpriteMode.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SpriteMoviePlayer.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SpriteMultiAngle.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Group "Shadows"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\behaviors src\PlanarShadow.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ShadowCaster.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SimpleShadow.cpp"
# ADD CPP /Yu"CKALL.h"
# End Source File
# End Group
# Begin Source File

SOURCE=".\behaviors src\Stats.cpp"
# End Source File
# Begin Source File

SOURCE=.\Visuals.cpp
# ADD CPP /Yc"CKAll.h"
# End Source File
# Begin Source File

SOURCE=.\Visuals.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\Visuals.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\Visuals.rc
# End Source File
# End Target
# End Project
