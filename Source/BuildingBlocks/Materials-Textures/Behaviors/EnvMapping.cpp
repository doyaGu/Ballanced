/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Env Mapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorEnvMappingDecl();
CKERROR CreateEnvMappingProto(CKBehaviorPrototype **);
int EnvMapping(const CKBehaviorContext &behcontext);
int EnvMapPreRender(CKRenderContext *dev, CKRenderObject *rent, void *arg);

CKObjectDeclaration *FillBehaviorEnvMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Environment Mapping");
    od->SetDescription("Sets a 3D Entity to be rendered with 'pseudo' Environment Mapping.");
    /* rem:
    <IMG SRC='EnvMapping.gif' WIDTH='131' HEIGHT='151' ALIGN='RIGHT'>
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the environnement mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.
    <SPAN CLASS=pin>Options: </SPAN><BR>
    <U>Flip U:</U> if TRUE, flip mapping horizontally.<BR>
    <U>Flip V:</U> if TRUE, flip mapping vertically.<BR>
    <U>Use Ref Orientation:</U> if TRUE, the orientation of "Projection Ref" will be used to define U and V axis (U = X axis of "Projection Ref", and V = Y axis of "Projection Ref").
    If FALSE, the orientation will be calculated automatcally from the "Projection Ref"'s position.<BR>
    <SPAN CLASS=pin>Projection Ref: </SPAN>3d Entity standing for the virtual light's position/orientation from which the environment mapping projection is performed.<BR>
    <BR>
      The "Environement Mapping" BB change UV mapping coordinates in such a way that the mapped texture seems now to be a refection of light.<BR>
      To perform such an effect you should use some kind of radial map with a large bright disk at the center.<BR>
    Environment Mapping can thus be used to simulate Phong Shading.<BR>
      If channel = -1, the material(s) used to render the environemnt mapping effect is/are the object's current material(s).<BR>
    If channel > -1, the environemnt mapping effect is calculated on the specified channel, and the mesh's UV coordinates are only changed for this channel (not for the mesh itself).<BR>
    <BR>
    For an example, load the compositions "undocumented samples\ring.cmo", "technical samples\visuals\Envmap.cmo".
    */
    /* warning:
      - The "Environment Mapping" building block change the mesh's UV coordinates, thus, if you need to get back the initial mapping,
    you should set Initial Conditions on the mesh before activating the Environement Mapping BB.<BR>
    - Loop this building block to get a constant effect.<BR>
      - If "Use Ref Orientation" is TRUE and if "Projection Ref" is non-NULL, you should use a "Look At" building block (or simply a target light as a referential) to make sure the pseudi-light
      always faces the object.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x10d1c997, 0x99dabef4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateEnvMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreateEnvMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Environment Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
#define CKPGUID_ENVMAPPINGFLIPMODE CKGUID(0x11416cd2, 0x293c701e)
    proto->DeclareInParameter("Options", CKPGUID_ENVMAPPINGFLIPMODE, "Use Ref Orientation");
    proto->DeclareInParameter("Projection Ref", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(EnvMapping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int EnvMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    // Get Owner
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    ent->AddPreRenderCallBack(EnvMapPreRender, (void *)beh->GetID(), TRUE);

    return CKBR_OK;
}

/***********************************************/
//	PreRender Callback
/***********************************************/
int EnvMapPreRender(CKRenderContext *rc, CKRenderObject *rent, void *arg)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return 0;

    CK3dEntity *ent = (CK3dEntity *)rent;
    if (!ent)
        return 0;

    // Get Channel
    int channel = -1;
    beh->GetInputParameterValue(0, &channel);

    // Get Flip Mode
    int options = 0;
    beh->GetInputParameterValue(1, &options);

    VxVector vectUp = VxVector::axisY();

    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(2);
    if (!ref)
    {
        ref = rc->GetViewpoint();
        vectUp = ref->GetWorldMatrix()[1];
    }

    VxVector vup, vright;

    if (((options & 4) && ref) || (beh->GetVersion() < 0x00010005))
    {
        // Use Projection Referential Orientation
        ref->GetOrientation(NULL, &vup, &vright);
    }
    else
    {
        // if it's not an old version, we can calculate the projU projV vectors
        // so that they form a plane that faces the object
        VxVector pos_light, pos_object;
        ref->GetPosition(&pos_light);
        ent->GetPosition(&pos_object);
        VxVector vdir = pos_object - pos_light;
        vdir.Normalize();

        vup = vectUp - vdir * DotProduct(vectUp, vdir);
        vup.Normalize();

        vright = CrossProduct(vup, vdir);
    }

    // Flip U and V ( by default V must be negative )
    if (options & 1)
        vright = -vright;
    if (!(options & 2))
        vup = -vup;

    // Transform projU and projV in the object referential
    VxVector projU, projV;
    ent->InverseTransformVector(&projU, &vright);
    ent->InverseTransformVector(&projV, &vup);

    float tmp = 0.45f;
    projU = Normalize(projU);
    projU *= tmp;
    projV = Normalize(projV);
    projV *= tmp;

    // Get the object's mesh
    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_GENERICERROR;

    // Parse all vertices, and change UV coordinates
    // according to projU projV and the vertex Normal
    int count = mesh->GetVertexCount();

    CKDWORD nStride;
    VxVector *normal = (VxVector *)mesh->GetNormalsPtr(&nStride);
    if (!normal)
        return CKBR_GENERICERROR;

    CKDWORD uvStride;
    Vx2DVector *uv = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, channel);
    if (!uv)
        return CKBR_GENERICERROR;

    VxVector normal_tmp;

    // reflective mapping ?
    if (options & 8)
    {
        VxVector posRef;
        ref->GetPosition(&posRef, ent);

        CKDWORD posStride;
        VxVector *pos = (VxVector *)mesh->GetPositionsPtr(&posStride);
        if (!pos)
            return CKBR_GENERICERROR;

        for (int i = 0; i < count; ++i)
        {
            normal_tmp = Reflect(posRef - *pos, *normal);
            normal_tmp.Normalize();
            uv->x = 0.5f + normal_tmp.x * projU.x + normal_tmp.y * projU.y + normal_tmp.z * projU.z;
            uv->y = 0.5f + normal_tmp.x * projV.x + normal_tmp.y * projV.y + normal_tmp.z * projV.z;

            normal = (VxVector *)((CKBYTE *)normal + nStride);
            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
            pos = (VxVector *)((CKBYTE *)pos + posStride);
        }
    }
    else
    {
        for (int i = 0; i < count; ++i)
        {
            normal_tmp = *normal;
            uv->x = 0.5f + normal_tmp.x * projU.x + normal_tmp.y * projU.y + normal_tmp.z * projU.z;
            uv->y = 0.5f + normal_tmp.x * projV.x + normal_tmp.y * projV.y + normal_tmp.z * projV.z;

            normal = (VxVector *)((CKBYTE *)normal + nStride);
            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
        }
    }

    mesh->UVChanged();

    return 0; // no need to update extents
}
