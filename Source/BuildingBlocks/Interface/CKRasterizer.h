/**
 * @file CKRasterizer.h
 * @brief The base classes declaration for rasterizer.
 *
 * The default implementation of the render engine in Virtools use rasterizer as a common interface to
 * access a render device.
 *
 * A Rasterizer DLL must provide the implementation for the 3 base classes by overloading them:
 *  - CKRasterizer: Top level class (One instance per rasterizer) which upon creation should check for available driver.
 *  - CKRasterizerDriver: One instance should be created for each available driver (graphic card, software, hardware,
 *  etc.). It is used to store capabilities and create rendering contexts.
 *  - CKRasterizerContext: A rendering context with all the methods to create/load textures, setup lights, materials,
 *  render states, and draw primitives.
 */
#ifndef CKRASTERIZER_H
#define CKRASTERIZER_H

#include "VxDefines.h"
#include "VxMath.h"
#include "CKRasterizerEnums.h"
#include "CKRasterizerTypes.h"

/**
 * The render engine will call the CKRasterizerGetInfo function
 * to gain access to rasterizer information. This information should be
 * stored in CKRasterizerInfo structure.
 */
struct CKRasterizerInfo
{
    XString DllName;              /// Filled by the render engine when parsing Dlls
    XString Desc;                 /// Description for this rasterizer (eg: "Open GL Rasterizer")
    INSTANCE_HANDLE DllInstance;  /// Filled by the render engine when loading the DLL
    CKRST_STARTFUNCTION StartFct; /// A pointer to a function that will create the CKRasterizer Instance
    CKRST_CLOSEFUNCTION CloseFct; /// A pointer to a function that will destroy the CKRasterizer Instance
    CKRST_OPTIONFCT OptionFct;

    CKRasterizerInfo()
    {
        DllInstance = NULL;
        StartFct = NULL;
        CloseFct = NULL;
        OptionFct = NULL;
    }
};

/**
 * There is only one function that a rasterizer dll is supposed to export: "CKRasterizerGetInfo",
 * it will be used by the render engine to retrieve information about the plugin.
 */
typedef void (*CKRST_GETINFO)(CKRasterizerInfo *);

/// Main class for rasterizer declaration
/**
 * A render engine is started by calling CKRasterizerStart which will try to create a CKRasterizer and initialize it.
 *
 * It will enumerate and create the available drivers on the system.
 *
 * If no driver for 3D rendering is available, the function should fail and return NULL.
 *
 * The CKRasterizer object is only used to
 *  - Generate texture, sprites, and vertex buffer index
 *    (which are simple CKDWORDs to identify a texture or VB across different contexts or drivers).
 *  - Access the list of available driver
 *
 *  Several rasterizers can work together in which case texture indices should be the same across them.
 *  Each rasterizer must warn the others of its existence through LinkRasterizer.
 *  (It is automatically called by the render engine upon registration of the rasterizers)
 */
class CKRasterizer
{
public:
    CKRasterizer();

    virtual ~CKRasterizer();

    //---- Initialisation/Destruction
    virtual CKBOOL Start(WIN_HANDLE AppWnd);
    virtual void Close() {}

    //--- Available drivers access
    virtual int GetDriverCount() { return m_Drivers.Size(); }
    virtual CKRasterizerDriver *GetDriver(CKDWORD index) { return m_Drivers[index]; }

    //--- Texture,Sprite,Vertex buffer Index Creation
    //--- Index are shared amongst all contexts
    virtual CKDWORD CreateObjectIndex(CKRST_OBJECTTYPE Type, CKBOOL WarnOthers = TRUE);
    virtual CKBOOL ReleaseObjectIndex(CKDWORD ObjectIndex, CKRST_OBJECTTYPE Type, CKBOOL WarnOthers = TRUE);
    virtual CKBYTE *AllocateObjects(int size);

    //--- Keep a track of other rasterizers
    void LinkRasterizer(CKRasterizer *rst);
    void RemoveLinkedRasterizer(CKRasterizer *rst);

