/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TextureSinus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTextureSinusDecl();
CKERROR CreateTextureSinusProto(CKBehaviorPrototype **);
int TextureSinus(const CKBehaviorContext &behcontext);
CKERROR TextureSinusCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureSinusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Texture Sine");
    od->SetDescription("Produces a sinusoidal displacement on the UV coords of a mesh's material (or one of its channel).");

    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>X Amplitude: </SPAN>amplitude of sine displacement along the X axis of the texture <BR>
    <SPAN CLASS=pin>Y Amplitude: </SPAN>amplitude of sine displacement along the Y axis of the texture.<BR>
    <SPAN CLASS=pin>Velocity: </SPAN>radial speed expressed in radian/seconds.<BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.<BR>
    <BR>
    */
    /* warning:
    - The initial UV (mapping coordinates) on the mesh for this texture are saved when the building block is attached
    to the mesh (that's why we can't target this building block to another mesh than the one
    the building block is applied to), therefore you needn't to put Initials Conditions to the mesh if
    you wish to recover the initial mapping of the texture.<BR>
    - Beware, not all the building blocks work with this specification (eg: Texture Scroller need IC to recover the initials mapping).<BR>
    - The "Texture Sine" BB is a time based building block (i.e. it will execute at the same speed what ever the frame rate is
    [this means you needn't to add a per second building block in front of it]).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4fe0646e, 0x7e0635c9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureSinusProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Animation");
    return od;
}

CKERROR CreateTextureSinusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Texture Sine");

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("X Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Y Amplitude", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Velocity", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);      // Data
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT, "0.0"); // Time

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextureSinus);

    proto->SetBehaviorCallbackFct(TextureSinusCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int TextureSinus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the amount value
    float xamp = 0.1f;
    beh->GetInputParameterValue(0, &xamp);

    // we get the amount value
    float yamp = 0.1f;
    beh->GetInputParameterValue(1, &yamp);

    float l = 1.0f;
    beh->GetInputParameterValue(2, &l);

    // we get the saved uv's
    VxUV *savedUV = (VxUV *)beh->GetLocalParameterReadDataPtr(0);

    // we get the interpolated mesh
    CKMesh *mesh = (CKMesh *)beh->GetOwner();

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
    for (int i = 0; i < pointsNumber; i++, uvarray = (VxUV *)((CKBYTE *)uvarray + Stride))
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

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR TextureSinusCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
#ifdef macintosh
    case CKM_BEHAVIORLOAD:
    {
        CKMesh *mesh = (CKMesh *)beh->GetOwner();
        if (!mesh)
            return 0;
        int nbvert = mesh->GetModifierUVCount();
        // we get the saved uv's
        DWORD *savedUV = (DWORD *)beh->GetLocalParameterWriteDataPtr(0);

        for (int i = 0; i < nbvert * 2; i++)
        {
            savedUV[i] = ENDIANSWAP32(savedUV[i]);
        }
    }
    break;
#endif

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
        CKMesh *mesh = (CKMesh *)beh->GetOwner();
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
