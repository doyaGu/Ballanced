/////////////////////////////////////////
/////////////////////////////////////////
//
//        TT_SceneStencilShadow
//
/////////////////////////////////////////
/////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl();
CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **pproto);
int SceneStencilShadow(const CKBehaviorContext &behcontext);
CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext);
int StencilShadowRenderCallback(CKRenderContext *dev, CKBehavior *beh);

// Edge structure for shadow volume calculation (40 bytes per edge)
struct StencilEdge
{
    int vertex0;           // First vertex index
    int vertex1;           // Second vertex index
    int neighborFace;      // Adjacent face index (-1 if boundary)
    int face;              // Face this edge belongs to
    VxVector centerA;      // Center of face A
    VxVector centerB;      // Center of adjacent face B
};

// Silhouette edge pair for rendering
struct SilhouetteEdge
{
    int edge0;             // First silhouette edge vertex
    int edge1;             // Second silhouette edge vertex
};

// Per-object shadow data (40 bytes)
struct StencilShadowObjectData
{
    StencilEdge *edges;        // 0: Edge array
    int edgeCount;             // 4: Number of edges
    SilhouetteEdge *silhouette;// 8: Silhouette edge pairs
    CK3dEntity *shadowEntity;  // 12: Shadow volume entity
    CKMesh *shadowMesh;        // 16: Shadow volume mesh
    int silhouetteCount;       // 20: Number of silhouette edges
    float shadowLength;        // 24: Shadow extrusion length
    CKBYTE staticShadow;       // 28: Is shadow static?
    CKBYTE selfShadow;         // 29: Does object shadow itself?
    CKBYTE pad[2];
    CK3dEntity *sourceEntity;  // 32: Source entity
    CKMesh *sourceMesh;        // 36: Source mesh
};

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SceneStencilShadow");
    od->SetDescription("Creates StencilBuffer shadows (32Bit required!)");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4dcf4ca0, 0x474e25ec));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSceneStencilShadowProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SceneStencilShadow");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("ShadowColor", CKPGUID_COLOR, "0, 0, 0, 128");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);           // 0: Object data array
    proto->DeclareLocalParameter("", CKPGUID_2DENTITY);          // 1: Shadow overlay sprite
    proto->DeclareLocalParameter("", CKPGUID_MATERIAL);          // 2: Shadow material
    proto->DeclareLocalParameter("LightMatrix", CKPGUID_MATRIX); // 3: Light world matrix
    proto->DeclareLocalParameter("ObjectCount", CKPGUID_INT);    // 4: Number of shadow objects

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SceneStencilShadow);

    proto->SetBehaviorCallbackFct(SceneStencilShadowCallBack);

    *pproto = proto;
    return CK_OK;
}

// Check if an edge with vertices v0, v1 already exists in the edge list, and if so link it to face
static CKBOOL FindAndLinkEdge(StencilEdge *edges, CKMesh *mesh, int *edgeCount, 
                               int faceCount, int face, float x0, float y0, float z0,
                               float x1, float y1, float z1, int idx0, int idx1)
{
    for (int i = 0; i < *edgeCount; i++)
    {
        if (face == i)
            continue;

        // Get face vertices
        int fv0, fv1, fv2;
        mesh->GetFaceVertexIndex(i, fv0, fv1, fv2);

        VxVector v0, v1, v2;
        mesh->GetVertexPosition(fv0, &v0);
        mesh->GetVertexPosition(fv1, &v1);
        mesh->GetVertexPosition(fv2, &v2);

        // Check if this face shares the edge (in either direction)
        CKBOOL match = FALSE;
        if ((x0 == v0.x && y0 == v0.y && z0 == v0.z && x1 == v1.x && y1 == v1.y && z1 == v1.z) ||
            (x0 == v1.x && y0 == v1.y && z0 == v1.z && x1 == v0.x && y1 == v0.y && z1 == v0.z))
            match = TRUE;
        else if ((x0 == v1.x && y0 == v1.y && z0 == v1.z && x1 == v2.x && y1 == v2.y && z1 == v2.z) ||
                 (x0 == v2.x && y0 == v2.y && z0 == v2.z && x1 == v1.x && y1 == v1.y && z1 == v1.z))
            match = TRUE;
        else if ((x0 == v2.x && y0 == v2.y && z0 == v2.z && x1 == v0.x && y1 == v0.y && z1 == v0.z) ||
                 (x0 == v0.x && y0 == v0.y && z0 == v0.z && x1 == v2.x && y1 == v2.y && z1 == v2.z))
            match = TRUE;

        if (match)
        {
            // Link existing edge to this face
            // (Store the edge pairing info)
            return TRUE;
        }
    }
    return FALSE;
}

