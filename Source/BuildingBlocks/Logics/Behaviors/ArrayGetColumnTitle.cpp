/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array GetColumnTitle
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorArrayGetColumnTitleDecl();
CKERROR CreateArrayGetColumnTitleProto(CKBehaviorPrototype **);
int ArrayGetColumnTitle(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayGetColumnTitleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Column Name");
    od->SetDescription("Gets the name of a column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated if the given row index exists.<BR>
    <BR>
    <SPAN CLASS=pin>Column Index: </SPAN>the index of the column from which the name must be returned.<BR>
    <BR>
    <SPAN CLASS=pout>Column Name: </SPAN>the name of the column.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x33b7ff41, 0x7b95c44));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayGetColumnTitleProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayGetColumnTitleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Column Name");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column Index", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Column Name", CKPGUID_STRING, "NULL");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(ArrayGetColumnTitle);

    *pproto = proto;
    return CK_OK;
}

int ArrayGetColumnTitle(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int col = 0;
    beh->GetInputParameterValue(0, &col);

    int nbCol = array->GetColumnCount();
    if ((col < 0) || (col >= nbCol))
        return CKBR_PARAMETERERROR;

    XBaseString ctitle(array->GetColumnName(col));
    beh->SetOutputParameterValue(0, ctitle.CStr(), ctitle.Length() + 1);

    return CKBR_OK;
}