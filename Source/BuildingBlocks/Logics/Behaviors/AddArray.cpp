/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Add Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorAddArrayDecl();
CKERROR CreateAddArrayProto(CKBehaviorPrototype **);
int AddArray(const CKBehaviorContext &behcontext);
CKERROR AddArrayCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Row");
    od->SetDescription("Adds a new row to the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated if the row is rightly added.<BR>
    <SPAN CLASS=out>Already Present: </SPAN>is activated if the specified value in the key column already exists.<BR>
    <BR>
    <SPAN CLASS=setting>Unique Key Column: </SPAN>if TRUE, then adding a value.<BR>
    <BR>
    This building block, when attached to an array or when given an array as a target,
    creates as many parameters as there are columns in this array, each parameter
    of the type of the corresponding column. When activated, the building block add a new
    row at the end of the array and set the values of this row with the specified input parameters.
    If the "Key Column Unicity" setting is set to TRUE, it creates
    the new row only if the value in the key column wasn't already present in the array.
    (If the value in the key column was already present, the the second output is activated)
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1c7e5dc6, 0x3f6423c2));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddArrayProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateAddArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Already Present");

    proto->DeclareSetting("Unique Key Column", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));

    proto->SetFunction(AddArray);
    proto->SetBehaviorCallbackFct(AddArrayCallBack);

    *pproto = proto;
    return CK_OK;
}

int AddArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we test, if asked if the line isn't already in the array
    CKBOOL unique = TRUE;
    beh->GetLocalParameterValue(0, &unique);
    if (unique)
    {
        int column = array->GetKeyColumn();
        if (column >= 0)
        {
            CKDWORD key;
            int size = GetInputValue(array, beh, column, column, key);
            CKDataRow *fit = array->FindRow(column, CKEQUAL, key, size);
            if (fit)
            {
                // Set IO states
                beh->ActivateInput(0, FALSE);
                beh->ActivateOutput(1);

                return CKBR_OK;
            }
        }
    }

    // the insertion itself
    CKDataRow *it = array->InsertRow();

    // we put the input parameters value in the array
    ReadInputParameters(array, it, beh, 0);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR AddArrayCallBack(const CKBehaviorContext &behcontext)
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
        CreateInputParameters(array, beh, 0);

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