// Add a new edge or find existing shared edge
static void AddEdge(StencilEdge *edges, CKMesh *mesh, int *edgeCount, int faceCount, 
                    int face, int v0, int v1, int neighborFace)
{
    // Check if edge already exists
    for (int i = 0; i < *edgeCount; i++)
    {
        if ((edges[i].vertex0 == v0 && edges[i].vertex1 == v1) ||
            (edges[i].vertex0 == v1 && edges[i].vertex1 == v0))
        {
            // Edge exists, link neighbor
            return;
        }
    }

    // Add new edge
    int idx = *edgeCount;
    edges[idx].vertex0 = v0;
    edges[idx].vertex1 = v1;
    edges[idx].face = face;
    edges[idx].neighborFace = neighborFace;

    // Calculate face centers
    int fv0, fv1, fv2;
    mesh->GetFaceVertexIndex(face, fv0, fv1, fv2);
    VxVector p0, p1, p2;
    mesh->GetVertexPosition(fv0, &p0);
    mesh->GetVertexPosition(fv1, &p1);
    mesh->GetVertexPosition(fv2, &p2);
    edges[idx].centerA.x = (p0.x + p1.x + p2.x) / 3.0f;
    edges[idx].centerA.y = (p0.y + p1.y + p2.y) / 3.0f;
    edges[idx].centerA.z = (p0.z + p1.z + p2.z) / 3.0f;

    if (neighborFace >= 0)
    {
        mesh->GetFaceVertexIndex(neighborFace, fv0, fv1, fv2);
        mesh->GetVertexPosition(fv0, &p0);
        mesh->GetVertexPosition(fv1, &p1);
        mesh->GetVertexPosition(fv2, &p2);
        edges[idx].centerB.x = (p0.x + p1.x + p2.x) / 3.0f;
        edges[idx].centerB.y = (p0.y + p1.y + p2.y) / 3.0f;
        edges[idx].centerB.z = (p0.z + p1.z + p2.z) / 3.0f;
    }

    (*edgeCount)++;
}

