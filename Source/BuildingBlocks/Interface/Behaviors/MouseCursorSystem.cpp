/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MouseCursorSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMouseCursorSystemDecl();
CKERROR CreateMouseCursorSystemProto(CKBehaviorPrototype **);
CKERROR MouseCursorSystemCallback(const CKBehaviorContext &behcontext);
int MouseCursorSystem(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMouseCursorSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mouse Cursor System");
    od->SetDescription("Change the system mouse cursor pointer.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2483576d, 0x57c66324));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMouseCursorSystemProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Interface/Screen");
    return od;
}

CKERROR CreateMouseCursorSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mouse Cursor System");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

#define CKPGUID_MOUSEPOINTERS CKGUID(0x37a05bdd, 0x1ed83f40)
    proto->DeclareInParameter("Mouse Pointer", CKPGUID_MOUSEPOINTERS, "Normal Select");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MouseCursorSystem);
    proto->SetBehaviorCallbackFct(MouseCursorSystemCallback);

    *pproto = proto;
    return CK_OK;
}

int MouseCursorSystem(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!im)
        return CKBR_GENERICERROR;

    VXCURSOR_POINTER mousePointer = VXCURSOR_NORMALSELECT;
    beh->GetInputParameterValue(0, &mousePointer);

    im->SetSystemCursor(mousePointer);
    return CKBR_OK;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR MouseCursorSystemCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORNEWSCENE:
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        if (beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            if (beh->IsActive())
            {
            }
        }
    }
    break;
    }
    return CKBR_OK;
}
