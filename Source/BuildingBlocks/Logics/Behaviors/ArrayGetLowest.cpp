/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Get Lowest
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayGetLowestDecl();
CKERROR CreateArrayGetLowestProto(CKBehaviorPrototype **);
int ArrayGetLowest(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayGetLowestDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Lowest");
    od->SetDescription("Gets the element of an array, which value in the given column is the lowest.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when the element is found in the array.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated when the element is not found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>the column in which the lowest value must be searched.<BR>
    <BR>
    <SPAN CLASS=pout>Row Number: </SPAN>the index of the row in which the element has been found.<BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the row found are written in these subsequent output parameters.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x71b25ed7, 0x3c365a30));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayGetLowestProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayGetLowestProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Lowest");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Row Number", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArrayGetLowest);
    proto->SetBehaviorCallbackFct(ArrayLineOutputCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayGetLowest(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(0, &column);

    // the search itself
    int row = -1;
    array->GetLowest(column, row);

    CKDataRow *it = array->GetRow(row);

    // we write the row number
    beh->SetOutputParameterValue(0, &row);

    if (!it)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // we write the array data in the output parameters
    WriteOutputParameters(array, it, beh, 1);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
