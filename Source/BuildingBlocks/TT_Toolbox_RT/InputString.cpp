//////////////////////////////////
//////////////////////////////////
//
//        TT InputString
//
//////////////////////////////////
//////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTInputStringBehaviorProto(CKBehaviorPrototype **pproto);
int TTInputString(const CKBehaviorContext& behcontext);
CKERROR TTInputStringCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTInputStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT InputString");
    od->SetDescription("Captures text entered by user.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x52fd6294,0x612f51a5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTInputStringBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTInputStringBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT InputString");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("On");
    proto->DeclareOutput("Off");
    proto->DeclareOutput("Updated String");

    proto->DeclareInParameter("Reset String", CKPGUID_STRING);
    proto->DeclareInParameter("End Key", CKPGUID_KEY);

    proto->DeclareOutParameter("String", CKPGUID_STRING);
    proto->DeclareOutParameter("StringWithoutCaret", CKPGUID_STRING);
    proto->DeclareOutParameter("Key", CKPGUID_KEY);

    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_STRING);

    proto->DeclareSetting("Max Size", CKPGUID_INT, "512");
    proto->DeclareSetting("Keyboard Repetition", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Multiline", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Disable Keyboard Section", , "0");
    proto->DeclareSetting("Use Caret", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTInputString);

    proto->SetBehaviorCallbackFct(TTInputStringCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTInputString(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTInputStringCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}