# Microsoft Developer Studio Project File - Name="Logics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Logics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Logics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Logics.mak" CFG="Logics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Logics - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Logics - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Logics - Win32 ReleaseDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0


CPP=cwcl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Logics - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\logics\Release"
# PROP Intermediate_Dir "\TempBuild\25\logics\Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
shaders=midl.exe
F90=fl32.exe
LIB32=xilink6.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Logics_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25480000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Logics.dll" /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"

!ELSEIF  "$(CFG)" == "Logics - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\TempBuild\25\Logics"
# PROP Intermediate_Dir "\TempBuild\25\Logics"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
shaders=midl.exe
F90=fl32.exe
LIB32=xilink6.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Logics_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"precomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ck2.lib VxMath.lib /nologo /base:"0x25480000" /dll /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Logics.dll" /pdbtype:sept /libpath:"$(NEMORDIR)\Virtools_SDK\Lib"


!ELSEIF  "$(CFG)" == "Logics - Win32 ReleaseDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Logics\ReleaseDebug"
# PROP BASE Intermediate_Dir "Logics\ReleaseDebug"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\TempBuild\25\Logics\ReleaseDebug"
# PROP Intermediate_Dir "\TempBuild\25\Logics\ReleaseDebug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
shaders=midl.exe
F90=fl32.exe
LIB32=xilink6.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /Ob2 /I "$(NEMORDIR)\Virtools_SDK\Includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 ck2.lib vxmath.lib /nologo /base:"0x25480000" /dll /pdb:none /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Logics.dll"
# ADD LINK32 ck2.lib vxmath.lib /nologo /base:"0x25480000" /dll /pdb:none /debug /machine:I386 /out:"$(NEMORDIR)\BuildingBlocks\Logics.dll"

!ENDIF 

# Begin Target

# Name "Logics - Win32 Release"
# Name "Logics - Win32 Debug"
# Name "Logics - Win32 ReleaseDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "behaviors"

# PROP Default_Filter ""
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\IsInViewFrustrum.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ObjectBetween.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Proximity.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RayBoxIntersection.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RayIntersection.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Test.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Synchro"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\EnterCriticalSection.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\LeaveCriticalSection.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RendezVous.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Streaming"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\AllButOne.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\BinaryMemory.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\BinarySwitch.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\BoolEvent.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\FIFO.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\IBCQ.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\KeepActivate.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\LIFO.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Nop.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\OneAtATime.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ParameterSelector.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Priority.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RandomSwitch.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\sequencer.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SpecificBoolEvent.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\StreamingEvent.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\StreamingNOT.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SwitchOnParameter.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TriggerEvent.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\WaitForAll.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\While.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Message"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\BroadcastMessage.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\CheckForMessage.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GetDataMessage.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SendMes.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SendMessageToGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SwitchOnMessage.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\WaitMes.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Group"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\AddObjectToGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\AddToGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\FillGroupByType.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GetNearestInGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GroupClear.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GroupIterator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GroupOperator.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\IsInGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveFromGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveObjectFromGroup.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Calculator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\BezierTransform.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Calculator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\DoOp.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\DoOp_BackCompatibility.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GetComponent.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GetDeltat.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Identity.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Mini Calculator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ModifyComponent.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ModifyComponent_Base.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ModifyComponent_Functions.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\PerSecond.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Random.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetComponent.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Threshold.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Variation.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Interpolator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ColorHSVInterpolator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Interpolator.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorColor.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorFloat.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorInt.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorMatrix.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorOrientation.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\InterpolatorVector.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Attribute"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\HasAttribute.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\HasAttribute_old.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ObjectsWithAttributeIt.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveAttribute.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveAttribute_old.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetAttribute.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SetAttribute_old.cpp"
# End Source File
# End Group
# Begin Group "Array"

# PROP Default_Filter ""
# Begin Group "Add/Remove"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\AddArray.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayClear.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayInsertColumn.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayLoad.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayRemoveColum.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayRemoveIf.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayRemoveLine.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayWrite.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\RemoveArray.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Get/Set"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ArrayChangeValueIf.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetColumnTitle.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetElement.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetHighest.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetLine.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetLowest.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayGetNearest.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySearch.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySet.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySetElement.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySetLine.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\GetArray.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Operations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ArrayColumnsOperate.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayColumnTransform.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayProduct.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySum.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ArrayIterator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayIteratorIf.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayTestRow.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayValueCount.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\CreateGroupFromArray.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Order"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ArrayMove.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayMoveColum.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayReverse.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayShuffle.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySort.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArraySwap.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ArrayUnique.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Source File

SOURCE=".\behaviors src\ArrayGeneralFunctions.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "Loops"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\BezierInterpolator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\BezierProgression.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Chrono.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\CollectionIterator.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Counter.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\HierarchyParser.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\LinearProgression.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\NoiseProgression.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TimeBezierInterpolator.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TimerMini.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\TimeTimer.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# End Group
# Begin Group "String"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\CreateString.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\CutString.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\LoadString.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ReverseString.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\SplitString.cpp"
# End Source File
# End Group
# End Group
# Begin Group "Perlin Noise"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\perlin.cpp"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\perlin.h"
# End Source File
# End Group
# Begin Source File

SOURCE=".\behaviors src\hsv_rgb.cpp"
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\Logics.cpp
# ADD CPP /Yu"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\Logics.def
# End Source File
# Begin Source File

SOURCE=.\Logics.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\behaviors src\ArrayGeneralFunctions.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\Calculator.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\hsv_rgb.h"
# End Source File
# Begin Source File

SOURCE=".\behaviors src\ModifyComponent.h"
# End Source File
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

SOURCE=".\res\Dialog Button\Cancel1.BMP"
# End Source File
# Begin Source File

SOURCE=".\res\Dialog Button\Cancel3.BMP"
# End Source File
# Begin Source File

SOURCE=.\res\Logics.rc2
# End Source File
# Begin Source File

SOURCE=".\res\Dialog Button\Ok1.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Dialog Button\Ok3.bmp"
# End Source File
# End Group
# End Target
# End Project
