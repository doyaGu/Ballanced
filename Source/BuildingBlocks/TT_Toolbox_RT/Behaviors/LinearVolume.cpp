///////////////////////////////////
///////////////////////////////////
//
//        TT_LinearVolume
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorLinearVolumeDecl();
CKERROR CreateLinearVolumeProto(CKBehaviorPrototype **pproto);
int LinearVolume(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLinearVolumeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LinearVolume");
    od->SetDescription("Plays different samples according to a given vehicle velocity");
    od->SetCategory("TT Toolbox/Sounds");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9b335b3, 0x12d17cdc));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLinearVolumeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLinearVolumeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LinearVolume");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Vol. in Decibel", CKPGUID_FLOAT, "0");

    proto->DeclareOutParameter("Linear Volume", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LinearVolume);

    *pproto = proto;
    return CK_OK;
}

double linearize(double val)
{
    if (val > 1.0)
        return 1.0;
    if (val > 0.01)
        return pow(50.0, val) * 0.02;
    return 0.0;
}

int LinearVolume(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    float vol = 0.0f;
    beh->GetInputParameterValue(0, &vol);
    vol = (float)linearize(vol);
    beh->SetOutputParameterValue(0, &vol);

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}