    //----- Buggy driver information
    CKBOOL LoadVideoCardFile(const char *FileName);
    CKDriverProblems *FindDriverProblems(const XString &Vendor, const XString &Renderer, const XString &Version,
                                         const XString &DeviceDesc, int Bpp);

public:
    CKRasterizerContext *m_FullscreenContext; // If a context is currently fullscreen...
    VxMemoryPool m_Objects;
    XSArray<CKBYTE> m_ObjectsIndex; // A List of CKBYTE mask to keep track of the existing object
    CKDWORD m_FirstFreeIndex[6];    // First Free Index for each type of objects...

    XSArray<CKRasterizer *> m_OtherRasterizers; // The list of other rasterizers..
    WIN_HANDLE m_MainWindow;                    // Application main window used to preform initialisations

    XClassArray<CKDriverProblems> m_ProblematicDrivers; // List of driver with identified problems
    XArray<CKRasterizerDriver *> m_Drivers;

    // Implementation specific data to follow....
};

/// Rasterizer driver abstraction class
/**
 * Once a rasterizer is created and started it enumerates all available drivers depending on the number of
 * graphic adapters installed and their different implementation (Hardware, software, hardware transform & lighting,
 * etc.).
 *
 * Once a driver is chosen (according to its capabilities for example) it can be used to create one or more contexts
 * for drawing
 * \sa \ref CreateContext
 * \sa \ref CKRasterizerContext
 */
class CKRasterizerDriver
{
public:
    CKRasterizerDriver();

    virtual ~CKRasterizerDriver();

    //--- Contexts creation
    virtual CKRasterizerContext *CreateContext();
    virtual CKBOOL DestroyContext(CKRasterizerContext *Context);

public:
    void InitNULLRasterizerCaps(CKRasterizer *Owner);

    CKBOOL m_Hardware;                           // Hardware accelerated driver ?
    CKBOOL m_CapsUpToDate;                       // Driver Capabilities are up-to-date ?
    CKRasterizer *m_Owner;                       // Owner CKRasterizer object
    CKDWORD m_DriverIndex;                       // Index of this driver in the Rasterizer List of drivers
    XArray<VxDisplayMode> m_DisplayModes;        // List of available display modes
    XClassArray<CKTextureDesc> m_TextureFormats; // List of available texture formats
    Vx3DCapsDesc m_3DCaps;                       // 3D capabilities
    Vx2DCapsDesc m_2DCaps;                       // 2D capabilities
    XString m_Desc;                              // Description string
    XArray<CKRasterizerContext *> m_Contexts;    // Currently created render contexts
    CKBOOL m_Stereo;                             // Support stereoscopic  ?
};

/**
 * This utility function converts the attenuation factors from DX5 model.
 * A(d) = a0 + a1 * (1 - d / R) + a2 * (1 - d / R)
 * To DX7 or GL attenuation model
 * A(d) = 1 / (a0 + a1.d + a2.d)
 *
 * \remark the result is not exactly the same.
 */
void ConvertAttenuationModelFromDX5(float &_a0, float &_a1, float &_a2, float range);

/**
 * This utility function returns the Vertex Buffer Format (CKRST_VERTEXFORMAT) and the size of a vertex from the given
 * draw primitive options.
 */
CKDWORD CKRSTGetVertexFormat(CKRST_DPFLAGS DpFlags, CKDWORD &VertexSize);

/**
 * This utility function returns the size of a vertex from a given CKRST_VERTEXFORMAT format.
 */
CKDWORD CKRSTGetVertexSize(CKDWORD VertexFormat);

/**
 * This utility function returns the new location of the current VBuffer memory pointer.
 * Copy the content of a DrawPrimitive Data structure inside a Vertex Buffer memory buffer.
 * No assumption or tests are made to check the coherence between the vertex format and incoming data.
 */
CKBYTE *CKRSTLoadVertexBuffer(CKBYTE *VBMem, CKDWORD VFormat, CKDWORD VSize, VxDrawPrimitiveData *data);

/**
 * This utility function setups the DpData structure (data pointers and stride only)
 * according the DpData.Flags and VBMem pointer...
 */
void CKRSTSetupDPFromVertexBuffer(CKBYTE *VBMem, CKVertexBufferDesc *VB, VxDrawPrimitiveData &DpData);

