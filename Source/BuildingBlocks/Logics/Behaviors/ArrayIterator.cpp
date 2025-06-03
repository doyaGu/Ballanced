/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Iterator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayIteratorDecl();
CKERROR CreateArrayIteratorProto(CKBehaviorPrototype **);
int ArrayIterator(const CKBehaviorContext &behcontext);
CKERROR ArrayIteratorCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Iterator");
    od->SetDescription("Iterate on each row of an array and return the values of the current row.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Starting Row: </SPAN>index of the row where iteration should start.<BR>
    <SPAN CLASS=pin>Ending Row: </SPAN>index of the row where iteration should stop (inclusive). -1 means the last row.<BR>
    <SPAN CLASS=pin>Backward: </SPAN>specfifies the direction of the iteration:FALSE=normal,TRUE=backward.<BR>
    <BR>
    <SPAN CLASS=pout>Row Index: </SPAN>index of the curent parsed row.<BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the current row are written in these subsequent output parameters.<BR>
    <BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode (The output parameters are only created in author mode, when editing the building block).<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x198f0af9, 0x268249f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayIteratorProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayIteratorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Iterator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Starting Row", CKPGUID_INT, "0");
    proto->DeclareInParameter("Ending Row", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Backward", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("Row Index", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArrayIterator);
    proto->SetBehaviorCallbackFct(ArrayIteratorCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayIterator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int linecount = array->GetRowCount();

    ///
    // Reading Input parameters

    int startingrow = 0;
    beh->GetInputParameterValue(0, &startingrow);
    if (startingrow >= linecount)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    int endingrow = -1;
    beh->GetInputParameterValue(1, &endingrow);
    if (endingrow == -1)
        endingrow = linecount - 1;

    CKBOOL backward = FALSE;
    beh->GetInputParameterValue(2, &backward);

    int index;
    CKParameterOut *pout;
    CKBOOL first = FALSE;
    if (beh->IsInputActive(0))
    { // we've entered by reset
        first = TRUE;

        beh->ActivateInput(0, FALSE);
        if (backward)
            index = endingrow;
        else
            index = startingrow;
        beh->SetOutputParameterValue(0, &index);
    }
    else
    { // we've entered by next
        beh->ActivateInput(1, FALSE);
        pout = beh->GetOutputParameter(0);

        int *pindex = (int *)pout->GetWriteDataPtr();
        if (backward)
            index = --(*pindex);
        else
            index = ++(*pindex);

        // warping test
        if (index <= -1)
        {
            index = linecount - 1;
            (*pindex) = index;
        }
        else
        {
            if (index >= linecount)
            {
                index = 0;
                (*pindex) = 0;
            }
        }

        pout->DataChanged();
    }

    if (backward)
    {
        endingrow = startingrow - 1;
        if (endingrow == -1)
            endingrow = linecount - 1;
    }
    else
    {
        ++endingrow;
        if (endingrow >= linecount)
            endingrow = 0;
    }

    if (!first || !linecount)
    {
        if (index == endingrow)
        { // we've finished
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }

    // we get the index'th line
    CKDataRow *it = array->GetRow(index);

    // we write the array data in the output param
    WriteOutputParameters(array, it, beh, 1);

    // We loop
    beh->ActivateOutput(1);

    return CKBR_OK;
}

CKERROR ArrayIteratorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIOREDITED:
    {
        CKDataArray *array = (CKDataArray *)beh->GetTarget();
        if (!array)
            return CKBR_OK;

        // we create the output parameters
        CreateOutputParameters(array, beh, 1);
    }
    break;
    }

    return CKBR_OK;
}
