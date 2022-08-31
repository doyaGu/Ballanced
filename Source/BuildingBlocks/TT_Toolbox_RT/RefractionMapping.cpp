////////////////////////////////////////
////////////////////////////////////////
//
//        TT_RefractionMapping
//
////////////////////////////////////////
////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTRefractionMappingBehaviorProto(CKBehaviorPrototype **pproto);
int TTRefractionMapping(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTRefractionMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RefractionMapping");
    od->SetDescription("erzeugt RefractionMapping �ber �nderung der MappingChannelNormalen");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53024810,0x22ed280f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Mirco");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTRefractionMappingBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTRefractionMappingBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RefractionMapping");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Camera", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Depth", CKPGUID_FLOAT, "10.0");
    proto->DeclareInParameter("Refractionfactor", CKPGUID_FLOAT, "0.95");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTRefractionMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTRefractionMapping(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTRefractionMappingCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}