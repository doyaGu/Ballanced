/*************************************************************************/
/*	File : VxMath.h														 */
/*																		 */	
/*	Main Header file for VxMath utility library							 */	
/*	Virtools SDK 														 */	 
/*	Copyright (c) Virtools 2000, All Rights Reserved.					 */	
/*************************************************************************/
#ifndef VXMATH_H
#define VXMATH_H

#include <stdlib.h>
#include <string.h>
#include <math.h>	

#ifdef VX_LIB

	#define VX_EXPORT  
#else
	#ifdef VX_API
		#if defined(WIN32)
			#define VX_EXPORT __declspec(dllexport)	// VC++ export option  {secret}
		#else
			#define VX_EXPORT
		#endif	
	#else
		#if defined(WIN32)
			#define VX_EXPORT __declspec(dllimport)	// VC++ export option  {secret}
		#else
			#define VX_EXPORT
		#endif	
	#endif
#endif

#include "VxMathDefines.h"

//*************** EXPORT DEFINES FOR LIB / DLL VERSIONS *************************************************

#ifndef CK_LIB
	#ifdef CK_PRIVATE_VERSION_VIRTOOLS
		#if defined(WIN32)
			#define DLL_EXPORT __declspec(dllexport)	// VC++ export option 
		#endif			
	#else 
		#define DLL_EXPORT 		
	#endif
#else
	#define DLL_EXPORT 
#endif

#ifndef CK_LIB
	#define PLUGIN_EXPORT 
#else
	#define PLUGIN_EXPORT 
#endif // CK_LIB

struct VxVector;
struct VxCompressedVector;

struct VxCompressedVectorOld;
struct Vx2DVector;
struct VxColor;
struct VxBbox;
struct VxQuaternion;
struct VxStridedData;
struct VxImageDescEx;

#include "XString.h"

#include "XUtil.h"
#include "XP.h"
// Port Class Utility is it the right place
#include "VxSharedLibrary.h"
#include "VxMeMoryMappedFile.h"
#include "CKPathSplitter.h"
#include "CKDirectoryParser.h"
#include "VxWindowFunctions.h"
#include "VxVector.h"
#include "Vx2dVector.h"
#include "VxMatrix.h"
#include "VxConfiguration.h"
#include "VxQuaternion.h"
#include "VxRect.h"
#include "VxOBB.h"
#include "VxRay.h"
#include "VxSphere.h"
#include "VxPlane.h"
#include "VxIntersect.h"
#include "VxDistance.h"
#include "VxFrustum.h"
#include "VxColor.h"
#include "VxMemoryPool.h"

// Containers
#include "XArray.h"
#include "XSArray.h"
#include "XClassArray.h"
#include "XList.h"
#include "XHashTable.h"
#include "XSHashTable.h"
#include "XSmartPtr.h"

typedef XArray<void*> XVoidArray;

// Threads and Synchro
#include "VxMutex.h"
#include "VxThread.h"

//----- Automatically called in dynamic library...{secret}

void InitVxMath(); 

void VxDetectProcessor(); 

