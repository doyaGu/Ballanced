#ifndef CKRASTERIZERTYPES_H
#define CKRASTERIZERTYPES_H

#include "VxDefines.h"
#include "VxColor.h"
#include "XArray.h"
#include "CKTypes.h"

class CKRasterizerDriver;
class CKRasterizerContext;
class CKRasterizer;

/**********************************************************
 Typedef for starting and ending function of a rasterizer
  These functions usually create and destroy a CKRasterizer instance
***********************************************************/
typedef CKRasterizer *(*CKRST_STARTFUNCTION)(WIN_HANDLE);
typedef void (*CKRST_CLOSEFUNCTION)(CKRasterizer *);
typedef void (*CKRST_OPTIONFCT)(CKDWORD);

/**************************************************
 To enable changes in the list of cards
 that have identified driver problems , this structure
 holds information about problematic drivers.
 They are created at load time by reading
 the CKGLRasterizer.ini file present in the render engines directory
 for the OpenGL rasterizer
***************************************************/
typedef struct CKDriverProblems
{
    //--- Driver identification
    XString m_Vendor;
    XString m_Renderer;
    XString m_DeviceDesc;
    XString m_Version;

    //--- Special constraints (Os,Version,Bpp)
    // If none are set the bug is always there
    CKBOOL m_VersionMustBeExact;
    XArray<VX_OSINFO> m_ConcernedOS; // List of OS on which the bug appears
    CKBOOL m_OnlyIn16;               // Bug present only in 16 bpp display mode
    CKBOOL m_OnlyIn32;               // Bug present only in 32 bpp display mode

    //--- Driver Bugs
    int m_RealMaxTextureWidth;                      // Max texture width and height
    int m_RealMaxTextureHeight;                     //
    CKBOOL m_ClampToEdgeBug;                        // Driver propose the "ClampToEdge" extension without really supporting it...
    XArray<VX_PIXELFORMAT> m_TextureFormatsRGBABug; // Some pixels format in OpenGL require a swap of the R & B components
    CKDriverProblems()
    {
        m_ClampToEdgeBug = FALSE;
        m_VersionMustBeExact = FALSE;
        m_OnlyIn16 = FALSE;
        m_OnlyIn32 = FALSE;
        m_RealMaxTextureWidth = 0;
        m_RealMaxTextureHeight = 0;
    }
} CKDriverProblems;

/***********************************************************
 This structure must map the VxStats structure
*********************************************************/
typedef struct CKRasterizerStats
{
    int NbTrianglesDrawn;    // Number of triangle primitives sent to rasterizer during one frame.
    int NbPointsDrawn;       // Number of points primitives sent to rasterizer during one frame.
    int NbLinesDrawn;        // Number of lines primitives sent to rasterizer during one frame.
    int NbVerticesProcessed; // Number of vertices transformed during one frame
} CKRasterizerStats;

/***********************************************************
 Light Structure passed to CKRasterizerContext::SetLight()
*********************************************************/
struct CKLightData
{
    VXLIGHT_TYPE Type;  // Point,Spot,Directional
    VxColor Diffuse;    // Diffuse Color
    VxColor Specular;   // Specular Color (Unused...)
    VxColor Ambient;    // Ambient Color (Unused...)
    VxVector Position;  // World Position
    VxVector Direction; // Direction
    float Range;        // Range
    float Falloff;
    float Attenuation0;
    float Attenuation1;
    float Attenuation2;
    float InnerSpotCone; // Only for spotlights
    float OuterSpotCone; // Only for spotlights
};

/***********************************************************
//---- Material Structure passed to CKRasterizerContext::SetMaterial()
*********************************************************/
struct CKMaterialData
{
    VxColor Diffuse;
    VxColor Ambient;
    VxColor Specular;
    VxColor Emissive;
    float SpecularPower;
};

/***********************************************************
//---- Viewport Structure passed to CKRasterizerContext::SetViewport()
*********************************************************/
struct CKViewportData
{
    int ViewX;      // Viewport Top-Left Corner
    int ViewY;      //  "		"	"		"
    int ViewWidth;  // Viewport Size
    int ViewHeight; //
    float ViewZMin; // Viewport Z Clipping (should be 0..1)
    float ViewZMax; //
};

struct CKRasterizerObjectDesc
{
    virtual ~CKRasterizerObjectDesc() {}
};

/**************************************************************
//--- Texture Object description structure
***************************************************************/
struct CKTextureDesc : public CKRasterizerObjectDesc
{
    CKDWORD Flags;        // CKRST_TEXTUREFLAGS
    VxImageDescEx Format; // Width/Height/bpp etc...
    CKDWORD MipMapCount;  // Number of mipmap level in the texture

