//////////////////////////////////////
//////////////////////////////////////
//
//        TT_IsObjectVisible
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorIsObjectVisibleDecl();
CKERROR CreateIsObjectVisibleProto(CKBehaviorPrototype **pproto);
int IsObjectVisible(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsObjectVisibleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsObjectVisible");
    od->SetDescription("Tests whether object is visible");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39cf3dd3, 0x11a92281));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsObjectVisibleProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsObjectVisibleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsObjectVisible");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsObjectVisible);

    *pproto = proto;
    return CK_OK;
}

int IsObjectVisible(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBehavior *object = (CKBehavior *)beh->GetInputParameterObject(0);
    if (!object)
    {
        behcontext.Context->OutputToConsole("no Object!");
        return CKBR_PARAMETERERROR;
    }

    if (object->IsVisible())
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