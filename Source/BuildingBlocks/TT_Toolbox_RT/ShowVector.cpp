/////////////////////////////////
/////////////////////////////////
//
//        TT_ShowVector
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorShowVectorDecl();
CKERROR CreateShowVectorProto(CKBehaviorPrototype **pproto);
int ShowVector(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorShowVectorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ShowVector");
    od->SetDescription("zeigt Vektor �ber 2 gegebene Punkte an");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2fca6b65, 0x73b31ebd));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShowVectorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateShowVectorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ShowVector");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("StartPos", CKPGUID_VECTOR);
    proto->DeclareInParameter("EndPos", CKPGUID_VECTOR);
    proto->DeclareInParameter("Curve", CKPGUID_CURVE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShowVector);

    *pproto = proto;
    return CK_OK;
}

int ShowVector(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}