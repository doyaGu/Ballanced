//////////////////////////////////////
//////////////////////////////////////
//
//        TT_ParameterMapper
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorParameterMapperDecl();
CKERROR CreateParameterMapperProto(CKBehaviorPrototype **pproto);
int ParameterMapper(const CKBehaviorContext &behcontext);
CKERROR ParameterMapperCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorParameterMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ParameterMapper");
    od->SetDescription("According to the Input activated, routes the corresponding Input Parameter to the Output Parameter.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x33e30a1, 0x42f452fc));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateParameterMapperProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateParameterMapperProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ParameterMapper");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("NONE");
    proto->DeclareOutput("Out 0");

    proto->DeclareInParameter("pIn 0", CKPGUID_INT);
    proto->DeclareInParameter("pIn 1", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ParameterMapper);

    proto->SetBehaviorFlags(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS);
    proto->SetBehaviorCallbackFct(ParameterMapperCallBack);

    *pproto = proto;
    return CK_OK;
}

int ParameterMapper(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int inputCount = beh->GetInputCount();
    int outputCount = beh->GetOutputCount();

    for (int i = 0; i < inputCount; i++)
    {
        if (beh->IsInputActive(i))
        {
            beh->ActivateInput(i, FALSE);

            int paramValue = 0;
            beh->GetInputParameterValue(i, &paramValue);

            // Route to corresponding output: paramValue + 1 (since 0 is NONE)
            if (paramValue < 0 || paramValue >= outputCount - 1)
            {
                // Invalid or out of range -> NONE output
                beh->ActivateOutput(0, TRUE);
            }
            else
            {
                beh->ActivateOutput(paramValue + 1, TRUE);
            }
        }
    }

    return CKBR_OK;
}

CKERROR ParameterMapperCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CallbackMessage == CKM_BEHAVIOREDITED)
    {
        int inputCount = beh->GetInputCount();
        int paramCount = beh->GetInputParameterCount();

        // Sync input parameter count with input count
        while (inputCount > paramCount)
        {
            char buffer[32];
            sprintf(buffer, "pIn %d", paramCount);
            beh->CreateInputParameter(buffer, CKPGUID_INT);
            paramCount++;
        }

        while (paramCount > inputCount)
        {
            CKParameterIn *param = beh->RemoveInputParameter(--paramCount);
            CKDestroyObject(param);
        }
    }

    return CKBR_OK;
}