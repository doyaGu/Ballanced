///////////////////////////////////
///////////////////////////////////
//
//        TT Set_3DSprite
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet3DSpriteDecl();
CKERROR CreateSet3DSpriteProto(CKBehaviorPrototype **pproto);
int Set3DSprite(const CKBehaviorContext &behcontext);
CKERROR Set3DSpriteCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet3DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_3DSprite");
    od->SetDescription("Sets position, size, offset and UV mapping for 3D sprite");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x408e30b5, 0x22a07ada));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet3DSpriteProto);
    od->SetCompatibleClassId(CKCID_SPRITE3D);
    return od;
}

CKERROR CreateSet3DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_3DSprite");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Offset", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("UV Left,Top", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("UV Right,Bottom", CKPGUID_2DVECTOR, "1,1");

    proto->DeclareSetting("Add Input:", CKPGUID_3DSPRITEINPUT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set3DSprite);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set3DSpriteCallBack);

    *pproto = proto;
    return CK_OK;
}

int Set3DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CKSprite3D *target = (CKSprite3D *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    // Get settings flags (what inputs to use)
    int flags = 0;
    beh->GetLocalParameterValue(0, &flags);

    int paramIndex = 0;

    // Check if Position is enabled (flag 8)
    if (flags >= 8)
    {
        flags -= 8;
        VxVector position;
        beh->GetInputParameterValue(paramIndex++, &position);
        target->SetPosition(&position);
    }

    // Check if Size is enabled (flag 4)
    Vx2DVector size;
    size.x = 1.0f;
    size.y = 1.0f;
    if (flags >= 4)
    {
        flags -= 4;
        beh->GetInputParameterValue(paramIndex++, &size);
        target->SetSize(size);
    }

    // Check if Offset is enabled (flag 2)
    Vx2DVector offset;
    offset.x = 0.0f;
    offset.y = 0.0f;
    if (flags >= 2)
    {
        flags -= 2;
        beh->GetInputParameterValue(paramIndex++, &offset);
        target->SetOffset(offset);
    }

    // Check if UV-Mapping is enabled (flag 1)
    if (flags >= 1)
    {
        VxRect uvRect;
        uvRect.left = 0;
        uvRect.top = 0;
        uvRect.right = 0;
        uvRect.bottom = 0;

        Vx2DVector uvLT, uvRB;
        beh->GetInputParameterValue(paramIndex, &uvLT);
        uvRect.left = uvLT.x;
        uvRect.top = uvLT.y;

        beh->GetInputParameterValue(paramIndex + 1, &uvRB);
        uvRect.right = uvRB.x;
        uvRect.bottom = uvRB.y;

        target->SetUVMapping(uvRect);
    }

    return CKBR_OK;
}

CKERROR Set3DSpriteCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Callback not used in this implementation
    return CKBR_OK;
}