/// Rasterizer context abstraction class
/**
 * A context is used to identify where the rendering take place and to specify how primitives should be drawn.
 *
 * A context is created by its driver, either fullscreen or windowed.
 *
 * A context is also used to create textures,sprites or vertex buffer and to load their context.
 * When accessing to textures, sprites or vertex buffer, you must use an index identifying the object,
 * this object must have been created previously by \ref CreateObjectIndex.
 * The object index is shared across all rasterizer object, so there is no need to recreate it when destroying/creating
 * contexts.
 *
 * Several methods are here to set the different parameters of the render engine:
 * Lighting states(Material and Light), Viewport States, Transformation states, Render States
 */
class CKRasterizerContext
{
public:
    //--- Construction/destruction
    CKRasterizerContext();

    virtual ~CKRasterizerContext();

    //--- Creation (destruction is done by deleting the context CKRasterizerDriver::DestroyContext)
    virtual CKBOOL Create(WIN_HANDLE Window, int PosX = 0, int PosY = 0, int Width = 0, int Height = 0, int Bpp = -1,
                          CKBOOL Fullscreen = FALSE, int RefreshRate = 0, int Zbpp = -1, int StencilBpp = -1) { return TRUE; }

    //---
    virtual CKBOOL Resize(int PosX = 0, int PosY = 0, int Width = 0, int Height = 0, CKDWORD Flags = 0) { return TRUE; }
    virtual CKBOOL Clear(CKDWORD Flags = CKRST_CTXCLEAR_ALL, CKDWORD Ccol = 0, float Z = 1.0f, CKDWORD Stencil = 0, int RectCount = 0,
                         CKRECT *rects = NULL) { return FALSE; }
    virtual CKBOOL BackToFront(CKBOOL vsync) { return FALSE; }

    //------------------------------------------------------
    //--- Starting/stopping primitive drawing
    virtual CKBOOL BeginScene() { return FALSE; }
    virtual CKBOOL EndScene() { return FALSE; }

    //----------------------------------------------------
    //--- Lighting & Material States
    virtual CKBOOL SetLight(CKDWORD LightIndex, CKLightData *data)
    {
        if (data && LightIndex < RST_MAX_LIGHT)
            m_CurrentLightData[LightIndex] = *data;
        return FALSE;
    }
    virtual CKBOOL EnableLight(CKDWORD LightIndex, CKBOOL Enable) { return FALSE; }
    virtual CKBOOL SetMaterial(CKMaterialData *mat);

    //-----------------------------------------------------
    //--- Viewport size and position
    virtual CKBOOL SetViewport(CKViewportData *data);

    //--- Transformation Matrix (World,View or projection )
    virtual CKBOOL SetTransformMatrix(VXMATRIX_TYPE Type, const VxMatrix &Mat);

    //----------------------------------------------------------
    //--- Rendering states
    //	To avoid redundant render state calls,a render state cache can be used by
    //	implementation of CKRasterizerContext by first calling CKRasterizerContext::InternalSetRenderState() before
    //	actually setting the render state (implemented in .h file to be inlined in implementations)
    virtual CKBOOL SetRenderState(VXRENDERSTATETYPE State, CKDWORD Value)
    {
        return InternalSetRenderState(State, Value);
    }
    virtual CKBOOL GetRenderState(VXRENDERSTATETYPE State, CKDWORD *Value)
    {
        return InternalGetRenderState(State, Value);
    }

    //----------------------------------------------------------
    //--- Display lists
    virtual CKBOOL NewDisplayList() { return FALSE; }
    virtual CKBOOL EndDisplayList() { return FALSE; }
    virtual CKBOOL DeleteDisplayList(int List) { return FALSE; }
    virtual CKBOOL CallDisplayList(int List) { return FALSE; }

    //-------------------------------------------------------------
    //--- Texture States control the texture and its aspects (filtering, etc.)
    //--- Set the current texture to be used for rendering (0 if no texturing)
    virtual CKBOOL SetTexture(CKDWORD Texture, int Stage = 0) { return FALSE; }
    virtual CKBOOL SetTextureStageState(int Stage, CKRST_TEXTURESTAGESTATETYPE Tss, CKDWORD Value) { return FALSE; }

