///////////////////////////////////////
///////////////////////////////////////
//
//        TT Set_2DSprite_old
//
///////////////////////////////////////
///////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet2DSpriteOldDecl();
CKERROR CreateSet2DSpriteOldProto(CKBehaviorPrototype **pproto);
int Set2DSpriteOld(const CKBehaviorContext &behcontext);
CKERROR Set2DSpriteOldCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DSpriteOldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_2DSprite_old");
    od->SetDescription("Old! Sets position, size and UV mapping for sprite");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56680b62, 0x23083054));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DSpriteOldProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DSpriteOldProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_2DSprite_old");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("UV Left,Rigth", CKPGUID_2DVECTOR, "0,1");
    proto->DeclareInParameter("UV Top,Bottom", CKPGUID_2DVECTOR, "0,1");

    proto->DeclareSetting("Add Input:", CKPGUID_2DSPRITEINPUT, "UV-Mapping,Size,Position");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DSpriteOld);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set2DSpriteOldCallBack);

    *pproto = proto;
    return CK_OK;
}

int Set2DSpriteOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CK2dEntity *target = (CK2dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    // Get settings flags (what inputs to use)
    int flags = 0;
    beh->GetLocalParameterValue(0, &flags);

    int paramIndex = 0;

    Vx2DVector position;
    position.x = 0.0f;
    position.y = 0.0f;

    // Check if Position input is enabled (flag 4)
    if (flags >= 4)
    {
        flags -= 4;
        beh->GetInputParameterValue(paramIndex++, &position);
    }
    else
    {
        // Use current position
        target->GetPosition(position, TRUE);
    }

    Vx2DVector size;
    size.x = 0.0f;
    size.y = 0.0f;

    // Check if Size input is enabled (flag 2)
    if (flags >= 2)
    {
        flags -= 2;
        beh->GetInputParameterValue(paramIndex++, &size);
    }
    else
    {
        // Use current size
        target->GetSize(size, TRUE);
    }

    // Check if UV-Mapping is enabled (flag 1)
    if (flags >= 1)
    {
        VxRect uvRect;
        uvRect.left = 0;
        uvRect.top = 0;
        uvRect.right = 0;
        uvRect.bottom = 0;

        Vx2DVector uvLR, uvTB;
        beh->GetInputParameterValue(paramIndex, &uvLR);
        uvRect.left = uvLR.x;
        uvRect.right = uvLR.y;

        beh->GetInputParameterValue(paramIndex + 1, &uvTB);
        uvRect.top = uvTB.x;
        uvRect.bottom = uvTB.y;

        target->SetSourceRect(uvRect);
    }

    // Set rect (position + size)
    VxRect rect;
    rect.left = position.x;
    rect.top = position.y;
    rect.right = position.x + size.x;
    rect.bottom = position.y + size.y;

    target->SetHomogeneousCoordinates(TRUE);
    target->SetRect(rect);

    return CKBR_OK;
}

CKERROR Set2DSpriteOldCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Callback not used in this implementation
    return CKBR_OK;
}