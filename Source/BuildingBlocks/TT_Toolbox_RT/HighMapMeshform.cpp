//////////////////////////////////////
//////////////////////////////////////
//
//        TT_HighMapMeshform
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTHighMapMeshformBehaviorProto(CKBehaviorPrototype **pproto);
int TTHighMapMeshform(const CKBehaviorContext& behcontext);
CKERROR TTHighMapMeshformCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTHighMapMeshformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_HighMapMeshform");
    od->SetDescription("verformt das Mesh aufgrund der Texturdaten");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e3c2d7e,0x7be7234e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTHighMapMeshformBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTHighMapMeshformBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_HighMapMeshform");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Height", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTHighMapMeshform);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TTHighMapMeshformCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTHighMapMeshform(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTHighMapMeshformCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}