    //-------------------------------------------------------------
    //--- Vertex & pixel shaders are created as textures and sprites
    //--- calling these functions with 0 as argument disables the
    //--- usage of programmable shaders and uses the default pipeline
    virtual CKBOOL SetVertexShader(CKDWORD VShaderIndex) { return FALSE; }
    virtual CKBOOL SetPixelShader(CKDWORD PShaderIndex) { return FALSE; }
    virtual CKBOOL SetVertexShaderConstant(CKDWORD Register, const void *Data, CKDWORD CstCount) { return FALSE; }
    virtual CKBOOL SetPixelShaderConstant(CKDWORD Register, const void *Data, CKDWORD CstCount) { return FALSE; }

    //----------------------------------------------------------------
    //--- Primitive Drawing (From system memory data or Vertex buffer)
    // At least a system memory (DrawPrimitive) version must be supported by an implementation
    virtual CKBOOL DrawPrimitive(VXPRIMITIVETYPE pType, CKWORD *indices, int indexcount, VxDrawPrimitiveData *data) { return FALSE; }
    virtual CKBOOL DrawPrimitiveVB(VXPRIMITIVETYPE pType, CKDWORD VertexBuffer, CKDWORD StartIndex, CKDWORD VertexCount,
                                   CKWORD *indices = NULL, int indexcount = NULL) { return FALSE; }
    virtual CKBOOL DrawPrimitiveVBIB(VXPRIMITIVETYPE pType, CKDWORD VB, CKDWORD IB, CKDWORD MinVIndex, CKDWORD VertexCount,
                                     CKDWORD StartIndex, int Indexcount) { return FALSE; }

    //-------------------------------------------------------------
    //--- Creation of Textures, Sprites and Vertex Buffer
    //--- Once an object index exists, and before it can be used a texture,sprite or VB
    //--- must be created with this method, The Desired format pointer must point to
    //--- a CKTextureDesc , CKSpriteDesc , CKVertexBufferDesc ,CKIndexBufferDesc structure according to
    //--- the type of object being created...
    virtual CKBOOL CreateObject(CKDWORD ObjIndex, CKRST_OBJECTTYPE Type, void *DesiredFormat) { return FALSE; }
    virtual CKBOOL DeleteObject(CKDWORD ObjIndex, CKRST_OBJECTTYPE Type);
    virtual CKBOOL FlushObjects(CKDWORD TypeMask);
    virtual void UpdateObjectArrays(CKRasterizer *rst);

    //-------------------------------------------------------------
    //--- Textures
    virtual CKBOOL LoadTexture(CKDWORD Texture, const VxImageDescEx &SurfDesc, int miplevel = -1) { return FALSE; }
    virtual CKTextureDesc *GetTextureData(CKDWORD Texture);
    //--- Copy the content of this context to a texture
    virtual CKBOOL CopyToTexture(CKDWORD Texture, VxRect *Src, VxRect *Dest,
                                 CKRST_CUBEFACE Face = CKRST_CUBEFACE_XPOS) { return NULL; }
    //--- Try to set a texture as the target for rendering
    virtual CKBOOL SetTargetTexture(CKDWORD TextureObject, int Width = 0, int Height = 0, CKRST_CUBEFACE Face = CKRST_CUBEFACE_XPOS,
                                    CKBOOL GenerateMipMap = FALSE) { return FALSE; }

    //-------------------------------------------------------------
    //--- Sprites
    virtual CKBOOL LoadSprite(CKDWORD Sprite, const VxImageDescEx &SurfDesc);
    virtual CKSpriteDesc *GetSpriteData(CKDWORD Sprite);
    //-- Draw the sprite on the context using the given source and destination rectangles
    virtual CKBOOL DrawSprite(CKDWORD Sprite, VxRect *src, VxRect *dst) { return FALSE; }

    //-------------------------------------------------------------
    //--- Vertex Buffers
    virtual void *LockVertexBuffer(CKDWORD VB, CKDWORD StartVertex, CKDWORD VertexCount,
                                   CKRST_LOCKFLAGS Lock = CKRST_LOCK_DEFAULT) { return NULL; }
    virtual CKBOOL UnlockVertexBuffer(CKDWORD VB) { return FALSE; }
    virtual CKVertexBufferDesc *GetVertexBufferData(CKDWORD VB);
    virtual CKBOOL OptimizeVertexBuffer(CKDWORD VB) { return FALSE; }

