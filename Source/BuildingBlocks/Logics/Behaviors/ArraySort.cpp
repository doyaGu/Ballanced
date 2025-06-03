/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Sort Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorArraySortDecl();
CKERROR CreateArraySortProto(CKBehaviorPrototype **);
int ArraySort(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySortDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sort Rows");
    od->SetDescription("Sorts the rows of an array, according to the values of a given column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Sorting Column: </SPAN>the column where values are to be concidered to actually sort the rows.<BR>
    <SPAN CLASS=pin>Ascending: </SPAN>check it, if you want your elements in increasing order, otherwise uncheck it.<BR>
    <BR>
    */
    /* warning:
    - You can put any information and parameter types you want in all the column of the array, but beware to use only FLOAT, INTEGER and STRING for the "Sorting Column".
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6f623e68, 0x62bb5a98));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySortProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySortProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sort Rows");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Sorting Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Ascending", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArraySort);

    *pproto = proto;
    return CK_OK;
}

int ArraySort(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(0, &column);

    // we get the order
    CKBOOL order = TRUE;
    beh->GetInputParameterValue(1, &order);

    // the search itself
    array->Sort(column, order);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
