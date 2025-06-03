#include "CKBaseManager.h"

#define FONT_MANAGER_GUID CKGUID(0x64fb5810, 0x73262d3b)

class CKTextureFont;

class CKFontManager : public CKBaseManager
{
    ////////////////////////////////////////////////////////
    //                Public Part                       ////
    ////////////////////////////////////////////////////////
public:
    ///
    // Virtuals Override

    // Initialization
    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKEnd();

    virtual CKERROR SequenceToBeDeleted(CK_ID *objids, int count);

    // Pause Resume
    virtual CKERROR OnCKReset();

    virtual CKERROR PostClearAll();
    virtual CKERROR PreProcess();
    virtual CKERROR PostLoad();

    virtual CKERROR LoadData(CKStateChunk *chunk, CKFile *LoadedFile);
    virtual CKStateChunk *SaveData(CKFile *SavedFile);

    virtual CKERROR OnPostRender(CKRenderContext *dev);
    virtual CKERROR OnPostSpriteRender(CKRenderContext *dev);

    virtual CKDWORD GetValidFunctionsMask() { return CKMANAGER_FUNC_OnSequenceToBeDeleted |
                                                     CKMANAGER_FUNC_PostClearAll |
                                                     CKMANAGER_FUNC_OnCKInit |
                                                     CKMANAGER_FUNC_PostLoad |
                                                     CKMANAGER_FUNC_OnCKEnd |
                                                     CKMANAGER_FUNC_OnCKReset |
                                                     CKMANAGER_FUNC_PreProcess |
                                                     CKMANAGER_FUNC_OnPostRender |
                                                     CKMANAGER_FUNC_OnPostSpriteRender; }

    // Methods

    // Create a font from a texture
    virtual int CreateTextureFont(CKSTRING FontName, CKTexture *fonttexture, VxRect &tzone, Vx2DVector &charnumber, CKBOOL fixed = TRUE, int firstcharacter = 0, float iSpaceSize = 0.3f);

    // Get a font index
    virtual int GetFontIndex(CKSTRING name);

    // Get a font
    virtual CKTextureFont *GetFont(unsigned int fontindex);

    // Create a logical font from a system font name
    virtual CKBOOL CreateFont(CKSTRING fontName, int systemFontIndex, int weigth, CKBOOL italic, CKBOOL underline, int resolution, int iForcedSize);

    // Create a texture from a logical font
    virtual CKTexture *CreateTextureFromFont(int systemFontIndex, int resolution, CKBOOL extended, CKBOOL bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iFontSize = 0);

    // Draw a text
    virtual void DrawText(CKRenderContext *iRC, int iFontIndex, const char *iText, const Vx2DVector &iPosition, const Vx2DVector &iScale, CKDWORD iStartColor, CKDWORD iEndColor);

    // font deletion
    virtual void DeleteFont(int iFontIndex);
};
