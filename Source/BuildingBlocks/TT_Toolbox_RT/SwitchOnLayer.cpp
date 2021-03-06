/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT SwitchOnLayer
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorSwitchOnLayerDecl();
CKERROR CreateSwitchOnLayerProto(CKBehaviorPrototype **);
int SwitchOnLayer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSwitchOnLayerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Switch on Layer");
    od->SetDescription("Gets the value of a square");
    od->SetCategory("TT Toolbox/Grids");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1A151160, 0x1BE26257));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchOnLayerProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSwitchOnLayerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Switch on Layer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("Enter");
    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Layer", CKPGUID_LAYERTYPE, "- default -");
    proto->DeclareInParameter("Value", CKPGUID_INT, "255");
    proto->DeclareInParameter("Exitvector", CKPGUID_VECTOR);

    proto->DeclareLocalParameter("Last Position", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("Was inside", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchOnLayer);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SwitchOnLayer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}