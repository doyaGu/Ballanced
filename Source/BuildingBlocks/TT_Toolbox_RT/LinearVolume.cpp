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

int LinearVolume(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}