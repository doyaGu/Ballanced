# Microsoft Developer Studio Project File - Name="TT_Toolbox_RT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TT_Toolbox_RT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TT_Toolbox_RT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TT_Toolbox_RT.mak" CFG="TT_Toolbox_RT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TT_Toolbox_RT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TT_Toolbox_RT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TT_Toolbox_RT - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /FD /c
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
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib user32.lib /nologo /dll /machine:I386 /out:"../../../BuildingBlocks/TT_Toolbox_RT.dll"

!ELSEIF  "$(CFG)" == "TT_Toolbox_RT - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /FD /GZ /c
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
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib user32.lib /nologo /dll /debug /machine:I386 /out:"../../../BuildingBlocks/TT_Toolbox_RT.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TT_Toolbox_RT - Win32 Release"
# Name "TT_Toolbox_RT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActivateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AnisoMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=.\BoundingSphereIntersection.cpp
# End Source File
# Begin Source File

SOURCE=.\BumpMap.cpp
# End Source File
# Begin Source File

SOURCE=.\CelShading.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckCurveCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildrenIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\Chrono.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertPixelHomogen.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertTimeToString.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateFontEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugOFF.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugON.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DXManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ExistFile.cpp
# End Source File
# Begin Source File

SOURCE=.\FindSubString.cpp
# End Source File
# Begin Source File

SOURCE=.\FlushTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\FollowCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\FontCoordinatesToDataArray.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneratePlane.cpp
# End Source File
# Begin Source File

SOURCE=.\GetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\GetCurvePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\GetEnvironmentVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFileNameFromPath.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFullAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\GetGroupBoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GetMemoryStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\GetNearestCurvePosition.cpp
# End Source File
# Begin Source File

SOURCE=.\GetTime.cpp
# End Source File
# Begin Source File

SOURCE=.\GetUV2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\GetZOrder.cpp
# End Source File
# Begin Source File

SOURCE=.\HighMapMeshform.cpp
# End Source File
# Begin Source File

SOURCE=.\HomingMissile.cpp
# End Source File
# Begin Source File

SOURCE=.\InputString.cpp
# End Source File
# Begin Source File

SOURCE=.\IsObjectActive.cpp
# End Source File
# Begin Source File

SOURCE=.\IsObjectVisible.cpp
# End Source File
# Begin Source File

SOURCE=.\IsScriptActive.cpp
# End Source File
# Begin Source File

SOURCE=.\JoystickVectorMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\JoystickWaiter.cpp
# End Source File
# Begin Source File

SOURCE=.\KeySwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyWaiter.cpp
# End Source File
# Begin Source File

SOURCE=.\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\LinearVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\ListAllCurrentUsedTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDir.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadMotorSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\LookAt.cpp
# End Source File
# Begin Source File

SOURCE=.\MagnetMeshForm.cpp
# End Source File
# Begin Source File

SOURCE=.\MotorSound.cpp
# End Source File
# Begin Source File

SOURCE=.\OperationSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ParameterMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\PMSMeshdeform.cpp
# End Source File
# Begin Source File

SOURCE=.\PreloadTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\PushButton.cpp
# End Source File
# Begin Source File

SOURCE=.\PushButton2.cpp
# End Source File
# Begin Source File

SOURCE=.\ReflectionMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\RefractionMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoveDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplaceInputparameter.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplacePath.cpp
# End Source File
# Begin Source File

SOURCE=.\RestoreDynamicIC.cpp
# End Source File
# Begin Source File

SOURCE=.\RestoreIC.cpp
# End Source File
# Begin Source File

SOURCE=.\RippleWave.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveIC.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleableProximity.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneStencilShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchRow.cpp
# End Source File
# Begin Source File

SOURCE=.\Set2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Set2DSpriteold.cpp
# End Source File
# Begin Source File

SOURCE=.\Set3DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\SetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\SetDynamicPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\SetFSAA.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMaterialValues.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMipMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadowMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadowPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowVector.cpp
# End Source File
# Begin Source File

SOURCE=.\SinusMeshdeform.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyAround.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitString.cpp
# End Source File
# Begin Source File

SOURCE=.\SwitchOnLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\TestForJoystick.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\TT_Toolbox_RT.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewFrustumClipping.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteBack.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DebugManager.h
# End Source File
# Begin Source File

SOURCE=.\DXManager.h
# End Source File
# Begin Source File

SOURCE=.\TimeManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
