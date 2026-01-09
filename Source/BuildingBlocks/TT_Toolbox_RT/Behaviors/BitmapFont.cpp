/////////////////////////////////
/////////////////////////////////
//
//        TT_BitmapFont
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorBitmapFontDecl();
CKERROR CreateBitmapFontProto(CKBehaviorPrototype **pproto);
int BitmapFont(const CKBehaviorContext &behcontext);
CKERROR BitmapFontCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBitmapFontDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_BitmapFont");
    od->SetDescription("Creates an object text from a texture");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x173e60ff, 0x4db14bcc));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBitmapFontProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBitmapFontProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_BitmapFont");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Scale", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("cursiv", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Src Blend", CKPGUID_BLENDFACTOR);
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BitmapFont);

    proto->SetBehaviorCallbackFct(BitmapFontCallBack);

    *pproto = proto;
    return CK_OK;
}

int BitmapFont(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
    CKRenderContext *rc = behcontext.CurrentRenderContext;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Build UV lookup table (16x16 character grid = 256 chars)
        float *uvTable = new float[4096]; // 256 chars * 4 floats per char (u0, u1, v0, v1)
        int idx = 0;
        for (int charIdx = 0; charIdx < 256; charIdx++)
        {
            int col = charIdx % 16;
            int row = charIdx / 16;
            uvTable[idx++] = col * 0.0625f;           // u0
            uvTable[idx++] = (col + 1) * 0.0625f;     // u1
            uvTable[idx++] = row * 0.0625f;           // v0
            uvTable[idx++] = (row + 1) * 0.0625f;     // v1
        }
        beh->SetLocalParameterValue(0, &uvTable, sizeof(float *));
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(0);
    Vx2DVector position, scale;
    position.x = 0.0f;
    position.y = 0.0f;
    beh->GetInputParameterValue(1, &position);
    scale.x = 1.0f;
    scale.y = 1.0f;
    beh->GetInputParameterValue(2, &scale);

    const char *text = (const char *)beh->GetInputParameterReadDataPtr(3);
    if (!text)
        return CKBR_OK;

    CKBOOL cursiv = FALSE;
    beh->GetInputParameterValue(4, &cursiv);

    VXBLEND_MODE srcBlend, destBlend;
    beh->GetInputParameterValue(5, &srcBlend);
    beh->GetInputParameterValue(6, &destBlend);

    VxColor color;
    memset(&color, 0, sizeof(color));
    beh->GetInputParameterValue(7, &color);

    float *uvTable = *(float **)beh->GetLocalParameterReadDataPtr(0);
    int textLen = strlen(text);

    // Create mesh and entity for rendering
    CKMesh *mesh = (CKMesh *)context->CreateObject(CKCID_MESH, "Mesh");
    CK3dEntity *entity = (CK3dEntity *)context->CreateObject(CKCID_3DENTITY, "Ent");

    mesh->SetVertexCount(4 * textLen);
    mesh->SetFaceCount(2 * textLen);

    int vertIdx = 3;
    int faceIdx = 0;
    float posX = 0.0f;

    for (int i = 0; i < textLen; i++)
    {
        int charCode = (unsigned char)text[i] - 32;
        if (cursiv)
            charCode += 128;

        // Set vertex positions
        VxVector v;
        v.z = 10.0f;

        v.x = posX * scale.x + position.x;
        v.y = position.y;
        mesh->SetVertexPosition(vertIdx - 3, &v);

        v.x = posX * scale.x + position.x;
        v.y = scale.y + position.y;
        mesh->SetVertexPosition(vertIdx - 2, &v);

        v.x = (posX + 1.0f) * scale.x + position.x;
        v.y = scale.y + position.y;
        mesh->SetVertexPosition(vertIdx - 1, &v);

        v.x = (posX + 1.0f) * scale.x + position.x;
        v.y = position.y;
        mesh->SetVertexPosition(vertIdx, &v);

        // Set UV coordinates from lookup table
        int uvIdx = charCode * 4;
        mesh->SetVertexTextureCoordinates(vertIdx - 3, uvTable[uvIdx], uvTable[uvIdx + 3]);
        mesh->SetVertexTextureCoordinates(vertIdx - 2, uvTable[uvIdx], uvTable[uvIdx + 2]);
        mesh->SetVertexTextureCoordinates(vertIdx - 1, uvTable[uvIdx + 1], uvTable[uvIdx + 2]);
        mesh->SetVertexTextureCoordinates(vertIdx, uvTable[uvIdx + 1], uvTable[uvIdx + 3]);

        // Set faces
        mesh->SetFaceVertexIndex(faceIdx, vertIdx - 3, vertIdx - 2, vertIdx);
        mesh->SetFaceVertexIndex(faceIdx + 1, vertIdx - 2, vertIdx - 1, vertIdx);

        mesh->SetFaceMaterial(faceIdx, material);
        mesh->SetFaceMaterial(faceIdx + 1, material);

        vertIdx += 4;
        faceIdx += 2;
        posX += 1.0f;
    }

    mesh->BuildNormals();
    entity->SetCurrentMesh(mesh, TRUE);
    entity->SetPosition(0, 0, 0);
    rc->AddObject(entity);

    return CKBR_OK;
}

CKERROR BitmapFontCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    if (behcontext.CallbackMessage == CKM_BEHAVIORDEACTIVATESCRIPT ||
        behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKObject *entity = context->GetObjectByName("Ent");
        if (entity)
        {
            CK3dEntity *ent3d = (CK3dEntity *)entity;
            CKMesh *mesh = ent3d->GetCurrentMesh();
            CKDestroyObject(mesh);
            CKDestroyObject(entity);
        }

        float **uvTable = (float **)beh->GetLocalParameterReadDataPtr(0);
        if (uvTable && *uvTable)
        {
            delete[] *uvTable;
        }
    }

    return CKBR_OK;
}