    //-------------------------------------------------------------
    //--- Copy the content of this rendering context to a memory buffer	(CopyToMemoryBuffer)
    //--- or Updates this rendering context with the content of a memory buffer	(CopyFromMemoryBuffer)
    virtual int CopyToMemoryBuffer(CKRECT *rect, VXBUFFER_TYPE buffer, VxImageDescEx &img_desc) { return 0; }
    virtual int CopyFromMemoryBuffer(CKRECT *rect, VXBUFFER_TYPE buffer, const VxImageDescEx &img_desc) { return 0; }

    //-------------------------------------------------------------
    //--- Each implementation can return here a pointer
    //--- to a structure containing its specific data
    virtual void *GetImplementationSpecificData() { return NULL; }

    //-----------------------------------------------------------------
    //--- Transform a set of vertices using the current transformation matrices
    //--- from a local coordinate system to screen and/or homogenous coordinates
    virtual CKBOOL TransformVertices(int VertexCount, VxTransformData *Data);

    //-----------------------------------------------------------------
    //--- Computes the visibility of a box in the current viewport
    //--- and also computes its screen extents if asked.
    virtual CKDWORD ComputeBoxVisibility(const VxBbox &box, CKBOOL World = FALSE, VxRect *extents = NULL);

    //-----------------------------------------------------------------
    //--- When using threads one must warn the context before/after using its methods of the active calling thread.
    //--- (mainly for OpenGL implementation to work correctly on multi-thread applications)
    virtual CKBOOL WarnThread(CKBOOL Enter) { return FALSE; }

    //-----------------------------------------------------------------
    //--- For web content the render context can be transparent (no clear of back buffer but instead
    //--- a copy of what is currently on screen)
    //--- The AddDirtyRect method warns the context that a rectangle had been redrawn
    //--- and should be updated (NULL to update the entire back buffer)
    virtual void SetTransparentMode(CKBOOL Trans) { m_TransparentMode = Trans; }
    virtual void AddDirtyRect(CKRECT *Rect)
    {
        if (!Rect)
            m_CleanAllRects = TRUE;
        else
            m_DirtyRects.PushBack(*Rect);
    }
    virtual void RestoreScreenBackup() {}

    //-----------------------------------------------------------------
    //--- User Clip Plane Function
    virtual CKBOOL SetUserClipPlane(CKDWORD ClipPlaneIndex, const VxPlane &PlaneEquation) { return FALSE; }
    virtual CKBOOL GetUserClipPlane(CKDWORD ClipPlaneIndex, VxPlane &PlaneEquation) { return FALSE; }

    //--------------------------------------------------------------
    // Each rasterizer context
    virtual void InitDefaultRenderStatesValue();

    //--------- Load a cube map texture face
    virtual CKBOOL LoadCubeMapTexture(CKDWORD Texture, const VxImageDescEx &SurfDesc, CKRST_CUBEFACE Face,
                                      int miplevel = -1) { return FALSE; }

    //--------- For rasterizer that supports stereo rendering (OpenGL)
    // set the buffer on which next drawing operations should occur
    virtual CKBOOL SetDrawBuffer(CKRST_DRAWBUFFER_FLAGS Flags) { return FALSE; }

    //-------------------------------------------------------------
    //--- Index Buffers
    virtual void *LockIndexBuffer(CKDWORD IB, CKDWORD StartIndex, CKDWORD IndexCount,
                                  CKRST_LOCKFLAGS Lock = CKRST_LOCK_DEFAULT) { return NULL; }
    virtual CKBOOL UnlockIndexBuffer(CKDWORD IB) { return FALSE; }
    virtual CKIndexBufferDesc *GetIndexBufferData(CKDWORD IB);

public:
    // Default Lib implementation for sprites (create them using sub-textures)
    CKBOOL CreateSprite(CKDWORD Sprite, CKSpriteDesc *DesiredFormat);

    //--- The complete projection matrix is not computed every time a transformation matrix is set...
    // calling this function
    void UpdateMatrices(CKDWORD Flags);

