/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetDeltaTime
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetDeltaTimeDecl();
CKERROR CreateGetDeltaTimeProto(CKBehaviorPrototype **);
int GetDeltaTime(const CKBehaviorContext &behcontext);
int GetDeltaTime_old(const CKBehaviorContext &behcontext);
CKERROR GetDeltaTimeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetDeltaTimeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Delta Time");
    od->SetDescription("Returns the elapsed time between the previous and the current Behavioral Process.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Delta Time: </SPAN>elapsed time between two behavioral process.<BR>
    <BR>
    */
    /* warning:
    - previous versions of this building block used a INTEGER type for the output value... now it's a TIME (i.e. a float).<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x47dc3232, 0x64bf203a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateGetDeltaTimeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetDeltaTimeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Delta Time");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Delta Time", CKPGUID_TIME);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetDeltaTime);
    proto->SetBehaviorCallbackFct(GetDeltaTimeCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR GetDeltaTimeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        if (beh->GetVersion() == 0x00020000)
        {
            beh->SetFunction(GetDeltaTime);
        }
        else
        {
            beh->SetFunction(GetDeltaTime_old);
        }
    }
    break;
    }

    return CKBR_OK;
}

int GetDeltaTime(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    float deltat = behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &deltat);

    return CKBR_OK;
}

int GetDeltaTime_old(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    int deltat = (int)behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &deltat);

    return CKBR_OK;
}
