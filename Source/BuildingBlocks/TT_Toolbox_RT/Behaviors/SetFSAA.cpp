//////////////////////////////
//////////////////////////////
//
//        TT_SetFSAA
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetFSAADecl();
CKERROR CreateSetFSAAProto(CKBehaviorPrototype **pproto);
int SetFSAA(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFSAADecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetFSAA");
    od->SetDescription("Toggles antialiasing on/off in the render state");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30bd6156, 0x14772834));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFSAAProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetFSAAProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetFSAA");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("FSAA", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFSAA);

    *pproto = proto;
    return CK_OK;
}

int SetFSAA(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    
    CKBOOL fsaa = FALSE;
    beh->GetInputParameterValue(0, &fsaa);
    behcontext.CurrentRenderContext->SetState(VXRENDERSTATE_ANTIALIAS, fsaa);

    return CKBR_OK;
}