    CKTextureDesc() : Flags(0), MipMapCount(0) {}
    virtual ~CKTextureDesc() {}
};

/*******************************************************************
Sprites can be implemented as a set of square sub textures
This structure holds the position and size of such a sub texture
*******************************************************************/
struct CKSPRTextInfo
{
    CKDWORD IndexTexture; // Texture object
    short int x;          //	Position
    short int y;          //		"
    short int w;          //	Size in the sprite
    short int h;          //  "    "
    short int sw;         //	Real texture size
    short int sh;         //   "    "
};

/*************************************************************
Sprite description structure
A sprite is in fact a set of pow2 textures...
************************************************************/
struct CKSpriteDesc : public CKTextureDesc
{
    XArray<CKSPRTextInfo> Textures; // Sub textures making this sprite and their placement
    CKRasterizer *Owner;            //

    CKSpriteDesc() : CKTextureDesc(), Textures(), Owner(NULL) {}
    virtual ~CKSpriteDesc() {};
};

/*************************************************************
Vertex Shader description structure (Empty , only used in DX8)
************************************************************/
struct CKVertexShaderDesc : public CKRasterizerObjectDesc
{
    CKDWORD *m_Function;
    CKDWORD m_FunctionSize;

    CKVertexShaderDesc() : m_Function(NULL) {}
    virtual ~CKVertexShaderDesc() {}
};

/*************************************************************
Pixel Shader description structure (Empty , only used in DX8)
************************************************************/
struct CKPixelShaderDesc : public CKRasterizerObjectDesc
{
    CKDWORD *m_Function;
    CKDWORD m_FunctionSize;

    CKPixelShaderDesc() : m_Function(NULL) {}
    virtual ~CKPixelShaderDesc() {}
};

/***********************************************************
//---- Vertex Buffer Description :
************************************************************/
struct CKVertexBufferDesc : CKRasterizerObjectDesc
{
    CKDWORD m_Flags;          // CKRST_VBFLAGS
    CKDWORD m_VertexFormat;   // Vertex format : CKRST_VERTEXFORMAT
    CKDWORD m_MaxVertexCount; // Max number of vertices this buffer can contain
    CKDWORD m_VertexSize;     // Size in bytes taken by a vertex..
    CKDWORD m_CurrentVCount;  // For dynamic buffers, current number of vertices taken in this buffer

    CKVertexBufferDesc()
    {
        m_Flags = m_VertexFormat = m_MaxVertexCount = m_VertexSize = m_CurrentVCount = 0;
    }
    virtual ~CKVertexBufferDesc() {}

    CKVertexBufferDesc &operator=(const CKVertexBufferDesc &b)
    {
        m_Flags = b.m_Flags;
        m_VertexFormat = b.m_VertexFormat;
        m_MaxVertexCount = b.m_MaxVertexCount;
        m_VertexSize = b.m_VertexSize;
        m_CurrentVCount = b.m_CurrentVCount;
        return *this;
    }
};

/***********************************************************
//---- Index Buffer Description :
************************************************************/
struct CKIndexBufferDesc : CKRasterizerObjectDesc
{
    CKDWORD m_Flags;         // CKRST_VBFLAGS
    CKDWORD m_MaxIndexCount; // Max number of indices this buffer can contain
    CKDWORD m_CurrentICount; // For dynamic buffers, current number of indices taken in this buffer

    CKIndexBufferDesc()
    {
        m_Flags = m_MaxIndexCount = m_CurrentICount = 0;
    }
    virtual ~CKIndexBufferDesc() {}

    CKIndexBufferDesc &operator=(const CKIndexBufferDesc &b)
    {
        m_Flags = b.m_Flags;
        m_MaxIndexCount = b.m_MaxIndexCount;
        m_CurrentICount = b.m_CurrentICount;
        return *this;
    }
};

//--- Default format of a prelit vertex (Position,Colors,and texture coordinates)
struct CKVertex
{
    VxVector4 V;
    CKDWORD Diffuse;
    CKDWORD Specular;
    float tu, tv;
};

/***************************
Render State cache values
***************************/
enum RSCache
{
    RSC_VALID      = 1, // Current render state value is valid in cache
    RSC_LOCKED     = 2, // Render state can not be changed
    RSC_DISABLED   = 4, // Render state is disabled (if it does not exist in a given implementation )
    RSC_OVERRIDDEN = 8  // Implementation can have special processing to do for a given render state
};

/**************************************************
//--- Used by the render state cache
***************************************************/
struct CKRenderStateData
{
    CKDWORD Value; // Current Value for this render state
    CKDWORD Valid; // Can value be modified...
    CKDWORD Flag;
    CKDWORD DefaultValue; // Default Value for this render state
};

#endif // CKRASTERIZERTYPES_H