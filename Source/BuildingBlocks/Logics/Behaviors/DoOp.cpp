/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DoOpera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "DoOp_BackCompatibility.h"

CKObjectDeclaration *FillBehaviorDoOperaDecl();
CKERROR CreateDoOperaProto(CKBehaviorPrototype **);
int DoOpera(const CKBehaviorContext &behcontext);
CKERROR DoOperaCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorDoOperaDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Op");
    od->SetDescription("Process any existing Parameter Operation (go to settings).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>p1: </SPAN>first argument of the operation.<BR>
    <SPAN CLASS=pin>p2: </SPAN>second argument of the operation.<BR>
    <BR>
    <SPAN CLASS=pout>res: </SPAN>result of the operation.<BR>
    <BR>
    Use this building block whenever you want to force a Parameter Operation to be process.<BR>
    <FONT SIZE=2>ex : </FONT>Suppose you need to get an object's position, and want to store this position in a local parameter.<BR>
    <FONT SIZE=2>While you can do this like that,</FONT><BR>
    <IMG SRC="Op1.gif"><BR><P>
    <FONT SIZE=2>It is better this way !</FONT><BR>
    <IMG SRC="Op2.gif"><BR><P>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2d5d6d01, 0x6a353eb0));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateDoOperaProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

/*************************************************/
/*                PROTO                          */
/*************************************************/
CKERROR CreateDoOperaProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Op");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("p1", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("p2", CKPGUID_FLOAT, "0");
    proto->DeclareOutParameter("res", CKPGUID_FLOAT, "0");

    proto->DeclareSetting(NULL, CKPGUID_VOIDBUF);         // internal A_LocalOpStruct
    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // op_guid first  part
    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // op_guid second part

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DoOpera);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_CUSTOMSETTINGSEDITDIALOG | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetBehaviorCallbackFct(DoOperaCallBack);

    *pproto = proto;
    return CK_OK;
}

typedef struct
{
    CK_PARAMETEROPERATION opfct;
    CKBOOL swap;
} A_LocalOpStruct;

