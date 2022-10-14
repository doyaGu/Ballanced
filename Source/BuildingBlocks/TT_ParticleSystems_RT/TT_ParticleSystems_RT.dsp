# Microsoft Developer Studio Project File - Name="TT_ParticleSystems_RT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TT_ParticleSystems_RT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TT_ParticleSystems_RT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TT_ParticleSystems_RT.mak" CFG="TT_ParticleSystems_RT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TT_ParticleSystems_RT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TT_ParticleSystems_RT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TT_ParticleSystems_RT - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_PARTICLESYSTEMS_RT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_PARTICLESYSTEMS_RT_EXPORTS" /FD /c
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
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib /nologo /dll /machine:I386 /out:"../../../BuildingBlocks/TT_ParticleSystems_RT.dll"

!ELSEIF  "$(CFG)" == "TT_ParticleSystems_RT - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_PARTICLESYSTEMS_RT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TT_PARTICLESYSTEMS_RT_EXPORTS" /FD /GZ /c
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
# ADD LINK32 CK2.lib VxMath.lib kernel32.lib /nologo /dll /debug /machine:I386 /out:"../../../BuildingBlocks/TT_ParticleSystems_RT.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TT_ParticleSystems_RT - Win32 Release"
# Name "TT_ParticleSystems_RT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CubicEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\CubicParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CylindricalEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\CylindricalParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\DiscEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\DiscParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\LinearParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\LineEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemDeflectors.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemInteractors.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemMeshes.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemRenderCallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleTools.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanarEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanarParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\PointEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\PointParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\SphericalEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\SphericalParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeDependentPointParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\TimePointEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\TT_ParticleSystems_RT.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveParticleSystem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BlockingQueue.h
# End Source File
# Begin Source File

SOURCE=.\CubicEmitter.h
# End Source File
# Begin Source File

SOURCE=.\CurveEmitter.h
# End Source File
# Begin Source File

SOURCE=.\CylindricalEmitter.h
# End Source File
# Begin Source File

SOURCE=.\DiscEmitter.h
# End Source File
# Begin Source File

SOURCE=.\GeneralParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\LineEmitter.h
# End Source File
# Begin Source File

SOURCE=.\ObjectEmitter.h
# End Source File
# Begin Source File

SOURCE=.\Particle.h
# End Source File
# Begin Source File

SOURCE=.\ParticleEmitter.h
# End Source File
# Begin Source File

SOURCE=.\ParticleGuids.h
# End Source File
# Begin Source File

SOURCE=.\ParticleManager.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystemRenderCallbacks.h
# End Source File
# Begin Source File

SOURCE=.\ParticleTools.h
# End Source File
# Begin Source File

SOURCE=.\PlanarEmitter.h
# End Source File
# Begin Source File

SOURCE=.\PointEmitter.h
# End Source File
# Begin Source File

SOURCE=.\SphericalEmitter.h
# End Source File
# Begin Source File

SOURCE=.\ThreadSafeQueue.h
# End Source File
# Begin Source File

SOURCE=.\TimePointEmitter.h
# End Source File
# Begin Source File

SOURCE=.\WaveEmitter.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
