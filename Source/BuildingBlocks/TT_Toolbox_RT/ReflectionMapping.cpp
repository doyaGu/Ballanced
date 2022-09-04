////////////////////////////////////////
////////////////////////////////////////
//
//        TT_ReflectionMapping
//
////////////////////////////////////////
////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorReflectionMappingDecl();
CKERROR CreateReflectionMappingProto(CKBehaviorPrototype **pproto);
int ReflectionMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReflectionMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReflectionMapping");
    od->SetDescription("erzeugt ReflectionMapping �ber �nderung der MappingChannelNormalen");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e212b2f, 0x24db67c6));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReflectionMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateReflectionMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ReflectionMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Camera", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReflectionMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int ReflectionMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}