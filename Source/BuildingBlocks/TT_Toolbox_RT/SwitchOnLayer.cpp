//////////////////////////////////////
//////////////////////////////////////
//
//        TT Switch on Layer
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSwitchonLayerDecl();
CKERROR CreateSwitchonLayerProto(CKBehaviorPrototype **pproto);
int SwitchonLayer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSwitchonLayerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Switch on Layer");
    od->SetDescription("Gets the value of a square");
    od->SetCategory("TT Toolbox/Grids");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1a151160, 0x1be26257));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchonLayerProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSwitchonLayerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Switch on Layer");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("Enter");
    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Layer", CKPGUID_LAYERTYPE, "- default -");
    proto->DeclareInParameter("Value", CKPGUID_INT, "255");

    proto->DeclareOutParameter("Exitvector", CKPGUID_VECTOR);

    proto->DeclareLocalParameter("Last Position", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("Was inside", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchonLayer);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SwitchonLayer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}