// Post-render callback for stencil shadow rendering
int StencilShadowRenderCallback(CKRenderContext *dev, CKBehavior *beh)
{
    CKContext *ctx = dev->GetCKContext();

    // Get shadow data
    StencilShadowObjectData **dataArrayPtr = (StencilShadowObjectData **)beh->GetLocalParameterReadDataPtr(0);
    int objectCount = 0;
    beh->GetLocalParameterValue(4, &objectCount);

    CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(1);
    if (!sprite)
    {
        ctx->OutputToConsole("kein Sprite gefunden(Render)", FALSE);
    }

    if (!beh->GetLocalParameterObject(2))
    {
        ctx->OutputToConsole("kein Material gefunden(Render)", FALSE);
    }

    // Save world transform + states we override
    const VxMatrix savedWorld = dev->GetWorldTransformationMatrix();
    const CKDWORD savedZEnable = dev->GetState(VXRENDERSTATE_ZENABLE);
    const CKDWORD savedZWriteEnable = dev->GetState(VXRENDERSTATE_ZWRITEENABLE);
    const CKDWORD savedCullMode = dev->GetState(VXRENDERSTATE_CULLMODE);
    const CKDWORD savedLighting = dev->GetState(VXRENDERSTATE_LIGHTING);
    const CKDWORD savedSpecularEnable = dev->GetState(VXRENDERSTATE_SPECULARENABLE);
    const CKDWORD savedStencilEnable = dev->GetState(VXRENDERSTATE_STENCILENABLE);
    const CKDWORD savedStencilFunc = dev->GetState(VXRENDERSTATE_STENCILFUNC);
    const CKDWORD savedStencilFail = dev->GetState(VXRENDERSTATE_STENCILFAIL);
    const CKDWORD savedStencilZFail = dev->GetState(VXRENDERSTATE_STENCILZFAIL);
    const CKDWORD savedStencilPass = dev->GetState(VXRENDERSTATE_STENCILPASS);
    const CKDWORD savedStencilRef = dev->GetState(VXRENDERSTATE_STENCILREF);
    const CKDWORD savedStencilMask = dev->GetState(VXRENDERSTATE_STENCILMASK);
    const CKDWORD savedStencilWriteMask = dev->GetState(VXRENDERSTATE_STENCILWRITEMASK);

    // Set up stencil buffer states for 2-pass (z-pass) shadow volume
    dev->SetState(VXRENDERSTATE_LIGHTING, FALSE);
    dev->SetState(VXRENDERSTATE_SPECULARENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_STENCILENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_ALWAYS);
    dev->SetState(VXRENDERSTATE_STENCILFAIL, VXSTENCILOP_KEEP);
    dev->SetState(VXRENDERSTATE_STENCILZFAIL, VXSTENCILOP_KEEP);
    dev->SetState(VXRENDERSTATE_STENCILREF, 0);
    dev->SetState(VXRENDERSTATE_STENCILMASK, 0xFF);
    dev->SetState(VXRENDERSTATE_STENCILWRITEMASK, 0xFF);

    // First pass: increment stencil for front faces
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CCW);
    dev->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_INCR);
    if (objectCount > 0)
    {
        StencilShadowObjectData *dataArray = *dataArrayPtr;
        for (int i = 0; i < objectCount; i++)
        {
            StencilShadowObjectData *objData = &dataArray[i];
            
            // Get entity world matrix and set it
            dev->SetWorldTransformationMatrix(objData->sourceEntity->GetWorldMatrix());

            // Disable lighting for shadow mesh
            objData->shadowEntity->ModifyMoveableFlags(0, VX_MOVEABLE_RENDERLAST);

            // Render shadow mesh
            objData->shadowMesh->Render(dev, objData->shadowEntity);
        }
    }

    // Second pass: decrement stencil for back faces
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CW);
    dev->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_DECR);
    if (objectCount > 0)
    {
        StencilShadowObjectData *dataArray = *dataArrayPtr;
        for (int i = 0; i < objectCount; i++)
        {
            StencilShadowObjectData *objData = &dataArray[i];

            // Get entity world matrix
            dev->SetWorldTransformationMatrix(objData->sourceEntity->GetWorldMatrix());

            // Swap winding
            objData->shadowMesh->BuildNormals();

            // Render shadow mesh back faces
            objData->shadowMesh->Render(dev, objData->shadowEntity);

            // Restore
            objData->shadowMesh->BuildNormals();
        }
    }

    // Render shadow overlay
    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_STENCILENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_STENCILWRITEMASK, 0);
    dev->SetState(VXRENDERSTATE_STENCILFAIL, VXSTENCILOP_KEEP);
    dev->SetState(VXRENDERSTATE_STENCILZFAIL, VXSTENCILOP_KEEP);
    dev->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_NOTEQUAL);
    dev->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_KEEP);

    sprite->Show(CKSHOW);
    sprite->Render(dev);
    sprite->Show(CKHIDE);

    // Render self-shadowing objects
    for (int i = 0; i < objectCount; i++)
    {
        StencilShadowObjectData *dataArray = *dataArrayPtr;
        if (dataArray[i].selfShadow)
        {
            dataArray[i].sourceEntity->Render(dev, 255);
        }
    }

    // Restore states
    dev->SetWorldTransformationMatrix(savedWorld);
    dev->SetState(VXRENDERSTATE_ZENABLE, savedZEnable);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, savedZWriteEnable);
    dev->SetState(VXRENDERSTATE_CULLMODE, savedCullMode);
    dev->SetState(VXRENDERSTATE_LIGHTING, savedLighting);
    dev->SetState(VXRENDERSTATE_SPECULARENABLE, savedSpecularEnable);
    dev->SetState(VXRENDERSTATE_STENCILENABLE, savedStencilEnable);
    dev->SetState(VXRENDERSTATE_STENCILFUNC, savedStencilFunc);
    dev->SetState(VXRENDERSTATE_STENCILFAIL, savedStencilFail);
    dev->SetState(VXRENDERSTATE_STENCILZFAIL, savedStencilZFail);
    dev->SetState(VXRENDERSTATE_STENCILPASS, savedStencilPass);
    dev->SetState(VXRENDERSTATE_STENCILREF, savedStencilRef);
    dev->SetState(VXRENDERSTATE_STENCILMASK, savedStencilMask);
    dev->SetState(VXRENDERSTATE_STENCILWRITEMASK, savedStencilWriteMask);

    return 1;
}

int SceneStencilShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKRenderContext *rc = behcontext.CurrentRenderContext;

    // Handle Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Check for stencil buffer support
    int stencilBits = 0;
    rc->GetPixelFormat(NULL, NULL, &stencilBits);
    if (stencilBits <= 1)
    {
        ctx->OutputToConsole("keine Stencilbufferunterstuetzung!", TRUE);
        return CKBR_GENERICERROR;
    }

    // Get light
    CKLight *light = (CKLight *)beh->GetInputParameterObject(0);
    if (!light)
    {
        ctx->OutputToConsole("kein Licht ausgewaehlt!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Store light type
    VXLIGHT_TYPE lightType = light->GetType();

    // Get and store light matrix
    VxMatrix lightMatrix = light->GetWorldMatrix();
    beh->SetLocalParameterValue(3, &lightMatrix, sizeof(VxMatrix));

    // Add post-render callback
    rc->AddPostRenderCallBack((CK_RENDERCALLBACK)StencilShadowRenderCallback, beh, TRUE, FALSE);

    CKBOOL firstFrame = FALSE;

    // Handle On input - initialization
    if (beh->IsInputActive(0))
    {
        firstFrame = TRUE;
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Create shadow overlay sprite
        CKSprite *sprite = (CKSprite *)ctx->CreateObject(CKCID_SPRITE, "ShadowSprite", CK_OBJECTCREATION_RENAME);
        if (!sprite)
        {
            ctx->OutputToConsole("Sprite konnte nicht erstellt werden", TRUE);
            return CKBR_PARAMETERERROR;
        }

        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "ShadowMat", CK_OBJECTCREATION_RENAME);
        if (!mat)
        {
            ctx->OutputToConsole("Material konnte nicht erstellt werden", TRUE);
            return CKBR_PARAMETERERROR;
        }

        // Configure sprite
        sprite->Create(1, 1);
        Vx2DVector pos(0.0f, 0.0f);
        Vx2DVector size(1.0f, 1.0f);
        sprite->SetPosition(pos, FALSE, FALSE);
        sprite->SetSize(size, FALSE);

        // Configure material  
        VxColor shadowColor;
        beh->GetInputParameterValue(1, &shadowColor);
        
        VxColor black(0.0f, 0.0f, 0.0f, 1.0f);
        VxColor white(1.0f, 1.0f, 1.0f, 1.0f);
        mat->SetDiffuse(black);
        mat->SetEmissive(shadowColor);
        mat->SetAmbient(black);
        mat->SetSpecular(white);
        mat->SetTwoSided(TRUE);
        mat->SetSourceBlend(VXBLEND_SRCALPHA);
        mat->SetDestBlend(VXBLEND_INVSRCALPHA);
        sprite->SetMaterial(mat);
        sprite->Show(CKHIDE);

        beh->SetLocalParameterObject(1, sprite);
        beh->SetLocalParameterObject(2, mat);

        // Get StencilShadow attribute
        CKAttributeManager *attMan = ctx->GetAttributeManager();
        int attType = attMan->GetAttributeTypeByName("StencilShadow");
        const XObjectPointerArray &attObjects = attMan->GetGlobalAttributeListPtr(attType);
        int objCount = attObjects.Size();

        beh->SetLocalParameterValue(4, &objCount, sizeof(int));

        // Allocate per-object data
        StencilShadowObjectData *dataArray = new StencilShadowObjectData[objCount];
        if (dataArray)
        {
            memset(dataArray, 0, sizeof(StencilShadowObjectData) * objCount);
        }

        int dataIdx = 0;
        for (int i = 0; i < objCount; i++)
        {
            CK3dEntity *entity = (CK3dEntity *)attObjects[i];
            if (!entity->IsVisible())
                continue;

            StencilShadowObjectData *objData = &dataArray[dataIdx];

            // Read attribute parameters
            CKParameterOut *attParam = entity->GetAttributeParameter(attType);
            CK_ID *attData = (CK_ID *)attParam->GetReadDataPtr(TRUE);

            // Attribute format: MeshID, Static, ShadowLength, SelfShadow
            int meshId = 0;
            CKBYTE staticFlag = 0;
            float shadowLength = 0.0f;
            CKBYTE selfShadow = 0;

            CKParameter *p0 = (CKParameter *)ctx->GetObject(attData[0]);
            CKParameter *p1 = (CKParameter *)ctx->GetObject(attData[1]);
            CKParameter *p2 = (CKParameter *)ctx->GetObject(attData[2]);
            CKParameter *p3 = (CKParameter *)ctx->GetObject(attData[3]);

            p0->GetValue(&meshId);
            p1->GetValue(&staticFlag);
            p2->GetValue(&shadowLength);
            p3->GetValue(&selfShadow);

            objData->staticShadow = staticFlag;
            objData->shadowLength = shadowLength;
            objData->sourceEntity = entity;
            objData->selfShadow = selfShadow;

            // Get mesh from entity
            CKMesh *mesh = entity->GetMesh(meshId);
            if (!mesh)
            {
                ctx->OutputToConsole("3dEntity hat kein Mesh!", FALSE);
                return CKBR_PARAMETERERROR;
            }
            objData->sourceMesh = mesh;

            mesh->BuildNormals();

            // Build edge list
            int faceCount = mesh->GetFaceCount();
            int maxEdges = faceCount * 3; // Each face has 3 edges

            StencilEdge *edges = (StencilEdge *)new StencilEdge[maxEdges];
            if (edges)
            {
                memset(edges, 0, sizeof(StencilEdge) * maxEdges);
            }

            int edgeCount = 0;

            // Process all faces to build edge connectivity
            for (int f = 0; f < faceCount; f++)
            {
                int fv0, fv1, fv2;
                mesh->GetFaceVertexIndex(f, fv0, fv1, fv2);

                VxVector v0, v1, v2;
                mesh->GetVertexPosition(fv0, &v0);
                mesh->GetVertexPosition(fv1, &v1);
                mesh->GetVertexPosition(fv2, &v2);

                // Try to find shared edges with other faces
                // Edge 0-1
                if (!FindAndLinkEdge(edges, mesh, &edgeCount, faceCount, f, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, fv0, fv1))
                    AddEdge(edges, mesh, &edgeCount, faceCount, f, fv0, fv1, -1);
                
                // Edge 1-2  
                if (!FindAndLinkEdge(edges, mesh, &edgeCount, faceCount, f, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, fv1, fv2))
                    AddEdge(edges, mesh, &edgeCount, faceCount, f, fv1, fv2, -1);
                
                // Edge 2-0
                if (!FindAndLinkEdge(edges, mesh, &edgeCount, faceCount, f, v2.x, v2.y, v2.z, v0.x, v0.y, v0.z, fv2, fv0))
                    AddEdge(edges, mesh, &edgeCount, faceCount, f, fv2, fv0, -1);
            }

            objData->edges = edges;
            objData->edgeCount = edgeCount;

            // Allocate silhouette array
            SilhouetteEdge *silhouette = new SilhouetteEdge[edgeCount];
            if (silhouette)
            {
                memset(silhouette, 0, sizeof(SilhouetteEdge) * edgeCount);
            }
            objData->silhouette = silhouette;

            // Create shadow entity and mesh
            CK3dEntity *shadowEnt = (CK3dEntity *)ctx->CreateObject(CKCID_3DENTITY, "ShadowEnt", CK_OBJECTCREATION_RENAME);
            if (!shadowEnt)
            {
                ctx->OutputToConsole("ShadowEntity konnte nicht erstellt werden", TRUE);
                return CKBR_PARAMETERERROR;
            }

            CKMesh *shadowMesh = (CKMesh *)ctx->CreateObject(CKCID_MESH, "ShadowMesh", CK_OBJECTCREATION_RENAME);
            if (!shadowMesh)
            {
                ctx->OutputToConsole("ShadowMesh konnte nicht erstellt werden", TRUE);
                return CKBR_PARAMETERERROR;
            }

            shadowEnt->SetCurrentMesh(shadowMesh, TRUE);
            objData->shadowEntity = shadowEnt;
            objData->shadowMesh = shadowMesh;

            dataIdx++;
        }

        beh->SetLocalParameterValue(0, &dataArray, sizeof(void *));
    }

    // Update shadow volumes each frame
    StencilShadowObjectData **dataArrayPtr = (StencilShadowObjectData **)beh->GetLocalParameterReadDataPtr(0);
    int objectCount = 0;
    beh->GetLocalParameterValue(4, &objectCount);

    if (objectCount <= 0)
        return CKBR_ACTIVATENEXTFRAME;

    StencilShadowObjectData *dataArray = *dataArrayPtr;

    for (int i = 0; i < objectCount; i++)
    {
        StencilShadowObjectData *objData = &dataArray[i];

        // Skip static shadows after first frame
        if (objData->staticShadow && !firstFrame)
            continue;

        // Get light direction/position relative to entity
        VxVector lightDir;
        CK3dEntity *entity = objData->sourceEntity;

        if (lightType == VX_LIGHTDIREC)
        {
            // Directional light : derive direction from light world matrix (entity space)
            const VxMatrix &lightWorld = light->GetWorldMatrix();
            const VxVector worldDir(lightWorld[2][0], lightWorld[2][1], lightWorld[2][2]);
            entity->InverseTransformVector(&lightDir, &worldDir, NULL);
            const float len = lightDir.Magnitude();
            if (len > 0.0f)
                lightDir /= len;
            lightDir *= -1.0f;
        }
        else if (lightType == VX_LIGHTPOINT)
        {
            // Point light - calculate direction from light to entity
            CKMesh *mesh = objData->sourceMesh;
            CK3dEntity *target = entity->GetParent();
            if (target)
            {
                VxVector lightPos;
                light->GetPosition(&lightPos, entity);

                // Use mesh center
                VxVector center;
                // Just use first vertex for direction
                mesh->GetVertexPosition(0, &center);
                
                lightDir = lightPos - center;
                float len = lightDir.Magnitude();
                if (len > 0.0f)
                    lightDir /= len;
            }
        }
        else
        {
            // Spot or other - treat like directional
            const VxMatrix &lightWorld = light->GetWorldMatrix();
            const VxVector worldDir(lightWorld[2][0], lightWorld[2][1], lightWorld[2][2]);
            entity->InverseTransformVector(&lightDir, &worldDir, NULL);
            const float len = lightDir.Magnitude();
            if (len > 0.0f)
                lightDir /= len;
            lightDir *= -1.0f;
        }

        // Find silhouette edges
        int silCount = 0;
        CKMesh *mesh = objData->sourceMesh;

        for (int e = 0; e < objData->edgeCount; e++)
        {
            StencilEdge *edge = &objData->edges[e];

            // Get vertex normal
            VxVector normal;
            mesh->GetVertexNormal(edge->vertex0, &normal);

            // Check if this edge is on the silhouette (one face facing light, one away)
            float dot = DotProduct(normal, lightDir);

            if (edge->neighborFace == -1)
            {
                // Boundary edge - silhouette if face is lit
                if (dot < 0.0f)
                {
                    objData->silhouette[silCount].edge0 = edge->vertex1;
                    objData->silhouette[silCount].edge1 = edge->vertex0;
                    silCount++;
                }
            }
            else
            {
                // Internal edge - get neighbor normal
                VxVector normal2;
                mesh->GetVertexNormal(edge->neighborFace, &normal2);

                // For point lights, recalculate direction at each vertex
                if (lightType == VX_LIGHTPOINT)
                {
                    VxVector lightPos;
                    light->GetPosition(&lightPos, entity);
                    
                    VxVector v;
                    mesh->GetVertexPosition(edge->vertex0, &v);
                    lightDir = lightPos - v;
                    float len = lightDir.Magnitude();
                    if (len > 0.0f)
                        lightDir /= len;
                    dot = DotProduct(normal, lightDir);
                }

                CKBOOL face0Lit = (dot >= 0.0f);

                // Check neighbor face
                if (lightType == VX_LIGHTPOINT)
                {
                    VxVector lightPos;
                    light->GetPosition(&lightPos, entity);
                    
                    VxVector v;
                    mesh->GetVertexPosition(edge->neighborFace, &v);
                    lightDir = lightPos - v;
                    float len = lightDir.Magnitude();
                    if (len > 0.0f)
                        lightDir /= len;
                }

                float dot2 = DotProduct(normal2, lightDir);
                CKBOOL face1Lit = (dot2 >= 0.0f);

                if (face0Lit != face1Lit)
                {
                    // Silhouette edge
                    if (face0Lit)
                    {
                        objData->silhouette[silCount].edge0 = edge->vertex0;
                        objData->silhouette[silCount].edge1 = edge->vertex1;
                    }
                    else
                    {
                        objData->silhouette[silCount].edge0 = edge->vertex1;
                        objData->silhouette[silCount].edge1 = edge->vertex0;
                    }
                    silCount++;
                }
            }
        }

        objData->silhouetteCount = silCount;

        // Build shadow volume mesh from silhouette
        CKMesh *shadowMesh = objData->shadowMesh;
        shadowMesh->SetVertexCount(silCount * 4);
        shadowMesh->SetFaceCount(silCount * 2);

        for (int s = 0; s < silCount; s++)
        {
            VxVector v0, v1;
            mesh->GetVertexPosition(objData->silhouette[s].edge0, &v0);
            mesh->GetVertexPosition(objData->silhouette[s].edge1, &v1);

            // Calculate extrusion direction
            VxVector extrudeDir;
            if (lightType == VX_LIGHTPOINT)
            {
                VxVector lightPos;
                light->GetPosition(&lightPos, entity);

                // Direction from light to v0
                extrudeDir = v0 - lightPos;
                float len = extrudeDir.Magnitude();
                if (len > 0.0f)
                    extrudeDir /= len;
                extrudeDir *= objData->shadowLength;
                VxVector v0Ext = v0 - extrudeDir;

                // Direction from light to v1
                extrudeDir = v1 - lightPos;
                len = extrudeDir.Magnitude();
                if (len > 0.0f)
                    extrudeDir /= len;
                extrudeDir *= objData->shadowLength;
                VxVector v1Ext = v1 - extrudeDir;

                // Set 4 vertices for this quad
                int baseVert = s * 4;
                shadowMesh->SetVertexPosition(baseVert, &v0);
                shadowMesh->SetVertexPosition(baseVert + 1, &v1);
                shadowMesh->SetVertexPosition(baseVert + 2, &v0Ext);
                shadowMesh->SetVertexPosition(baseVert + 3, &v1Ext);

                // Set 2 triangles
                int baseFace = s * 2;
                shadowMesh->SetFaceVertexIndex(baseFace, baseVert, baseVert + 2, baseVert + 1);
                shadowMesh->SetFaceVertexIndex(baseFace + 1, baseVert + 1, baseVert + 2, baseVert + 3);
            }
            else
            {
                // Directional light - constant extrusion direction
                VxVector dirLocal;
                const VxMatrix &lightWorld = light->GetWorldMatrix();
                const VxVector worldDir(lightWorld[2][0], lightWorld[2][1], lightWorld[2][2]);
                entity->InverseTransformVector(&dirLocal, &worldDir, NULL);
                float len = dirLocal.Magnitude();
                if (len > 0.0f)
                    dirLocal /= len;
                extrudeDir = dirLocal * objData->shadowLength;

                VxVector v0Ext = v0 - extrudeDir;
                VxVector v1Ext = v1 - extrudeDir;

                int baseVert = s * 4;
                shadowMesh->SetVertexPosition(baseVert, &v0);
                shadowMesh->SetVertexPosition(baseVert + 1, &v1);
                shadowMesh->SetVertexPosition(baseVert + 2, &v0Ext);
                shadowMesh->SetVertexPosition(baseVert + 3, &v1Ext);

                int baseFace = s * 2;
                shadowMesh->SetFaceVertexIndex(baseFace, baseVert, baseVert + 2, baseVert + 1);
                shadowMesh->SetFaceVertexIndex(baseFace + 1, baseVert + 1, baseVert + 2, baseVert + 3);
            }
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        // Clean up allocated resources
        StencilShadowObjectData **dataArrayPtr = (StencilShadowObjectData **)beh->GetLocalParameterReadDataPtr(0);
        if (dataArrayPtr && *dataArrayPtr)
        {
            int objectCount = 0;
            beh->GetLocalParameterValue(4, &objectCount);

            StencilShadowObjectData *dataArray = *dataArrayPtr;
            for (int i = 0; i < objectCount; i++)
            {
                if (dataArray[i].edges)
                {
                    delete[] dataArray[i].edges;
                    dataArray[i].edges = NULL;
                }
                if (dataArray[i].silhouette)
                {
                    delete[] dataArray[i].silhouette;
                    dataArray[i].silhouette = NULL;
                }
            }
            delete[] dataArray;

            void *nullPtr = NULL;
            beh->SetLocalParameterValue(0, &nullPtr, sizeof(void *));
        }

        // Destroy created objects
        CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(1);
        if (sprite)
        {
            ctx->DestroyObject(sprite);
            beh->SetLocalParameterObject(1, NULL);
        }

        CKMaterial *mat = (CKMaterial *)beh->GetLocalParameterObject(2);
        if (mat)
        {
            ctx->DestroyObject(mat);
            beh->SetLocalParameterObject(2, NULL);
        }
    }
    break;
    }

    return CKBR_OK;
}