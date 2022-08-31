//////////////////////////////////
//////////////////////////////////
//
//        TT_SplitString
//
//////////////////////////////////
//////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSplitStringBehaviorProto(CKBehaviorPrototype **pproto);
int TTSplitString(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSplitStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SplitString");
    od->SetDescription("split a string into Sub-Elements.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ae74e2f,0x799c49b5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSplitStringBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSplitStringBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SplitString");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Delimiter", CKPGUID_STRING);

    proto->DeclareOutParameter("Elements Found", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSplitString);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS));
    *pproto = proto;
    return CK_OK;
}

int TTSplitString(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSplitStringCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}