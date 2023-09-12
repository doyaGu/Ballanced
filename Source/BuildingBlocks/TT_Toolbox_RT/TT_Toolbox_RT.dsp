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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /FD /c
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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_TOOLBOX_RT_EXPORTS" /FD /GZ /c
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
# Begin Group "Behaviors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Behaviors\ActivateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\AnisoMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\BoundingSphereIntersection.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\BumpMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\CelShading.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\CheckCurveCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ChildrenIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Chrono.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ConvertBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ConvertPixelHomogen.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ConvertTimeToString.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\CopyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\CreateDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\CreateFontEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\DebugOFF.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\DebugON.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\DeleteFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ExistFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\FindSubString.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\FlushTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\FollowCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\FontCoordinatesToDataArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GeneratePlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetCurvePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetEnvironmentVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetFileNameFromPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetFullAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetGroupBoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetMemoryStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetNearestCurvePosition.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetUV2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\GetZOrder.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\HighMapMeshform.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\HomingMissile.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\InputString.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\IsObjectActive.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\IsObjectVisible.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\IsScriptActive.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\JoystickVectorMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\JoystickWaiter.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\KeySwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\KeyWaiter.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\LinearVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ListAllCurrentUsedTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ListDir.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\LoadMotorSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\LookAt.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\MagnetMeshForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\MotorSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\OperationSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ParameterMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\PMSMeshdeform.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\PreloadTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\PushButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\PushButton2.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ReflectionMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\RefractionMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\RemoveDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ReplaceInputparameter.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ReplacePath.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\RestoreDynamicIC.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\RestoreIC.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\RippleWave.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SaveAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SaveIC.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ScaleableProximity.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SceneStencilShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SearchRow.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Set2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Set2DSpriteOld.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Set3DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetDynamicPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetFSAA.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetMaterialValues.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SetMipMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ShadowMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ShadowPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ShowVector.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SinusMeshdeform.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SkyAround.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SplitString.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\SwitchOnLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\TestForJoystick.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\TextureInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\ViewFrustumClipping.cpp
# End Source File
# Begin Source File

SOURCE=.\Behaviors\WriteBack.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\DebugManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DXManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TT_Toolbox_RT.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CKFontManager.h
# End Source File
# Begin Source File

SOURCE=.\CKTextureFont.h
# End Source File
# Begin Source File

SOURCE=.\DebugManager.h
# End Source File
# Begin Source File

SOURCE=.\DXManager.h
# End Source File
# Begin Source File

SOURCE=.\TimeManager.h
# End Source File
# Begin Source File

SOURCE=.\ToolboxGuids.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
