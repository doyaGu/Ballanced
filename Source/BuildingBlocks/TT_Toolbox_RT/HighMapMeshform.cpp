//////////////////////////////////////
//////////////////////////////////////
//
//        TT_HighMapMeshform
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorHighMapMeshformDecl();
CKERROR CreateHighMapMeshformProto(CKBehaviorPrototype **pproto);
int HighMapMeshform(const CKBehaviorContext &behcontext);
CKERROR HighMapMeshformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHighMapMeshformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_HighMapMeshform");
    od->SetDescription("verformt das Mesh aufgrund der Texturdaten");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e3c2d7e, 0x7be7234e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHighMapMeshformProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateHighMapMeshformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_HighMapMeshform");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Height", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(HighMapMeshform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(HighMapMeshformCallBack);

    *pproto = proto;
    return CK_OK;
}

int HighMapMeshform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR HighMapMeshformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}