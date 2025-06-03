/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Add Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayInsertColumnDecl();
CKERROR CreateArrayInsertColumnProto(CKBehaviorPrototype **);
int ArrayInsertColumn(const CKBehaviorContext &behcontext);
CKERROR ArrayInsertColumnCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayInsertColumnDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Insert Column");
    od->SetDescription("Inserts a column in the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Index: </SPAN>index of the new column (next columns will be automatically shift).<BR>
    <SPAN CLASS=pin>Column Name: </SPAN>name to be given to the new column.<BR>
    <SPAN CLASS=pin>Column Type: </SPAN>type of the new column (Integer, Float, String, Object, Parameter).<BR>
    <SPAN CLASS=pin>Parameter Type: </SPAN>parameter which is used only to know which type of parameter has to be used if "Column Type"=Parameter.<BR>
    eg: if you need a column containing Vectors, then you should give "Parameter Type" input parmeter, the type VECTOR.<BR>
    <BR>
    */
    /* warning:
    - The "Parameter Type" input parameter is taken into consideration only if "Column Type"=Parameter. Otherwise, it's useless.<BR>
      - The "Insert Column" isn't supposed to be used in combination with BB such as Iterator BB.
      Because the Iterator BB (as other BB) dynamically change its outputs/inputs when they are edited (right-click + Edit Building Block),
      to match the column numbers and types.<BR>
      If you add a column to your array after having the "Iterator" BB edited, the "Iterator" BB won't be aware of this change, and its outputs/inputs won't be update.<BR>
      Thus the "Insert Column" BB should be used instead with a "GetElement" and "Set Element" BB.<BR>
      */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5cb7802, 0x4a64a58));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayInsertColumnProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayInsertColumnProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Insert Column");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Index", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Column Name", CKPGUID_STRING, "New Column");
    proto->DeclareInParameter("Column Type", CKPGUID_ARRAYTYPE, "Integer");
    proto->DeclareInParameter("Parameter Type", CKPGUID_PARAMETERTYPE, "Vector");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(ArrayInsertColumn);

    *pproto = proto;
    return CK_OK;
}

int ArrayInsertColumn(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int c = -1;
    beh->GetInputParameterValue(0, &c);

    CKSTRING columnname = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CK_ARRAYTYPE type = CKARRAYTYPE_INT;
    beh->GetInputParameterValue(2, &type);

    CKGUID guid;
    if (type == CKARRAYTYPE_PARAMETER)
    {
        CKParameterIn *pin = beh->GetInputParameter(3);
        if (pin)
            guid = pin->GetGUID();

        if (guid == CKPGUID_PARAMETERTYPE)
        {
            int pt = 0;
            pin->GetValue(&pt);

            guid = behcontext.ParameterManager->ParameterTypeToGuid(pt);
        }
    }

    // the Column Insertion
    array->InsertColumn(c, type, columnname, guid);

    return CKBR_OK;
}
