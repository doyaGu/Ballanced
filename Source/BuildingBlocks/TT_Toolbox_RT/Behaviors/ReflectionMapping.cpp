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
    od->SetDescription("Creates reflection mapping when changing mappings channel normals");
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

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
    {
        return CKBR_OWNERERROR;
    }

    int matChannel = -1;
    beh->GetInputParameterValue(0, &matChannel);

    CKCamera *camera = (CKCamera *)beh->GetInputParameterObject(1);
    VxVector cameraPosition;
    camera->GetPosition(&cameraPosition, target);

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
    {
        return CKBR_OK;
    }

    CKDWORD uvStride = 0;
    CKDWORD vertexStride = 0;
    CKDWORD normalStride = 0;
    CKBYTE *uvs = mesh->GetModifierUVs(&uvStride);
    CKBYTE *vertices = mesh->GetModifierVertices(&vertexStride);
    void *normals = mesh->GetNormalsPtr(&normalStride);

    const int count = mesh->GetVertexCount();
    for (int i = 0; i < count; ++i)
    {
        VxVector *pos = (VxVector *)&vertices[i * vertexStride];
        VxVector *normal = (VxVector *)&((CKBYTE *)normals)[i * normalStride];
        Vx2DVector *uv = (Vx2DVector *)&uvs[i * uvStride];

        VxVector e = cameraPosition - *pos;
        e.Normalize();

        VxVector r = 2 * normal->Dot(e) * *normal - *pos;
        r.Normalize();
        uv->x = (r.x + 1.0f) * 0.5f;
        uv->y = (r.z + 1.0f) * 0.5f;
    }

    mesh->UVChanged();
    return CKBR_ACTIVATENEXTFRAME;
}