#include "CKAll.h"

#include "ParticleManager.h"

#define PUTVERTEX(i, a, b, c) \
    V.x = a;                  \
    V.y = b;                  \
    V.z = c;                  \
    mesh->SetVertexPosition(i, &V)

void CreateCircle(CKMesh *mesh, int start, int count, float radius, VxVector &axis, CKDWORD col, float pos = 0.0f)
{
    float angle = 0;
    float step = 2.0f * PI / count;
    VxVector v(pos, pos, pos);
    int i;

    for (i = start; i < start + count; i++)
    {
        if (axis.x && axis.y)
        {
            v.x = radius * cosf(angle);
            v.y = radius * sinf(angle);
        }
        else if (axis.x && axis.z)
        {
            v.x = radius * cosf(angle);
            v.z = radius * sinf(angle);
        }
        else if (axis.y && axis.z)
        {
            v.y = radius * cosf(angle);
            v.z = radius * sinf(angle);
        }
        mesh->SetVertexPosition(i, &v);
        mesh->SetVertexColor(i, col);
        mesh->SetVertexSpecularColor(i, 0);
        angle += step;
    }
    for (i = start; i < start + count - 1; i++)
        mesh->SetLine(i, i, i + 1);
    mesh->SetLine(i, i, start);
}

void ParticleManager::CreateGlobalWindMesh()
{
    if (m_GlobalWindMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Global Wind Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_GlobalWindMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(8);
    mesh->SetLineCount(7);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, 0.5f, 0);
    PUTVERTEX(1, -s, -0.5f, 0);
    PUTVERTEX(2, s, -0.5f, 0);
    PUTVERTEX(3, s, 0.5f, 0);
    PUTVERTEX(4, 0, 0, 0);
    PUTVERTEX(5, 0, 0, s);
    PUTVERTEX(6, 0.5f, 0, 0.5f);
    PUTVERTEX(7, -0.5f, 0, 0.5f);

    for (int i = 0; i < 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(0.0f, 0.6f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 2);
    mesh->SetLine(1, 1, 3);
    mesh->SetLine(2, 4, 5);
    mesh->SetLine(3, 5, 6);
    mesh->SetLine(4, 5, 7);
    mesh->SetLine(5, 0, 1);
    mesh->SetLine(6, 2, 3);
}

void ParticleManager::CreateLocalWindMesh()
{
    if (m_LocalWindMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Local Wind Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_LocalWindMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetVertexCount(n + 8);
    mesh->SetLineCount(n + 5);
    int i;

    for (i = 0; i < n - 1; i++)
    {
        mesh->SetLine(i, i, i + 1);
    }
    mesh->SetLine(i, i, 0);
    VxVector v(0.0f, 0.0f, 0.0f);
    float angle = 0;
    float step = 2.0f * PI / n;
    for (i = 0; i < n + 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(0.0f, 0.9f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    for (i = 0; i < n; i++)
    {
        v.x = cosf(angle);
        v.y = sinf(angle);
        mesh->SetVertexPosition(i, &v);
        angle += step;
    }

    VxVector V;
    PUTVERTEX(n, 0, 0, 0);
    PUTVERTEX(n + 1, 0, 0, 1.0f);
    PUTVERTEX(n + 2, 0.5f, 0, 0.5f);
    PUTVERTEX(n + 3, -0.5f, 0, 0.5f);
    PUTVERTEX(n + 4, -0.705f, 0.705f, 0.0f);
    PUTVERTEX(n + 5, 0.705f, -0.705f, 0.0f);
    PUTVERTEX(n + 6, -0.705f, -0.705f, 0.0f);
    PUTVERTEX(n + 7, 0.705f, 0.705f, 0.0f);
    mesh->SetLine(n, n, n + 1);
    mesh->SetLine(n + 1, n + 1, n + 2);
    mesh->SetLine(n + 2, n + 1, n + 3);
    mesh->SetLine(n + 3, n + 4, n + 5);
    mesh->SetLine(n + 4, n + 6, n + 7);
}

void ParticleManager::CreateMagnetMesh()
{
    if (m_MagnetMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Magnet Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_MagnetMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n1 = 12;
    int n2 = 24;
    int n3 = 32;
    mesh->SetVertexCount(2 * (n1 + n2 + n3));
    mesh->SetLineCount(2 * (n1 + n2 + n3));

    VxVector axis(0.0f, 1.0f, 1.0f);
    CreateCircle(mesh, 0, n1, 0.3f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
    CreateCircle(mesh, n1, n2, 0.6f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
    CreateCircle(mesh, n1 + n2, n3, 1.0f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
    axis = VxVector(1.0f, 0.0f, 1.0f);
    CreateCircle(mesh, n1 + n2 + n3, n1, 0.3f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
    CreateCircle(mesh, 2 * n1 + n2 + n3, n2, 0.6f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
    CreateCircle(mesh, 2 * (n1 + n2) + n3, n3, 1.0f, axis, RGBAFTOCOLOR(0.7f, 0.0f, 0.0f, 1.0f));
}

void ParticleManager::CreateVortexMesh()
{
    if (m_VortexMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Vortex Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_VortexMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 64;
    mesh->SetVertexCount(n);
    mesh->SetLineCount(n - 1);
    int i;

    for (i = 0; i < n - 1; i++)
    {
        mesh->SetLine(i, i, i + 1);
    }
    // mesh->SetLine(i,i,0);
    VxVector v(0.0f, 0.0f, 0.0f);
    float angle = 0;
    float step = 6.0f * PI / n;
    float att = 1.0f;
    for (i = 0; i < n; i++)
    {
        v.x = att * cosf(angle);
        v.y = att * sinf(angle);
        v.z = 1.0f - att;
        mesh->SetVertexPosition(i, &v);
        mesh->SetVertexColor(i, RGBAFTOCOLOR(0.9f, 0.0f, 0.7f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
        angle += step;
        att -= 1.0f / n;
    }
}

void ParticleManager::CreateDisruptionBoxMesh()
{
    if (m_DisruptionBoxMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Disruption Box Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DisruptionBoxMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(8);
    mesh->SetLineCount(12);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, -s, -s);
    PUTVERTEX(1, s, -s, -s);
    PUTVERTEX(2, s, s, -s);
    PUTVERTEX(3, -s, s, -s);

    PUTVERTEX(4, -s, -s, s);
    PUTVERTEX(5, s, -s, s);
    PUTVERTEX(6, s, s, s);
    PUTVERTEX(7, -s, s, s);

    for (int i = 0; i < 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(0.0f, 0.0f, 0.7f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 2, 3);
    mesh->SetLine(3, 3, 0);

    mesh->SetLine(4, 4, 5);
    mesh->SetLine(5, 5, 6);
    mesh->SetLine(6, 6, 7);
    mesh->SetLine(7, 7, 4);

    mesh->SetLine(8, 0, 4);
    mesh->SetLine(9, 1, 5);
    mesh->SetLine(10, 2, 6);
    mesh->SetLine(11, 3, 7);
}

void ParticleManager::CreateMutationBoxMesh()
{
    if (m_MutationBoxMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Mutation Box Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_MutationBoxMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(8);
    mesh->SetLineCount(16);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, -s, -s);
    PUTVERTEX(1, s, -s, -s);
    PUTVERTEX(2, s, s, -s);
    PUTVERTEX(3, -s, s, -s);

    PUTVERTEX(4, -s, -s, s);
    PUTVERTEX(5, s, -s, s);
    PUTVERTEX(6, s, s, s);
    PUTVERTEX(7, -s, s, s);

    for (int i = 0; i < 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(0.0f, 0.5f, 0.7f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 2, 3);
    mesh->SetLine(3, 3, 0);

    mesh->SetLine(4, 4, 5);
    mesh->SetLine(5, 5, 6);
    mesh->SetLine(6, 6, 7);
    mesh->SetLine(7, 7, 4);

    mesh->SetLine(8, 0, 4);
    mesh->SetLine(9, 1, 5);
    mesh->SetLine(10, 2, 6);
    mesh->SetLine(11, 3, 7);

    // diagonals
    mesh->SetLine(12, 0, 6);
    mesh->SetLine(13, 1, 7);
    mesh->SetLine(14, 2, 4);
    mesh->SetLine(15, 3, 5);
}

void ParticleManager::CreateTunnelMesh()
{
    if (m_TunnelMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Tunnel Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_TunnelMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetVertexCount(2 * n);
    mesh->SetLineCount(2 * n + 4);

    VxVector axis(1.0f, 1.0f, 0.0f);
    CreateCircle(mesh, 0, n, 1.0f, axis, RGBAFTOCOLOR(0.0, 1.0, 0.0, 1.0), 0.0f);
    CreateCircle(mesh, n, n, 1.0f, axis, RGBAFTOCOLOR(0.0, 1.0, 0.0, 1.0), 1.0f);
    mesh->SetLine(2 * n, 0, n);
    mesh->SetLine(2 * n + 1, n / 4, 5 * n / 4);
    mesh->SetLine(2 * n + 2, n / 2, 3 * n / 2);
    mesh->SetLine(2 * n + 3, 3 * n / 4, 7 * n / 4);
}

void ParticleManager::CreateDPlaneMesh()
{
    if (m_DPlaneMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "DPlane Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DPlaneMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetLitMode(VX_PRELITMESH);
    mesh->SetVertexCount(4);
    mesh->SetLineCount(4);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, 0, -s);
    PUTVERTEX(1, s, 0, -s);
    PUTVERTEX(2, -s, 0, s);
    PUTVERTEX(3, s, 0, s);

    for (int i = 0; i < 4; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 0, 2);
    mesh->SetLine(2, 3, 1);
    mesh->SetLine(3, 2, 3);
}

void ParticleManager::CreateDInfinitePlaneMesh()
{
    if (m_DInfinitePlaneMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "DInfinite Plane Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DInfinitePlaneMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetLitMode(VX_PRELITMESH);
    mesh->SetVertexCount(4);
    mesh->SetLineCount(6);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, 0, -s);
    PUTVERTEX(1, s, 0, -s);
    PUTVERTEX(2, -s, 0, s);
    PUTVERTEX(3, s, 0, s);

    for (int i = 0; i < 4; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 0, 2);
    mesh->SetLine(2, 3, 1);
    mesh->SetLine(3, 2, 3);
    mesh->SetLine(4, 0, 3);
    mesh->SetLine(5, 1, 2);
}

void ParticleManager::CreateDCylinderMesh()
{
    if (m_DCylinderMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Cylinder Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DCylinderMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetVertexCount(2 * n);
    mesh->SetLineCount(2 * n + 4);

    VxVector axis(1.0f, 1.0f, 0.0f);
    CreateCircle(mesh, 0, n, 1.0f, axis, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0), -1.0f);
    CreateCircle(mesh, n, n, 1.0f, axis, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0), 1.0f);
    mesh->SetLine(2 * n, 0, n);
    mesh->SetLine(2 * n + 1, n / 4, 5 * n / 4);
    mesh->SetLine(2 * n + 2, n / 2, 3 * n / 2);
    mesh->SetLine(2 * n + 3, 3 * n / 4, 7 * n / 4);
}

void ParticleManager::CreateDSphereMesh()
{
    if (m_DSphereMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "DSphere Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DSphereMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetVertexCount(3 * n);
    mesh->SetLineCount(3 * n);

    VxVector axis(1.0f, 1.0f, 0.0f);
    CreateCircle(mesh, 0, n, 1.0f, axis, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
    axis = VxVector(1.0f, 0.0f, 1.0f);
    CreateCircle(mesh, n, n, 1.0f, axis, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
    axis = VxVector(0.0f, 1.0f, 1.0f);
    CreateCircle(mesh, 2 * n, n, 1.0f, axis, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
}

void ParticleManager::CreateDBoxMesh()
{
    if (m_DBoxMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "DBox Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DBoxMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(8);
    mesh->SetLineCount(12);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, -s, -s);
    PUTVERTEX(1, s, -s, -s);
    PUTVERTEX(2, s, s, -s);
    PUTVERTEX(3, -s, s, -s);

    PUTVERTEX(4, -s, -s, s);
    PUTVERTEX(5, s, -s, s);
    PUTVERTEX(6, s, s, s);
    PUTVERTEX(7, -s, s, s);

    for (int i = 0; i < 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0, 1.0, 0.0, 1.0));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 2, 3);
    mesh->SetLine(3, 3, 0);

    mesh->SetLine(4, 4, 5);
    mesh->SetLine(5, 5, 6);
    mesh->SetLine(6, 6, 7);
    mesh->SetLine(7, 7, 4);

    mesh->SetLine(8, 0, 4);
    mesh->SetLine(9, 1, 5);
    mesh->SetLine(10, 2, 6);
    mesh->SetLine(11, 3, 7);
}

void ParticleManager::CreatePointMesh()
{
    if (m_PointMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Point Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_PointMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(5);
    mesh->SetLineCount(4);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, 0, 0, 0);
    PUTVERTEX(1, 0, 0, s);
    PUTVERTEX(2, -0.5f, 0, 0.5f);
    PUTVERTEX(3, 0.5f, 0, 0.5f);
    PUTVERTEX(4, 0, 0, 0.5f);

    for (int i = 0; i < 5; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 4);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 1, 3);
    mesh->SetLine(3, 2, 3);
}

void ParticleManager::CreateLinearMesh()
{
    if (m_LinearMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Linear Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_LinearMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetLitMode(VX_PRELITMESH);
    mesh->SetVertexCount(7);
    mesh->SetLineCount(5);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, 0, 0, 0);
    PUTVERTEX(1, 0, 0, s);
    PUTVERTEX(2, -0.5f, 0, 0.5f);
    PUTVERTEX(3, 0.5f, 0, 0.5f);
    PUTVERTEX(4, -s, 0, 0);
    PUTVERTEX(5, s, 0, 0);
    PUTVERTEX(6, 0, 0, 0.5f);

    for (int i = 0; i < 7; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 6);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 1, 3);
    mesh->SetLine(3, 4, 5);
    mesh->SetLine(4, 2, 3);
}

void ParticleManager::CreatePlanarMesh()
{
    if (m_PlanarMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Planar Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_PlanarMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetLitMode(VX_PRELITMESH);
    mesh->SetVertexCount(9);
    mesh->SetLineCount(8);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, 0, 0, 0);
    PUTVERTEX(1, 0, 0, s);
    PUTVERTEX(2, -0.5f, 0, 0.5f);
    PUTVERTEX(3, 0.5f, 0, 0.5f);
    PUTVERTEX(4, -s, -s, 0);
    PUTVERTEX(5, s, -s, 0);
    PUTVERTEX(6, -s, s, 0);
    PUTVERTEX(7, s, s, 0);
    PUTVERTEX(8, 0, 0, 0.5f);

    for (int i = 0; i < 9; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 8);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 1, 3);
    mesh->SetLine(3, 4, 5);
    mesh->SetLine(4, 4, 6);
    mesh->SetLine(5, 7, 5);
    mesh->SetLine(6, 6, 7);
    mesh->SetLine(7, 2, 3);
}

void ParticleManager::CreateCubicMesh()
{
    if (m_CubicMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Cubic Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_CubicMesh = CKOBJID(mesh);

    // Creation of the line mesh
    mesh->SetVertexCount(8);
    mesh->SetLineCount(12);
    VxVector V;
    float s = 1.0f;

    PUTVERTEX(0, -s, -s, -s);
    PUTVERTEX(1, s, -s, -s);
    PUTVERTEX(2, s, s, -s);
    PUTVERTEX(3, -s, s, -s);

    PUTVERTEX(4, -s, -s, s);
    PUTVERTEX(5, s, -s, s);
    PUTVERTEX(6, s, s, s);
    PUTVERTEX(7, -s, s, s);

    for (int i = 0; i < 8; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(0, 0, 1);
    mesh->SetLine(1, 1, 2);
    mesh->SetLine(2, 2, 3);
    mesh->SetLine(3, 3, 0);

    mesh->SetLine(4, 4, 5);
    mesh->SetLine(5, 5, 6);
    mesh->SetLine(6, 6, 7);
    mesh->SetLine(7, 7, 4);

    mesh->SetLine(8, 0, 4);
    mesh->SetLine(9, 1, 5);
    mesh->SetLine(10, 2, 6);
    mesh->SetLine(11, 3, 7);
}

void ParticleManager::CreateDiscMesh()
{
    if (m_DiscMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Disc Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_DiscMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetLitMode(VX_PRELITMESH);
    mesh->SetVertexCount(n + 5);
    mesh->SetLineCount(n + 4);
    int i;

    for (i = 0; i < n - 1; i++)
    {
        mesh->SetLine(i, i, i + 1);
    }
    mesh->SetLine(i, i, 0);
    VxVector v(0.0f, 0.0f, 0.0f);
    float angle = 0;
    float step = 2.0f * PI / n;
    for (i = 0; i < n; i++)
    {
        v.x = cosf(angle);
        v.y = sinf(angle);
        mesh->SetVertexPosition(i, &v);
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
        angle += step;
    }

    VxVector V;
    PUTVERTEX(n, 0, 0, 0);
    PUTVERTEX(n + 1, 0, 0, 1.0f);
    PUTVERTEX(n + 2, 0.5f, 0, 0.5f);
    PUTVERTEX(n + 3, -0.5f, 0, 0.5f);
    PUTVERTEX(n + 4, 0, 0, 0.5f);
    for (i = n; i < n + 5; i++)
    {
        mesh->SetVertexColor(i, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f));
        mesh->SetVertexSpecularColor(i, 0);
    }
    mesh->SetLine(n, n, n + 4);
    mesh->SetLine(n + 1, n + 1, n + 2);
    mesh->SetLine(n + 2, n + 1, n + 3);
    mesh->SetLine(n + 3, n + 2, n + 3);
}

void ParticleManager::CreateCylindricalMesh()
{
    if (m_CylindricalMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Cylinder Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_CylindricalMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 32;
    mesh->SetVertexCount(2 * n);
    mesh->SetLineCount(2 * n + 4);

    VxVector axis(1.0f, 1.0f, 0.0f);
    CreateCircle(mesh, 0, n, 1.0f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), -1.0f);
    CreateCircle(mesh, n, n, 1.0f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), 1.0f);
    mesh->SetLine(2 * n, 0, n);
    mesh->SetLine(2 * n + 1, n / 4, 5 * n / 4);
    mesh->SetLine(2 * n + 2, n / 2, 3 * n / 2);
    mesh->SetLine(2 * n + 3, 3 * n / 4, 7 * n / 4);
}

void ParticleManager::CreateSphericalMesh()
{
    if (m_SphericalMesh)
        return;
    CKMesh *mesh = (CKMesh *)m_Context->CreateObject(CKCID_MESH, "Spherical Mesh");
    mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
    m_SphericalMesh = CKOBJID(mesh);

    // Creation of the line mesh
    int n = 16;
    mesh->SetVertexCount(6 * n);
    mesh->SetLineCount(6 * n);

    VxVector axis(1.0f, 0.0f, 1.0f);
    CreateCircle(mesh, 0, n, 0.42f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), 0.72f);
    CreateCircle(mesh, n, n, 0.77f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), 0.44f);
    CreateCircle(mesh, 2 * n, n, 0.96f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), 0.16f);
    CreateCircle(mesh, 3 * n, n, 0.98f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), -0.12f);
    CreateCircle(mesh, 4 * n, n, 0.8f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), -0.4f);
    CreateCircle(mesh, 5 * n, n, 0.48f, axis, RGBAFTOCOLOR(1.0f, 0.7f, 0.0f, 1.0f), -0.68f);
}
