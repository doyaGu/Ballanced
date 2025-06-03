/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array SetLine
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArraySetLineDecl();
CKERROR CreateArraySetLineProto(CKBehaviorPrototype **);
int ArraySetLine(const CKBehaviorContext &behcontext);
CKERROR ArraySetLineCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySetLineDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Row");
    od->SetDescription("Set the elements value of a row of an array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when process is completed.<BR>
    <SPAN CLASS=out>Already Present: </SPAN>is activated if the given row already exist.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>The index of the row from which the elements must be changed.<BR>
    <SPAN CLASS=pin>(Element Values): </SPAN>All the elements of the row to set are read from
    these input parameters.<BR>
    If you don't link some input parameters, the values will be unchanged.<BR>
    <BR>
    <SPAN CLASS=setting>Unique Key Column: </SPAN>Specify if you want or not take care of the key column.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x62e87901, 0x2df007dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySetLineProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySetLineProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Already Present");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");

    proto->DeclareSetting("Unique Key Column", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArraySetLine);
    proto->SetBehaviorCallbackFct(ArraySetLineCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArraySetLine(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the line
    int line = 0;
    beh->GetInputParameterValue(0, &line);

    // we test, if asked if the line isn't already in the array
    CKBOOL unique = TRUE;
    beh->GetLocalParameterValue(0, &unique);
    if (unique)
    {
        int column = array->GetKeyColumn();
        if (column >= 0)
        {
            CKDWORD key;
            int size = GetInputValue(array, beh, column, column + 1, key);
            int ind = array->FindRowIndex(column, CKEQUAL, key, size);
            if (ind != -1 && ind != line)
            {
                // Set IO states
                beh->ActivateInput(0, FALSE);
                beh->ActivateOutput(1);

                return CKBR_OK;
            }
        }
    }

    // the insertion itself
    CKDataRow *it = array->GetRow(line);

    if (it)
    {
        // we put the input parameters value in the array
        ReadInputParameters(array, it, beh, 1);
    }

    // SetLine IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArraySetLineCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORSETTINGSEDITED:
    case CKM_BEHAVIOREDITED:
    {
        CKDataArray *array = (CKDataArray *)beh->GetTarget();
        if (!array)
            return CKBR_OK;

        // we create the input parameters
        CreateInputParameters(array, beh, 1);

        CKBOOL unique = TRUE;
        beh->GetLocalParameterValue(0, &unique);
        if (unique)
        {
            if (beh->GetOutputCount() == 1)
                beh->AddOutput("Already Present");
        }
        else
        {
            beh->DeleteOutput(1);
        }
    }
    break;
    }

    return CKBR_OK;
}
