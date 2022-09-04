//////////////////////////////////
//////////////////////////////////
//
//        TT InputString
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorInputStringDecl();
CKERROR CreateInputStringProto(CKBehaviorPrototype **pproto);
int InputString(const CKBehaviorContext &behcontext);
CKERROR InputStringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInputStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT InputString");
    od->SetDescription("Captures text entered by user.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x52fd6294, 0x612f51a5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInputStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateInputStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT InputString");
    if (!proto) return CKERR_OUTOFMEMORY;

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
    proto->DeclareSetting("Disable Keyboard Section", CKPGUID_KEYBOARDPART, "0");
    proto->DeclareSetting("Use Caret", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(InputString);

    proto->SetBehaviorCallbackFct(InputStringCallBack);

    *pproto = proto;
    return CK_OK;
}

int InputString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR InputStringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}