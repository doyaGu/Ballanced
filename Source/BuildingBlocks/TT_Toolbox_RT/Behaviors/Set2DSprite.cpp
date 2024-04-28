///////////////////////////////////
///////////////////////////////////
//
//        TT Set_2DSprite
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet2DSpriteDecl();
CKERROR CreateSet2DSpriteProto(CKBehaviorPrototype **pproto);
int Set2DSprite(const CKBehaviorContext &behcontext);
CKERROR Set2DSpriteCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_2DSprite");
    od->SetDescription("Sets position, size and UV mapping for sprite");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3c392558, 0x419d2680));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DSpriteProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_2DSprite");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("UV-Rect", CKPGUID_RECT, "0,0,1,1");

    proto->DeclareSetting("Add Input:", CKPGUID_2DSPRITEINPUT, "UV-Mapping,Size,Position");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DSprite);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set2DSpriteCallBack);

    *pproto = proto;
    return CK_OK;
}

#define INPUT_POSITION 4
#define INPUT_SIZE 2
#define INPUT_UV 1

int Set2DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CK2dEntity *target = (CK2dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    int input = 0;
    beh->GetLocalParameterValue(0, &input);

    int inPos = 0;

    Vx2DVector position;
    if (input & INPUT_POSITION)
    {
        beh->GetInputParameterValue(inPos++, &position);
    }
    else
    {
        target->GetPosition(position, TRUE);
    }

    Vx2DVector size;
    if (input & INPUT_SIZE)
    {
        beh->GetInputParameterValue(inPos++, &size);
    }
    else
    {
        target->GetSize(size, TRUE);
    }

    if (input & INPUT_UV)
    {
        VxRect uv;
        beh->GetInputParameterValue(inPos++, &uv);
        target->SetSourceRect(uv);
    }

    VxRect rect(position.x, position.y, position.x + size.x, position.y + size.y);
    target->SetHomogeneousCoordinates();
    target->SetHomogeneousRect(rect);

    return CKBR_OK;
}

CKERROR Set2DSpriteCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CallbackMessage == CKM_BEHAVIORSETTINGSEDITED)
    {
        int input = 0;
        beh->GetLocalParameterValue(0, &input);
        for (int i = beh->GetInputCount() - 1; i >= 0; --i)
        {
            CKParameterIn *pin = beh->RemoveInputParameter(i);
            CKDestroyObject(pin);
        }

        if (input & INPUT_POSITION)
        {
            beh->CreateInputParameter("Position", CKPGUID_2DVECTOR);
        }

        if (input & INPUT_SIZE)
        {
            beh->CreateInputParameter("Size", CKPGUID_2DVECTOR);
        }

        if (input & INPUT_UV)
        {
            beh->CreateInputParameter("UV-Rect", CKPGUID_RECT);
        }
    }

    return CKBR_OK;
}