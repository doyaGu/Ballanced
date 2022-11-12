/////////////////////////////////////
/////////////////////////////////////
//
//        TT_IsObjectActive
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorIsObjectActiveDecl();
CKERROR CreateIsObjectActiveProto(CKBehaviorPrototype **pproto);
int IsObjectActive(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsObjectActiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsObjectActive");
    od->SetDescription("Tests whether object is active");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x409f03d9, 0x7616062c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsObjectActiveProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsObjectActiveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsObjectActive");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsObjectActive);

    *pproto = proto;
    return CK_OK;
}

int IsObjectActive(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBehavior *object = (CKBehavior *)beh->GetInputParameterObject(0);
    if (!object)
    {
        behcontext.Context->OutputToConsole("no Object!");
        return CKBR_PARAMETERERROR;
    }

    if (behcontext.Context->GetCurrentScene()->IsObjectActive(object))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
        beh->ActivateOutput(1, FALSE);
    }
    else
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, FALSE);
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}