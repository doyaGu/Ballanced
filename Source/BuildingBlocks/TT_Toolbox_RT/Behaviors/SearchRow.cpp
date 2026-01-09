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
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column = 0;
    beh->GetInputParameterValue(0, &column);

    CKParameterIn *searchParam = beh->GetInputParameter(1);
    CKParameter *param = searchParam->GetRealSource();

    void *searchData = NULL;
    int dataSize = 0;

    CK_ARRAYTYPE colType = array->GetColumnType(column);
    CKGUID paramGuid = param->GetGUID();

    switch (colType)
    {
    case CKARRAYTYPE_INT:
        if (paramGuid == CKPGUID_INT)
        {
            param->GetValue(&searchData, TRUE);
        }
        break;

    case CKARRAYTYPE_FLOAT:
        if (paramGuid == CKPGUID_FLOAT)
        {
            param->GetValue(&searchData, TRUE);
        }
        break;

    case CKARRAYTYPE_STRING:
        searchData = param->GetReadDataPtr(TRUE);
        if (searchData)
            dataSize = strlen((char *)searchData) + 1;
        break;

    case CKARRAYTYPE_OBJECT:
        if (paramGuid == CKPGUID_BEOBJECT)
        {
            param->GetValue(&searchData, TRUE);
        }
        break;

    case CKARRAYTYPE_PARAMETER:
    {
        CKGUID colGuid = array->GetColumnParameterGuid(column);
        if (paramGuid == colGuid)
        {
            searchData = param->GetReadDataPtr(TRUE);
            dataSize = param->GetDataSize();
        }
    }
    break;

    default:
        ctx->OutputToConsole("Wrong value type", TRUE);
        return CKBR_OK;
    }

    int rowIndex = array->FindRowIndex(column, CKEQUAL, (CKDWORD)searchData, dataSize, 0);
    beh->SetOutputParameterValue(0, &rowIndex);

    return CKBR_OK;
}