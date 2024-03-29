/////////////////////////////////////
/////////////////////////////////////
//
//        TT Find SubString
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFindSubStringDecl();
CKERROR CreateFindSubStringProto(CKBehaviorPrototype **pproto);
int FindSubString(const CKBehaviorContext &behcontext);
CKERROR FindSubStringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFindSubStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Find SubString");
    od->SetDescription("Searches for strings in object names");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12f31e27, 0x4cea0bf5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFindSubStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateFindSubStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Find SubString");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In0");

    proto->DeclareOutput("None");
    proto->DeclareOutput("StringOut 1");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("String 1", CKPGUID_STRING);

    proto->DeclareOutParameter("String ID", CKPGUID_INT);

    proto->DeclareSetting("String Count", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FindSubString);

    proto->SetBehaviorCallbackFct(FindSubStringCallBack);

    *pproto = proto;
    return CK_OK;
}

int FindSubString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR FindSubStringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}