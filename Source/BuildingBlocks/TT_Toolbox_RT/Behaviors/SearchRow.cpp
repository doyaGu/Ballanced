////////////////////////////////
////////////////////////////////
//
//        TT_SearchRow
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSearchRowDecl();
CKERROR CreateSearchRowProto(CKBehaviorPrototype **pproto);
int SearchRow(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSearchRowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SearchRow");
    od->SetDescription("Finds value in array and returns row");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x279603a9, 0x16f20716));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSearchRowProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateSearchRowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SearchRow");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Search Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Search Value", CKPGUID_FLOAT, "0");

    proto->DeclareOutParameter("First Row Found", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SearchRow);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SearchRow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}