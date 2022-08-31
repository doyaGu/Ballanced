//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT ConvertTimeToString
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTConvertTimeToStringBehaviorProto(CKBehaviorPrototype **pproto);
int TTConvertTimeToString(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTConvertTimeToStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ConvertTimeToString");
    od->SetDescription("konvertiert Time in Stringform 00:00:00");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x69e34443,0x459f23db));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTConvertTimeToStringBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTConvertTimeToStringBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ConvertTimeToString");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->DeclareOutParameter("String Time", CKPGUID_STRING, "00:00:00");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTConvertTimeToString);

    *pproto = proto;
    return CK_OK;
}

int TTConvertTimeToString(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTConvertTimeToStringCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}