    //-------------------------------------------------------------------------------
    //--- Render state cache management
    //--- in inline functions
    //--- A rasterizer context implementation should always
    //--- call these methods to maintain the render state cache
    //--- which avoid redundant render state calls
    void FlushRenderStateCache();
    void InvalidateStateCache(VXRENDERSTATETYPE State);
    CKDWORD GetRSCacheValue(VXRENDERSTATETYPE State);
    CKBOOL InternalSetRenderState(VXRENDERSTATETYPE State, CKDWORD Value);
    CKBOOL InternalGetRenderState(VXRENDERSTATETYPE State, CKDWORD *Value);

    void ResetDirtyRects()
    {
        m_CleanAllRects = FALSE;
        m_DirtyRects.Resize(0);
    }

    //-------------- Dynamic Vertex buffers --------------
    // This method will create a vertex buffer the first time it is called
    // with a given vertex format and will later on returns this same
    // vertex buffer, so it can be used when drawing dynamic primitives
    // the returned buffer that can be filled and render with DrawPrimitiveVB
    // (Implemented by Lib)
    // AddKey is a value that must be < 255 that can be used to have different
    // dynamic vertex buffer with the same vertex format
    CKDWORD GetDynamicVertexBuffer(CKDWORD VertexFormat, CKDWORD VertexCount, CKDWORD VertexSize, CKDWORD AddKey);

public:
    CKRasterizerDriver *m_Driver; // Driver that was used to create this context

    //----- Size Info
    CKDWORD m_PosX;   // Top left corner of the context (relative to its window)
    CKDWORD m_PosY;   // ....
    CKDWORD m_Width;  // Size of the context
    CKDWORD m_Height; // ....

    //------ Pixel Format
    CKDWORD m_Bpp;                // Color buffer bits per pixel
    CKDWORD m_ZBpp;               // Depth buffer bits per pixel
    CKDWORD m_StencilBpp;         // Stencil buffer bits per pixel
    VX_PIXELFORMAT m_PixelFormat; // Color buffer pixel format

    //------- Fullscreen Info
    CKDWORD m_Fullscreen;  // Currently fullscreen ?
    CKDWORD m_RefreshRate; // Fullscreen refresh rate

    WIN_HANDLE m_Window; // Window on which the rendering occurs.
    CKBOOL m_SceneBegined;

    //------- Transformation matrices (World, View, Projection)
    CKDWORD m_MatrixUptodate;    // Are m_ViewProjMatrix & m_TotalMatrix up-to-date ?
    VxMatrix m_WorldMatrix;      // Local->World transformation matrix
    VxMatrix m_ViewMatrix;       // World->View transformation matrix
    VxMatrix m_ProjectionMatrix; // Projection matrix
    VxMatrix m_ModelViewMatrix;  // World*View
    VxMatrix m_ViewProjMatrix;   // View*Proj
    VxMatrix m_TotalMatrix;      // World*View*Proj (from a local coordinate system to screen)

    //------- Current Viewport Size
    CKViewportData m_ViewportData; // Viewport position and size

    //------- Texture,sprites,Vertex & index buffers,vertex and pixel shaders objects arrays
    XArray<CKTextureDesc *> m_Textures;           // Array of texture specific data (format,data,etc..)
    XArray<CKSpriteDesc *> m_Sprites;             // sprites
    XArray<CKVertexBufferDesc *> m_VertexBuffers; // Vertex Buffers
    XArray<CKIndexBufferDesc *> m_IndexBuffers;   // Index Buffers
    XArray<CKVertexShaderDesc *> m_VertexShaders; // Vertex Shaders
    XArray<CKPixelShaderDesc *> m_PixelShaders;   // Pixel Shaders

    //------- Lighting data
    CKMaterialData m_CurrentMaterialData;
    CKLightData m_CurrentLightData[RST_MAX_LIGHT];

    //---- A special case for the VXRENDERSTATE_INVERSEWINDING render state
    //---- which is maintained by the context
    CKBOOL m_InverseWinding;

    //--- For web content the render context can be transparent (no clear of back buffer but instead
    //--- a copy of what is currently on screen)
    CKBOOL m_TransparentMode;
    CKBOOL m_CleanAllRects;
    XArray<CKRECT> m_DirtyRects;

