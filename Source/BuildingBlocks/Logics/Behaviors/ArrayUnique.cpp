/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Unique Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKDataArray.h"

CKObjectDeclaration *FillBehaviorArrayUniqueDecl();
CKERROR CreateArrayUniqueProto(CKBehaviorPrototype **);
int ArrayUnique(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayUniqueDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Make Rows Unique");
    od->SetDescription("Assures the uniqueness of the elements of a column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>index of the column where duplicated values are to be searched.<BR>
    <BR>
    This building block will delete all the rows containing a value in the given column that has already be parsed.
    But you should sort your array before calling this bulding block, because rows are delete only if they follow each other.<BR>
    Thus, if you want to have only one row of a value in your array, you need to first sort the array on this
    column, then apply a unique on the same column.<BR>
    */
    /* warning:
    - Do not forget do sort your array before calling this building block (see "Sort Rows" building block).<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2b175818, 0x2c270b20));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayUniqueProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayUniqueProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Make Rows Unique");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayUnique);

    *pproto = proto;
    return CK_OK;
}

int ArrayUnique(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(0, &column);

    // the search itself
    array->Unique(column);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
