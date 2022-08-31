////////////////////////////////
////////////////////////////////
//
//        TT_SearchRow
//
////////////////////////////////
////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSearchRowBehaviorProto(CKBehaviorPrototype **pproto);
int TTSearchRow(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSearchRowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SearchRow");
    od->SetDescription("sucht Wert in Array und gibt Zeile aus");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x279603a9,0x16f20716));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSearchRowBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSearchRowBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SearchRow");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Search Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Search Value", CKPGUID_FLOAT, "0");

    proto->DeclareOutParameter("First Row Found", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSearchRow);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTSearchRow(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSearchRowCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}