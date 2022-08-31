////////////////////////////////////
////////////////////////////////////
//
//        TT_SetMipMapping
//
////////////////////////////////////
////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSetMipMappingBehaviorProto(CKBehaviorPrototype **pproto);
int TTSetMipMapping(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSetMipMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetMipMapping");
    od->SetDescription("stellt MipMapping f�r Material ein");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38c34dab,0x5dd03bdf));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSetMipMappingBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSetMipMappingBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetMipMapping");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Filter Min", CKPGUID_FILTERMODE, "ANISOTROPIC");
    proto->DeclareInParameter("Filter Mag", CKPGUID_FILTERMODE, "ANISOTROPIC");
    proto->DeclareInParameter("MipMap Count", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSetMipMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTSetMipMapping(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSetMipMappingCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}