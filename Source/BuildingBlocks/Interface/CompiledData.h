#ifndef CK_COMPILEDDATA_H
#define CK_COMPILEDDATA_H

#include "CKAll.h"

class CompiledTextData
{
public:
    CompiledTextData();
    ~CompiledTextData();

    void ClearStructure();
    void PrepareStructure();
    void ExtendStructure(int nc);

    VxDrawPrimitiveData *GetStructure(CKRST_DPFLAGS Flags, int VertexCount);
    CKWORD *GetIndices(int IndicesCount);
    void PatchIndices(int initialsize, int newsize);
    void Render(CKRenderContext *dev);

    VxRect m_DrawZone;

protected:
    VxDrawPrimitiveData m_DrawPrimData;
    VxDrawPrimitiveData m_DrawPrimDataUser;
    XArray<CKWORD> m_Indices;
};

#endif