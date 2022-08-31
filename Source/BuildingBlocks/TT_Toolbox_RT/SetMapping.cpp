/////////////////////////////////
/////////////////////////////////
//
//        TT_SetMapping
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSetMappingBehaviorProto(CKBehaviorPrototype **pproto);
int TTSetMapping(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSetMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetMapping");
    od->SetDescription("aendern der UV-Koordianten f�r die Materialchannels");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e36319f,0x6ec6031f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSetMappingBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSetMappingBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetMapping");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("New planar Mapping", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Rotate", CKPGUID_ANGLE, "0.0");
    proto->DeclareInParameter("Offset UV", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Tile U", CKPGUID_2DVECTOR, "1,1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSetMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTSetMapping(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSetMappingCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}