/*************************************************/
/*                FUNCTION                       */
/*************************************************/
int DoOpera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKParameterIn *p1 = beh->GetInputParameter(0);
    CKParameterIn *p2 = beh->GetInputParameter(1);
    CKParameterOut *res = beh->GetOutputParameter(0);

    A_LocalOpStruct *opstruct = (A_LocalOpStruct *)beh->GetLocalParameterReadDataPtr(0);

    if (opstruct->opfct)
    {
        //____________________________ Operation Function
        if (opstruct->swap)
            opstruct->opfct(ctx, res, p2, p1);
        else
            opstruct->opfct(ctx, res, p1, p2);
        if (res)
            res->DataChanged();
    }

    //____________________________ IOs Activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR DoOperaCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL k = FALSE;
    A_LocalOpStruct *opstructtmp;
    opstructtmp = (A_LocalOpStruct *)beh->GetLocalParameterReadDataPtr(0);

    // Mac
    ENDIANSWAP32(opstructtmp->swap);

    if (opstructtmp)
        k = opstructtmp->swap;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    {
        CKGUID i1g, i2g, og, opg;
        CKParameterIn *pin;
        CKParameterOut *pout;
        if (pin = beh->GetInputParameter(0))
            i1g = pin->GetGUID();
        if (pin = beh->GetInputParameter(1))
            i2g = pin->GetGUID();
        if (pout = beh->GetOutputParameter(0))
            og = pout->GetGUID();

        CKParameterManager *pm = behcontext.ParameterManager;

        if (beh->GetLocalParameterCount() == 2) // old version
        {
            // create new int for guid
            CKParameterLocal *p;
            p = beh->CreateLocalParameter(NULL, CKPGUID_INT);

            //--- conversion
            int op_type = 8;
            beh->GetLocalParameterValue(1, &op_type);
            opg = ((CKGUID *)DoOp_BackCompatibility)[op_type];

            //--- write it correctly as a guid
            int *ptr = (int *)&opg;
            beh->SetLocalParameterValue(1, &ptr[0]);
            beh->SetLocalParameterValue(2, &ptr[1]);
        }
        else // new version
        {
            int op_guid1 = 0;
            beh->GetLocalParameterValue(1, &op_guid1);
            int op_guid2 = 0;
            beh->GetLocalParameterValue(2, &op_guid2);
            opg = CKGUID(op_guid1, op_guid2);
        }

        A_LocalOpStruct opstruct;
        opstruct.opfct = pm->GetOperationFunction(opg, og, i1g, i2g);
        opstruct.swap = FALSE;
        if (!opstruct.opfct || k)
        {
            opstruct.opfct = pm->GetOperationFunction(opg, og, i2g, i1g);
            opstruct.swap = TRUE;
        }

        beh->SetLocalParameterValue(0, &opstruct, sizeof(A_LocalOpStruct));
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKContext *context = beh->GetCKContext();

        // construct CKParameterOperation to edit it
        CKParameterIn *pin;
        CKParameterOut *pout;
        CKGUID i1g, i2g, resg, opg;
        pin = beh->GetInputParameter(0);
        i1g = pin->GetGUID();
        pin = beh->GetInputParameter(1);
        i2g = pin->GetGUID();
        pout = beh->GetOutputParameter(0);
        resg = pout->GetGUID();
        int op_guid1 = 0;
        beh->GetLocalParameterValue(1, &op_guid1);
        int op_guid2 = 0;
        beh->GetLocalParameterValue(2, &op_guid2);
        opg = CKGUID(op_guid1, op_guid2);
        // CKParameterOperation * ckop=context->CreateCKParameterOperation("",opg,resg,i1g,i2g);

//#pragma todo("Do not create a parameter operation, use the dialog box information")
        CKParameterOperation *ckop = (CKParameterOperation *)context->CreateObject(CKCID_PARAMETEROPERATION, NULL, CK_OBJECTCREATION_DYNAMIC);
        ckop->Reconstruct("", opg, resg, i1g, i2g);
        if (context->SendInterfaceMessage(CKUIM_EDITOBJECT, ckop->GetID(), 0) == CK_OK)
        {
            CKParameterManager *pMgr = context->GetParameterManager();

            opg = ckop->GetOperationGuid();
            if (ckop->GetInParameter1())
                i1g = ckop->GetInParameter1()->GetGUID();
            else
                i1g = CKPGUID_NONE;
            if (ckop->GetInParameter2())
                i2g = ckop->GetInParameter2()->GetGUID();
            else
                i2g = CKPGUID_NONE;
            if (ckop->GetOutParameter())
                resg = ckop->GetOutParameter()->GetGUID();
            else
                resg = CKPGUID_NONE;

            i1g, i2g, resg, opg;
            A_LocalOpStruct *opstructtmp = (A_LocalOpStruct *)beh->GetLocalParameterReadDataPtr(0);
            opstructtmp->opfct = pMgr->GetOperationFunction(opg, resg, i1g, i2g);
            if (!opstructtmp->opfct)
            {
                opstructtmp->opfct = pMgr->GetOperationFunction(opg, resg, i2g, i1g);
                opstructtmp->swap = TRUE;
            }
            else
                opstructtmp->swap = FALSE;
            beh->SetLocalParameterValue(0, opstructtmp);

            //___________________ Update Local Parameter
            int *ptr = (int *)&opg;
            beh->SetLocalParameterValue(1, &ptr[0]);
            beh->SetLocalParameterValue(2, &ptr[1]);

            //___________________ Set Inputs Type
            CKParameterIn *pin;
            if (pin = beh->GetInputParameter(0))
            {
                pin->SetGUID(i1g, TRUE);
            }
            if (pin = beh->GetInputParameter(1))
            {
                pin->SetGUID(i2g, TRUE);
            }

            //___________________ Set Outputs Type
            CKParameterOut *pout;
            if (pout = beh->GetOutputParameter(0))
            {
                pout->SetGUID(resg);
            }
        }
        context->DestroyObject(ckop->GetID());

        /*
            CKInterfaceManager* UIMan=(CKInterfaceManager*)behcontext.Context->GetManagerByGuid(INTERFACE_MANAGER_GUID);
          if (UIMan)  UIMan->CallBehaviorSettingsEditionFunction(beh,0);
        */

        break;
    }
    }

    return CKBR_OK;
}
