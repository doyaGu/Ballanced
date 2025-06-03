/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              FilterTexture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "TextureProcessing.h"

CKObjectDeclaration *FillBehaviorFilterTextureDecl();
CKERROR CreateFilterTextureProto(CKBehaviorPrototype **);
int FilterTexture(const CKBehaviorContext &behcontext);
CKERROR FilterTextureCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFilterTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Filter Texture");
    od->SetDescription("Apply a filter matrix to the texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Matrix Row ...: </SPAN>A 3x3 integer matrix representing the filter to apply to the texture.<BR>
    <SPAN CLASS=pin>Scale: </SPAN>An integer to rescaled each value once multiplied by the matrix. 0 means
    the sum of all Matrix terms, for an uniform scaling.<BR>
    <BR>
    This behavior has the same effect than the Custom Filter in Adobe(r) Photoshop(r). You can play around to find
    some interesting matrices. Known matrices are the Gaussian Blur : (1,2,1),(2,4,2),(1,2,1), the Emboss Filter :
    (-1,0,1),(-1,0,1),(-1,0,1), the Low-Pass Filter (0,1,0),(1,3,1),(0,1,0), etc.<BR>
    */
    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x52df323c, 0x39dc1211));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFilterTextureProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateFilterTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Filter Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Matrix Row 1", CKPGUID_VECTOR, "1,2,1");
    proto->DeclareInParameter("Matrix Row 2", CKPGUID_VECTOR, "2,8,2");
    proto->DeclareInParameter("Matrix Row 3", CKPGUID_VECTOR, "1,2,1");
    proto->DeclareInParameter("Scale", CKPGUID_INT, "0");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FilterTexture);

    *pproto = proto;
    return CK_OK;
}

int FilterTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    VxVector v1;
    beh->GetInputParameterValue(0, &v1);
    VxVector v2;
    beh->GetInputParameterValue(1, &v2);
    VxVector v3;
    beh->GetInputParameterValue(2, &v3);

    int matrix[3][3] = {(int)v1.x, (int)v1.y, (int)v1.z, (int)v2.x, (int)v2.y, (int)v2.z, (int)v3.x, (int)v3.y, (int)v3.z};

    int sum = 0;
    beh->GetInputParameterValue(3, &sum);

#ifdef WIN32
    // MMX asm code
    ApplyMatrixToTexture((CKTexture *)beh->GetTarget(), matrix, sum);
#endif
#ifdef macintosh
#pragma message("Should implement Altivec version of ApplyMatrixToTexture")
    ApplyMatrixToTexture((CKTexture *)beh->GetTarget(), matrix, sum);
#endif

    return CKBR_OK;
}
