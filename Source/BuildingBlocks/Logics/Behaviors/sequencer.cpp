/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Sequencer
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSequencerDecl();
CKERROR CreateSequencerProto(CKBehaviorPrototype **);
int Sequencer(const CKBehaviorContext &behcontext);
int Sequencer2(const CKBehaviorContext &behcontext);
CKERROR SequencerCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

/*******************************************************/
/*                    DECLARATION                      */
/*******************************************************/
CKObjectDeclaration *FillBehaviorSequencerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sequencer");
    od->SetDescription("When 'In' is activated for the nth time, exits by the nth output.");
    od->SetCategory("Logics/Streaming");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x42530844, 0x257b6053));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSequencerProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

/*******************************************************/
/*                    PROTOTYPE                        */
/*******************************************************/
CKERROR CreateSequencerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sequencer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("In");

    proto->DeclareOutput("Exit Reset");
    proto->DeclareOutput("Out 1");

    proto->DeclareOutParameter("Current", CKPGUID_INT);

    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEOUTPUTS);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Sequencer2);

    proto->SetBehaviorCallbackFct(SequencerCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                MAIN FUNCTION                        */
/*******************************************************/
int Sequencer2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // We enter by In
    {
        beh->ActivateInput(1, FALSE);

        int current;
        beh->GetOutputParameterValue(0, &current);

        ++current;
        if (current >= beh->GetOutputCount() - 1)
            current = 0;
        beh->SetOutputParameterValue(0, &current);

        beh->ActivateOutput(current + 1);
    }

    if (beh->IsInputActive(0)) // We enter by Reset
    {
        beh->ActivateInput(0, FALSE);

        int current = -1;
        beh->SetOutputParameterValue(0, &current);

        beh->ActivateOutput(0, TRUE);
    }

    return CKBR_OK;
}

//-----------------------------------------------
// old version (back compatibilty)
//-----------------------------------------------
int Sequencer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0)) // We enter by In
    {
        beh->ActivateInput(0, FALSE);

        int current;
        beh->GetLocalParameterValue(0, &current);

        beh->ActivateOutput(current);

        current++;
        if (current >= beh->GetOutputCount())
            current = 0;
        beh->SetLocalParameterValue(0, &current);
    }

    if (beh->IsInputActive(1)) // We enter by Reset
    {
        beh->ActivateInput(1, FALSE);

        int current = 1;
        beh->SetLocalParameterValue(0, &current);

        beh->ActivateOutput(0, TRUE);
    }

    return CKBR_OK;
}
//---

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SequencerCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORRESET:
    {
        if (beh->GetVersion() > 0x00010000)
        {
            int current = -1;
            beh->SetOutputParameterValue(0, &current);
        }
        else
        {
            int current = 0;
            beh->SetLocalParameterValue(0, &current);
        }
    }
    break;

    case CKM_BEHAVIORLOAD:
    {
        if (beh->GetVersion() > 0x00010000)
        {
            beh->SetFunction(Sequencer2);
        }
        else
        {
            beh->SetFunction(Sequencer);
        }
    }
    break;
    }

    return CKBR_OK;
}
