/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT TextureSine
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorTextureSineDecl();
CKERROR CreateTextureSineProto(CKBehaviorPrototype **);
int TextureSine(const CKBehaviorContext &behcontext);
CKERROR TextureSineCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureSineDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT TextureSine");
    od->SetDescription("Produces a sinusoidal displacement on the UV coords of a mesh's material (or one of its channel).");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9C1208, 0x3A8D779E));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureSineProto);
    od->SetCompatibleClassId(CKCID_MESH);
    return od;
}

CKERROR CreateTextureSineProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT TextureSine");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("X Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Y Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Velocity", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);      // Data
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT, "0.0"); // Time

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextureSine);

    proto->SetBehaviorCallbackFct(TextureSineCallBack);

    *pproto = proto;
    return CK_OK;
}

int TextureSine(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float xamp = 0.1f;
    beh->GetInputParameterValue(0, &xamp);

    float yamp = 0.1f;
    beh->GetInputParameterValue(1, &yamp);

    float velocity = 1.0f;
    beh->GetInputParameterValue(2, &velocity);

    VxUV *savedUV = (VxUV *)beh->GetLocalParameterReadDataPtr(0);

    CKMesh *mesh = (CKMesh *)beh->GetTarget();

    int channel = -1;
    beh->GetInputParameterValue(3, &channel);
    int channelcount = mesh->GetChannelCount();
    if (channel < -1 || channel >= channelcount)
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        return CKBR_PARAMETERERROR;
    }

    CKDWORD Stride;
    VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride, channel);
    int pointsNumber = mesh->GetModifierUVCount(channel);

    float time;
    beh->GetLocalParameterValue(1, &time);

    float t;
    for (int i = 0; i < pointsNumber; i++, uvarray = (VxUV *)((BYTE *)uvarray + Stride))
    {
        t = ((float)i / pointsNumber - 0.5f) * 4.0f + time * l;
        uvarray->u = savedUV[i].u + (0.5f - savedUV[i].u) * xamp * cosf(t);
        uvarray->v = savedUV[i].v + (0.5f - savedUV[i].v) * yamp * sinf(t);
    }
    mesh->ModifierUVMove();

    float pi = 3.1415926535f;
    time += behcontext.DeltaTime * 0.001f;

    if (l * time > 2 * pi)
        time -= (2 * pi / l);

    beh->SetLocalParameterValue(1, &time);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR TextureSinusCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        // we get the mesh vertices
        CKMesh *mesh = (CKMesh *)beh->GetOwner();
        if (!mesh)
            return 0;

        CKDWORD Stride;
        VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride);
        int nbvert = mesh->GetModifierUVCount();

        VxUV *savedUV;
        savedUV = new VxUV[nbvert];

        for (int i = 0; i < nbvert; i++, uvarray = (VxUV *)((BYTE *)uvarray + Stride))
        {
            savedUV[i] = *uvarray;
        }

        beh->SetLocalParameterValue(0, savedUV, nbvert * sizeof(VxUV));

        delete[] savedUV;
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        // we get the mesh vertices
        if (!beh)
            return 0;
        CKMesh *mesh = (CKMesh *)beh->GetOwner();
        if (!mesh)
            return 0;

        CKDWORD Stride;
        VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride);
        int nbvert = mesh->GetModifierUVCount();

        VxUV *savePos = (VxUV *)beh->GetLocalParameterWriteDataPtr(0);
        if (!savePos)
            return 0;

        for (int i = 0; i < nbvert; i++, uvarray = (VxUV *)((BYTE *)uvarray + Stride))
        {
            *uvarray = savePos[i];
        }
        mesh->ModifierUVMove();
    }
    }
    return CKBR_OK;
}
