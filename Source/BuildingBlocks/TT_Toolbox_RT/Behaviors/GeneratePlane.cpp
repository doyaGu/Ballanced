////////////////////////////////////
////////////////////////////////////
//
//        TT_GeneratePlane
//
////////////////////////////////////
////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGeneratePlaneDecl();
CKERROR CreateGeneratePlaneProto(CKBehaviorPrototype **pproto);
int GeneratePlane(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGeneratePlaneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GeneratePlane");
    od->SetDescription("Generates plane");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ae72f89, 0x57eb5413));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneratePlaneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGeneratePlaneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GeneratePlane");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Scale", CKPGUID_VECTOR, "1,1,1");
    proto->DeclareInParameter("Size X", CKPGUID_INT, "1");
    proto->DeclareInParameter("Size Y", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GeneratePlane);

    *pproto = proto;
    return CK_OK;
}

int GeneratePlane(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateOutput(0, TRUE);

    VxVector position, scale;
    position.z = 0.0f;
    beh->GetInputParameterValue(0, &position);

    VxVector scaleVec;
    beh->GetInputParameterValue(1, &scaleVec);

    int sizeX = 1, sizeY = 1;
    beh->GetInputParameterValue(2, &sizeX);
    beh->GetInputParameterValue(3, &sizeY);

    CKMesh *mesh = (CKMesh *)context->CreateObject(CKCID_MESH, "GeneratedMesh");
    CK3dEntity *entity = (CK3dEntity *)context->CreateObject(CKCID_3DENTITY, "GeneratedEntity");

    mesh->SetVertexCount(4 * sizeX * sizeY);
    mesh->SetFaceCount(2 * sizeX * sizeY);

    int vertIdx = 0;
    int faceIdx = 0;

    for (int y = 0; y < sizeY; y++)
    {
        float negY0 = -(float)y;
        float negY1 = -(float)(y + 1);

        for (int x = 0; x < sizeX; x++)
        {
            float posX0 = (float)x;
            float posX1 = (float)(x + 1);

            VxVector v0, v1, v2, v3;
            v0.x = posX0; v0.y = 0.0f; v0.z = negY0;
            v1.x = posX0; v1.y = 0.0f; v1.z = negY1;
            v2.x = posX1; v2.y = 0.0f; v2.z = negY1;
            v3.x = posX1; v3.y = 0.0f; v3.z = negY0;

            mesh->SetVertexPosition(vertIdx, &v0);
            mesh->SetVertexPosition(vertIdx + 1, &v1);
            mesh->SetVertexPosition(vertIdx + 2, &v2);
            mesh->SetVertexPosition(vertIdx + 3, &v3);

            mesh->SetFaceVertexIndex(faceIdx, vertIdx, vertIdx + 1, vertIdx + 2);
            mesh->SetFaceVertexIndex(faceIdx + 1, vertIdx + 3, vertIdx, vertIdx + 2);

            vertIdx += 4;
            faceIdx += 2;
        }
    }

    mesh->BuildFaceNormals();
    entity->SetCurrentMesh(mesh, TRUE);
    entity->SetPosition(&position);
    entity->SetScale(&scaleVec);

    CKScene *scene = context->GetCurrentScene();
    scene->AddObjectToScene(entity, TRUE);

    return CKBR_OK;
}