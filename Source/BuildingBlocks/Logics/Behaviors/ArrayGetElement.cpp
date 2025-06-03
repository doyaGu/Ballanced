/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array GetElement
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayGetElementDecl();
CKERROR CreateArrayGetElementProto(CKBehaviorPrototype **);
int ArrayGetElement(const CKBehaviorContext &behcontext);
CKERROR ArrayGetElementCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayGetElementDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Cell");
    od->SetDescription("Gets the cell of a chosen row and column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated if the given row index exists.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated if the given row index doesn't exist.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>the index of the row from which the cell must be returned.<BR>
    <SPAN CLASS=pin>Column Index: </SPAN>the column of the row from which the cell must be returned.<BR>
    <BR>
    <SPAN CLASS=pout>Cell Value: </SPAN>the cell of the row found. You need to change the type of this
    parameter according to the type of the colmun you will have at runtime.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x33b99f51, 0x7d95c45));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayGetElementProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayGetElementProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Cell");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");
    proto->DeclareInParameter("Column Index", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Cell Value", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS));

    proto->SetFunction(ArrayGetElement);
    proto->SetBehaviorCallbackFct(ArrayGetElementCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayGetElement(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the row
    int line = 0;
    beh->GetInputParameterValue(0, &line);

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(1, &column);

    // the search itself
    CKDWORD *e = array->GetElement(line, column);
    if (!e)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    CKParameterOut *pout;
    pout = beh->GetOutputParameter(0);

    switch (array->GetColumnType(column))
    {
    case CKARRAYTYPE_INT:
    {
        if (pout->GetGUID() == CKPGUID_INT)
        {
            pout->SetValue(e);
        }
    }
    break;
    case CKARRAYTYPE_FLOAT:
    {
        if (pout->GetGUID() == CKPGUID_FLOAT)
        {
            pout->SetValue(e);
        }
    }
    break;
    case CKARRAYTYPE_STRING:
    {
        if (pout->GetGUID() == CKPGUID_STRING)
        {
            char *ns = (char *)*e;
            if (ns)
                pout->SetValue(ns, strlen(ns) + 1);
            else
            {
                char *s = "";
                pout->SetValue(s, 1);
            }
        }
    }
    break;
    case CKARRAYTYPE_OBJECT:
    {
        CK_CLASSID cid;
        if (cid = pout->GetParameterClassID())
        {
            CKObject *obj = behcontext.Context->GetObject(*e);
            if (CKIsChildClassOf(obj, cid))
                pout->SetValue(e);
            else
            {
                int dummy = 0;
                pout->SetValue(&dummy);
            }
        }
    }
    break;
    case CKARRAYTYPE_PARAMETER:
    {
        CKParameterOut *arraypout = (CKParameterOut *)*e;
        pout->CopyValue(arraypout);
    }
    break;
    }

    // we write the array data in the output param

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayGetElementCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        for (int i = 1; i < beh->GetOutputParameterCount(); ++i)
        {
            CKDestroyObject(beh->RemoveOutputParameter(1));
        }
    }
    break;
    }

    return CKBR_OK;
}
