/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array GetLine
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayGetLineDecl();
CKERROR CreateArrayGetLineProto(CKBehaviorPrototype **);
int ArrayGetLine(const CKBehaviorContext &behcontext);
CKERROR ArrayGetLineCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayGetLineDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Row");
    od->SetDescription("Gets the elements of a chosen row.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated if the given row index exists.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated if the given row index doesn't exist.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>the index of the row from which the elements must be returned.<BR>
    <BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the row found are written in
    these subsequent output parameters.<BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode.
    (The output parameters are only created in author mode, when editing the building block.)
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x33b77f41, 0x7b95c45));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayGetLineProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayGetLineProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArrayGetLine);
    proto->SetBehaviorCallbackFct(ArrayGetLineCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayGetLine(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int line = 0;
    beh->GetInputParameterValue(0, &line);

    // the search itself
    CKDataRow *it = array->GetRow(line);

    if (!it)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // we write the array data in the output param
    WriteOutputParameters(array, it, beh, 0);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayGetLineCallBack(const CKBehaviorContext &behcontext)
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
        CreateOutputParameters(array, beh, 0);
    }
    break;
    }

    return CKBR_OK;
}