//------ Interpolation
VX_EXPORT void InterpolateFloatArray(void* Res,void* array1,void *array2,float factor,int count);
VX_EXPORT void InterpolateVectorArray(void* Res,void* Inarray1,void *Inarray2,float factor,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT void MultiplyVectorArray(void* Res,void* Inarray1,const VxVector& factor,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT void MultiplyVector2Array(void* Res,void* Inarray1,const Vx2DVector& factor,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT void MultiplyVector4Array(void* Res,void* Inarray1,const VxVector4& factor,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT void MultiplyAddVectorArray(void* Res,void* Inarray1,const VxVector& factor,const VxVector& offset,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT void MultiplyAddVector4Array(void* Res,void* Inarray1,const VxVector4& factor,const VxVector4& offset,int count,DWORD StrideRes,DWORD StrideIn);
VX_EXPORT BOOL VxTransformBox2D(const VxMatrix& World_ProjectionMat,const VxBbox& box,VxRect* ScreenSize,VxRect* Extents,VXCLIP_FLAGS& OrClipFlags,VXCLIP_FLAGS& AndClipFlags); 
VX_EXPORT void VxProjectBoxZExtents(const VxMatrix& World_ProjectionMat,const VxBbox& box,float& ZhMin,float& ZhMax); 

//------- Structure copying
VX_EXPORT BOOL VxFillStructure(int Count,void* Dst,DWORD Stride,DWORD SizeSrc,void* Src); 
VX_EXPORT BOOL VxCopyStructure(int Count,void* Dst,DWORD OutStride,DWORD SizeSrc,void* Src,DWORD InStride);
VX_EXPORT BOOL VxIndexedCopy(const VxStridedData& Dst,const VxStridedData& Src,DWORD SizeSrc,int* Indices,int IndexCount);

//---- Graphic Utilities
VX_EXPORT 	void VxDoBlit(const VxImageDescEx& src_desc,const VxImageDescEx& dst_desc);
VX_EXPORT 	void VxDoBlitUpsideDown(const VxImageDescEx& src_desc,const VxImageDescEx& dst_desc);

VX_EXPORT 	void VxDoAlphaBlit(const VxImageDescEx& dst_desc,BYTE AlphaValue);
VX_EXPORT 	void VxDoAlphaBlit(const VxImageDescEx& dst_desc,BYTE* AlphaValues);

VX_EXPORT 	void VxGetBitCounts(const VxImageDescEx& desc,DWORD& Rbits,DWORD& Gbits,DWORD& Bbits,DWORD& Abits);
VX_EXPORT 	void VxGetBitShifts(const VxImageDescEx& desc,DWORD& Rshift,DWORD& Gshift,DWORD& Bshift,DWORD& Ashift);

VX_EXPORT 	void VxGenerateMipMap(const VxImageDescEx& src_desc,BYTE* DestBuffer);
VX_EXPORT 	void VxResizeImage32(const VxImageDescEx& src_desc,const VxImageDescEx& dst_desc);

VX_EXPORT	BOOL VxConvertToNormalMap(const VxImageDescEx& image,DWORD ColorMask);
VX_EXPORT	BOOL VxConvertToBumpMap(const VxImageDescEx& image);

VX_EXPORT 	DWORD GetBitCount(DWORD dwMask);
VX_EXPORT 	DWORD GetBitShift(DWORD dwMask);

VX_EXPORT 	VX_PIXELFORMAT  VxImageDesc2PixelFormat(const VxImageDescEx& desc);
VX_EXPORT 	void			VxPixelFormat2ImageDesc(VX_PIXELFORMAT Pf,VxImageDescEx& desc);
VX_EXPORT 	const char*		VxPixelFormat2String(VX_PIXELFORMAT Pf);

VX_EXPORT	void	VxBppToMask(VxImageDescEx& desc);

VX_EXPORT	int		GetQuantizationSamplingFactor();
VX_EXPORT	void	SetQuantizationSamplingFactor(int sf);

//---- Processor features
VX_EXPORT	char*			GetProcessorDescription();
VX_EXPORT	int				GetProcessorFrequency();
VX_EXPORT	DWORD			GetProcessorFeatures();
VX_EXPORT	void			ModifyProcessorFeatures(DWORD Add,DWORD Remove);
VX_EXPORT	ProcessorsType	GetProcessorType();

VX_EXPORT BOOL VxPtInRect(CKRECT *rect, CKPOINT *pt);

// Summary: Compute best Fit Box for a set of points
// 
VX_EXPORT BOOL VxComputeBestFitBBox(const BYTE *Points, const DWORD Stride, const int Count, VxMatrix& BBoxMatrix, const float AdditionnalBorder );

// Path Conversion
//
VX_EXPORT void VxAddDirectorySeparator(XString& path);
VX_EXPORT void VxConvertPathToSystemPath(XString& path);

/*************************************************
{filename:VxTimeProfiler}
Name: VxTimeProfiler
Summary: Class for profiling purposes

Remarks: 
	This class provides methods to accurately compute
	the time elapsed.
Example:
      // To profile several items :

      VxTimeProfiler MyProfiler;
      ...
	  float delta_time=MyProfiler.Current();
	  MyProfiler.Reset();
	  ...
	  float delta_time2=MyProfiler.Current();
See also: 
*************************************************/
class VX_EXPORT  VxTimeProfiler {
public:

	/*************************************************
	Name: VxTimeProfiler
	Summary: Starts profiling
	*************************************************/
	VxTimeProfiler() { Reset(); }

	/*************************************************
	Summary: Restarts the timer
	*************************************************/
	void Reset();

	/*************************************************
	Summary: Returns the current time elapsed (in milliseconds)
	*************************************************/
	float Current();

	/*************************************************
	Summary: Returns the current time elapsed (in milliseconds)
	*************************************************/
	float Split()
	{
		float c = Current();
		Reset();
		return c;
	}

protected:
	DWORD Times[4];
};


/****************************************************************
Name: VxImageDescEx

Summary: Enhanced Image description

Remarks:
The VxImageDescEx holds basically an VxImageDesc with additionnal support for
Colormap, Image pointer and is ready for future enhancements.


****************************************************************/
typedef struct VxImageDescEx {
	int		Size;				// Size of the structure
	DWORD	Flags;				// Reserved for special formats (such as compressed ) 0 otherwise

	int		Width;				// Width in pixel of the image
	int		Height;				// Height in pixel of the image
	union   {
		int		BytesPerLine;		// Pitch (width in bytes) of the image
		int		TotalImageSize;		// For compressed image (DXT1...) the total size of the image
	};
	int		BitsPerPixel;		// Number of bits per pixel
	union {
		DWORD	RedMask;			// Mask for Red component
		DWORD   BumpDuMask;			// Mask for Bump Du component
	};
	union {
		DWORD	GreenMask;			// Mask for Green component	
		DWORD	BumpDvMask;			// Mask for Bump Dv component
	};
	union {
		DWORD	BlueMask;			// Mask for Blue component
		DWORD   BumpLumMask;		// Mask for Luminance component
		
	};
	DWORD	AlphaMask;			// Mask for Alpha component

	short	BytesPerColorEntry;	// ColorMap Stride
	short	ColorMapEntries;	// If other than 0 image is palletized

	BYTE*	ColorMap;			// Palette colors
	BYTE*	Image;				// Image

public:
	VxImageDescEx() 
	{
		Size=sizeof(VxImageDescEx);
		memset((BYTE*)this+4,0,Size-4);
	}
	
	void Set(const VxImageDescEx& desc)
	{
		Size=sizeof(VxImageDescEx);
		if (desc.Size<Size) memset((BYTE*)this+4,0,Size-4);
		if (desc.Size>Size) return;
		memcpy((BYTE*)this+4,(BYTE*)&desc+4,desc.Size-4);
	}
	BOOL HasAlpha() {
		return ((AlphaMask!=0) || (Flags>=_DXT1));
	}


	int operator == (const VxImageDescEx& desc)
	{
		return (Size==desc.Size &&
				Height==desc.Height && Width==desc.Width &&
				BitsPerPixel==desc.BitsPerPixel && BytesPerLine==desc.BytesPerLine &&
				RedMask==desc.RedMask && GreenMask==desc.GreenMask &&
				BlueMask==desc.BlueMask && AlphaMask==desc.AlphaMask &&
				BytesPerColorEntry==desc.BytesPerColorEntry && ColorMapEntries==desc.ColorMapEntries);
	}

	int operator != (const VxImageDescEx& desc)
	{
		return (Size!=desc.Size ||
				Height!=desc.Height || Width!=desc.Width ||
				BitsPerPixel!=desc.BitsPerPixel || BytesPerLine!=desc.BytesPerLine ||
				RedMask!=desc.RedMask || GreenMask!=desc.GreenMask ||
				BlueMask!=desc.BlueMask || AlphaMask!=desc.AlphaMask ||
				BytesPerColorEntry!=desc.BytesPerColorEntry || ColorMapEntries!=desc.ColorMapEntries);
	}

} VxImageDescEx;


#endif