    //--- Number of VBL to wait before performing
    // the "BackToFront". If (m_PresentInterval == 0) this means an immediate presentation
    // otherwise it is the number of VBL swap before rendering...
    // This value came in replacement for the WaitForVBL parameter of BackToFront
    CKDWORD m_PresentInterval;
    CKDWORD m_CurrentPresentInterval;

    //---- Enables image antialiasing
    CKBOOL m_Antialias;

    //---- For DX8 rasterizer this ensures that vertex shader are supported :
    //---- for example if the graphic card and driver support T&L
    //---- but not vertex shaders in hardware the chosen driver will be an non T&L device.
    //---- Otherwise created device will only support vertex shaders if they are supported in hardware.
    CKBOOL m_EnsureVertexShader;

    //---- Pressing CTRL+ALT+F10 dump the content of the screen,depth and stencil buffer to
    //---- the root of the current hard drive....
    CKBOOL m_EnableScreenDump;

    //-------------------------------------
    // A cache of render state to avoid redundant render state calls
    // for render states
    CKRenderStateData m_StateCache[VXRENDERSTATE_MAXSTATE];
    int m_RenderStateCacheHit;  // Render state already set
    int m_RenderStateCacheMiss; // Render state not in cache

    //----------------------------------------------------------------------
    //--- to avoid redundant call to SetTransform with a unity matrix
    //--- we keep track of all possible matrix that are currently at the
    //--- identity (a combination of CKRST_MATMASK values )
    //--- the default value is 0, and it's the rasterizer implementation
    //--- responsibility to update and use this value.
    CKDWORD m_UnityMatrixMask;
};

/*******************************************************************************
Render State cache management
*******************************************************************************/
inline void CKRasterizerContext::FlushRenderStateCache()
{
    for (int i = 0; i < VXRENDERSTATE_MAXSTATE; ++i)
    {
        m_StateCache[i].Valid = FALSE;
        m_StateCache[i].Flag = FALSE;
        m_StateCache[i].Value = m_StateCache[i].DefaultValue;
    }
}

inline void CKRasterizerContext::InvalidateStateCache(VXRENDERSTATETYPE State)
{
    m_StateCache[State].Valid = FALSE;
}

inline CKDWORD CKRasterizerContext::GetRSCacheValue(VXRENDERSTATETYPE State)
{
    return m_StateCache[State].Value;
}

inline CKBOOL CKRasterizerContext::InternalSetRenderState(VXRENDERSTATETYPE State, CKDWORD Value)
{
    if (m_StateCache[State].Flag)
        return TRUE;
    if (m_StateCache[State].Valid && (m_StateCache[State].Value == Value))
    {
        m_RenderStateCacheHit++;
        return TRUE;
    }
    else
    {
        m_RenderStateCacheMiss++;
        m_StateCache[State].Value = Value;
        m_StateCache[State].Valid = TRUE;
        return FALSE;
    }
}

// Returns the value of cached render state or FALSE if the cache is invalid
inline CKBOOL CKRasterizerContext::InternalGetRenderState(VXRENDERSTATETYPE State, CKDWORD *Value)
{
    if (m_StateCache[State].Valid)
    {
        *Value = m_StateCache[State].Value;
        return TRUE;
    }
    else
    {
        *Value = m_StateCache[State].DefaultValue;
        return FALSE;
    }
}

/**************************************************
Small util to get the position of a given bit...
*****************************************************/
inline int GetFirstBitpos(CKDWORD data)
{
#ifdef WIN32
    __asm
    {
        mov eax, data
        and eax, eax
        jz exitnow
        bsf eax, eax
        inc eax
        exitnow:
    }
#else
    CKDWORD Lsb = 0;
    if (!data)
        return 0;
    while (!(data & 1))
    {
        data >>= 1;
        Lsb++;
    }
    return Lsb + 1;
#endif
}

inline int ObjTypeIndex(unsigned int objType)
{
#ifdef WIN32
    __asm
    {
        mov eax, objType
        bsf eax, eax
    }
#else
    int index = 0;
    if (objType == 0)
        return 0;
    while (!(objType & 1))
    {
        objType >>= 1;
        ++index;
    }
    return index;
#endif
}

#endif // CKRASTERIZER_H