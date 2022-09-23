/////////////////////////////////
/////////////////////////////////
//
//        TT_SetMapping
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetMappingDecl();
CKERROR CreateSetMappingProto(CKBehaviorPrototype **pproto);
int SetMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetMapping");
    od->SetDescription("Change the UV coordinates for the Material Channels");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e36319f, 0x6ec6031f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateSetMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("New planar Mapping", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Rotate", CKPGUID_ANGLE, "0.0");
    proto->DeclareInParameter("Offset UV", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Tile U", CKPGUID_2DVECTOR, "1,1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int SetMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}