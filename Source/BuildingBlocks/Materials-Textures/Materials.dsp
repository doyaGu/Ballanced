# Microsoft Developer Studio Project File - Name="Materials" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Materials - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Materials.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Materials.mak" CFG="Materials - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Materials - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Materials - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Materials - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0


CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Materials - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Materials\Release"
# PROP Intermediate_Dir "\TempBuild\25\Materials\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Materials_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25500000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Materials.dll" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"

!ELSEIF  "$(CFG)" == "Materials - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\TempBuild\25\Materials"
# PROP Intermediate_Dir "\TempBuild\25\Materials"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Materials_EXPORTS" /YX /FD /GZ /c
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
# ADD LINK32 ck2.lib VxMath.lib /nologo /base:"0x25500000" /dll /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Materials.dll" /pdbtype:sept /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"


!ELSEIF  "$(CFG)" == "Materials - Win32 ReleaseDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Materials\ReleaseDebug"
# PROP BASE Intermediate_Dir "Materials\ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Materials\ReleaseDebug"
# PROP Intermediate_Dir "\TempBuild\25\Materials\ReleaseDebug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=xilink6.exe -lib
F90=fl32.exe
shaders=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"precomp.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Ob2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ck2.lib vxmath.lib /nologo /base:"0x25500000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Materials.dll"
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25500000" /dll /pdb:none /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Materials.dll"

!ENDIF 

# Begin Target

# Name "Materials - Win32 Release"
# Name "Materials - Win32 Debug"
# Name "Materials - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Behaviors"

# PROP Default_Filter ""
# Begin Group "Basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ChangeTextureSlot.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetAlphaTest.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetAmbient.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetBlendModes.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetColorKey.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetDestBlend.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetDiffuse.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetEmissive.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetFillMode.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetLitMode.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetMaterial.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetMaterialSprite3D.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetPower.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetPrelitColor.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetScrBlend.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetShadeMode.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetSpecular.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTexture.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTextureMag.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTextureMin.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTransparent.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetTwoSided.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetWrapMode.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetZBuffer.cpp"
# End Source File
# End Group
# Begin Group "Channel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ActivateChannel.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\AddChannel.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveChannel.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetChannelDestBlend.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetChannelMaterial.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetChannelSrcBlend.cpp"
# End Source File
# End Group
# Begin Group "Animation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\MoviePlayer.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TextureScroller.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TextureSinus.cpp"
# End Source File
# End Group
# Begin Group "Texture"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\behaviors src\ChangeTextureSize.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ChangeTextureVideoFormat.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\CreateMipMapTexture.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetMipMapLevel.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\WriteIntexture.cpp"
# End Source File
# End Group
# Begin Group "Mapping"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=".\behaviors src\CylindricalMapping.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\EnvMapping.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\PlanarMapping.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ScreenMapping.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetUVs.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SphericalMapping.cpp"
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\GUIDs.txt
# End Source File
# Begin Source File

SOURCE=.\Materials.cpp
# End Source File
# Begin Source File

SOURCE=.\Materials.def
# End Source File
# Begin Source File

SOURCE=.\Materials.rc
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
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\Materials.rc2
# End Source File
# End Group
# End Target
# End Project
