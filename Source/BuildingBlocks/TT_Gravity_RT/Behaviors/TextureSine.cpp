/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT TextureSine
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorTextureSineDecl();
CKERROR CreateTextureSineProto(CKBehaviorPrototype **pproto);
int TextureSine(const CKBehaviorContext &behcontext);
CKERROR TextureSineCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureSineDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_TextureSine");
    od->SetDescription("Produces a sinusoidal displacement on the UV coords of a mesh's material (or one of its channel).");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9c1208, 0x3a8d779e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureSineProto);
    od->SetCompatibleClassId(CKCID_MESH);
    return od;
}

CKERROR CreateTextureSineProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_TextureSine");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("X Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Y Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Velocity", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("", CKPGUID_FLOAT, "0.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextureSine);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
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

    CKDWORD stride;
    VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&stride, channel);
    int nbvert = mesh->GetModifierUVCount(channel);

    float time;
    beh->GetLocalParameterValue(1, &time);

    float t;
    for (int i = 0; i < nbvert; i++, uvarray = (VxUV *)((CKBYTE *)uvarray + stride))
    {
        t = ((float)i / nbvert - 0.5f) * 4.0f + time * velocity;
        uvarray->u = savedUV[i].u + (0.5f - savedUV[i].u) * xamp * cosf(t);
        uvarray->v = savedUV[i].v + (0.5f - savedUV[i].v) * yamp * sinf(t);
    }
    mesh->ModifierUVMove();

    time += behcontext.DeltaTime * 0.001f;
    if (time * velocity > 2 * PI)
        time -= (2 * PI / velocity);
    beh->SetLocalParameterValue(1, &time);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}

CKERROR TextureSineCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        // we get the mesh vertices
        CKMesh *mesh = (CKMesh *)beh->GetTarget();
        if (!mesh)
            return 0;

        CKDWORD Stride;
        VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride);
        int nbvert = mesh->GetModifierUVCount();

        VxUV *savedUV;
        savedUV = new VxUV[nbvert];

        for (int i = 0; i < nbvert; i++, uvarray = (VxUV *)((CKBYTE *)uvarray + Stride))
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
        CKMesh *mesh = (CKMesh *)beh->GetTarget();
        if (!mesh)
            return 0;

        CKDWORD Stride;
        VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride);
        int nbvert = mesh->GetModifierUVCount();

        VxUV *savePos = (VxUV *)beh->GetLocalParameterWriteDataPtr(0);
        if (!savePos)
            return 0;

        for (int i = 0; i < nbvert; i++, uvarray = (VxUV *)((CKBYTE *)uvarray + Stride))
        {
            *uvarray = savePos[i];
        }
        mesh->ModifierUVMove();
    }
    }
    return CKBR_OK;
}
