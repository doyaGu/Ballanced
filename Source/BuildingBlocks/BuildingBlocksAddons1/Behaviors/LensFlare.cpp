/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            LensFlare
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLensFlareDecl();
CKERROR CreateLensFlareProto(CKBehaviorPrototype **);
int LensFlare(const CKBehaviorContext &behcontext);
CKERROR LensFlareCallback(const CKBehaviorContext &behcontext);
void RenderLensFlare(CKRenderContext *dev, void *arg);

#define FLARE_APPEARING     1
#define FLARE_DISAPPEARING  2
#define FLARE_VISIBLE       4

struct Flare
{
    // Default Constructor
    Flare()
    {
        m_Position = 1.0f;
        m_Scale.Set(1.0f, 1.0f);
        m_AppearingTime = 100;
        m_DisappearingTime = 100;
        m_AngularSpeed = 0.0f;
        m_CurrentTime = 0;
        m_Color.Set(1.0f, 1.0f, 1.0f);
        m_Aperture = 0.0f;
        m_Mode = 0;
    }
    float m_Position; // postion on axis
    float m_AngularSpeed;
    Vx2DVector m_Scale; // relative size

    short int m_AppearingTime;
    short int m_DisappearingTime;
    float m_CurrentTime;
    float m_Aperture;
    CKDWORD m_Mode; // AppearingMode

    VxColor m_Color;        // color
    VxRect m_TextureOffset; // texture
};

struct FlaresArray
{
    ///
    // Methods

    // Default Constructor
    FlaresArray()
    {
        m_Flares = new Flare[1];
        m_Count = 1;
    }
    ~FlaresArray()
    {
        delete[] m_Flares;
    }
    void CheckSize(int s)
    {
        if (s != m_Count)
        {
            m_Count = s;
            delete[] m_Flares;
            m_Flares = new Flare[s];
        }
    }

    ///
    // Members
    Flare *m_Flares;
    int m_Count;
};

CKObjectDeclaration *FillBehaviorLensFlareDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Lens Flare");
    od->SetDescription("Renders the Light with a specific 'Light Texture'.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Flare Array: </SPAN>array containing flares, glares and glow values (see below).<BR>
    <SPAN CLASS=pin>Flare Texture: </SPAN>texture containing one or several flares, glares or glows elements.<BR>
    <SPAN CLASS=pin>Size Scale: </SPAN>A Global flares size miltiplier.<BR>
    <BR>
    <SPAN CLASS=setting>Read Array Each Frame: </SPAN>A Global flares size multiplier.<BR>
    <BR>
    The array you provide in input will contain rings, glares, glows, streaks or whatever you can imagine of.
    Depending on the size, the color and the texture element you use, you'll achieve completly different effects.
    It must have the following format:<BR>
    <FONT COLOR=#000077>Texture Region (Parameter Rectangle): </FONT>rectangle defining the flare in the texture. (0,0,0,0) means the entire texture.<BR>
    <FONT COLOR=#000077>Position (Float): </FONT>position of the flare on the virtual line, going through the light and
    intersecting the center of the screen. 1 means the light, 0 means the center of the screen.<BR>
    <FONT COLOR=#000077>Color (Parameter Color): </FONT>color of the flare.<BR>
    <FONT COLOR=#000077>Size (Parameter 2D Vector): </FONT>horizontal and Vertical size of the flare.<BR>
    <FONT COLOR=#000077>Angular Speed (Float): </FONT>rotating speed of the flare when it moves. 0 means no rotation at all.<BR>
    <FONT COLOR=#000077>Fade In Time (Integer): </FONT>time in ms for the flare to appear.<BR>
    <FONT COLOR=#000077>Fade Out Time (Integer): </FONT>time in ms for the flare to disappear.<BR>
    <FONT COLOR=#000077>Visibility Angle (Parameter Angle): </FONT>angle from the center of the camera in which the flare is visible.
    0 means in fact 90� so the entire half space before the camera. Use different values between 10 and 30� for the rings
    to see them disappear one after one.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x76926718, 0x7ee807d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateLensFlareProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/FX");
    return od;
}

CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Lens Flare");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Flare Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Flare Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Scale", CKPGUID_2DVECTOR, "1.0,1.0");

    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER); // Flares Data
    proto->DeclareSetting("Read Array Each Frame", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LensFlare);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetBehaviorCallbackFct(LensFlareCallback);

    *pproto = proto;
    return CK_OK;
}

int LensFlare(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKBOOL readarray = FALSE;
    beh->GetLocalParameterValue(1, &readarray);

    if (beh->IsInputActive(0))
    {
        // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        // We read the array the first time
        readarray = TRUE;
    }
    else
    {
        // we enter by off
        if (beh->IsInputActive(1))
        {
            // We enter by 'ON'
            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
    }

    behcontext.CurrentRenderContext->AddPostRenderCallBack(RenderLensFlare, (void *)beh->GetID(), TRUE);

    if (readarray)
    {
        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
        // Reading Flares Values
        FlaresArray *flares = *(FlaresArray **)beh->GetLocalParameterReadDataPtr(0);
        if (array)
        {
            // Array Specified
            // we check for the number of Flares
            flares->CheckSize(array->GetRowCount());

            // We now fill the array
            CKGUID guid;
            CK_ARRAYTYPE at;
            for (int i = 0; i < flares->m_Count; ++i)
            {
                ///
                // Texture Offset
                guid = array->GetColumnParameterGuid(0);
                if (guid == CKPGUID_RECT)
                { // Rectangle specified
                    array->GetElementValue(i, 0, &flares->m_Flares[i].m_TextureOffset);
                }

                ///
                // Position
                at = array->GetColumnType(1);
                if (at == CKARRAYTYPE_FLOAT)
                {
                    // float specified
                    array->GetElementValue(i, 1, &flares->m_Flares[i].m_Position);
                }

                ///
                // Color
                guid = array->GetColumnParameterGuid(2);
                if (guid == CKPGUID_COLOR)
                {
                    // Color specified
                    array->GetElementValue(i, 2, &flares->m_Flares[i].m_Color);
                }

                ///
                // Size
                guid = array->GetColumnParameterGuid(3);
                if (guid == CKPGUID_2DVECTOR)
                {
                    // 2DVector specified
                    array->GetElementValue(i, 3, &flares->m_Flares[i].m_Scale);
                }

                ///
                // Angular Speed
                at = array->GetColumnType(4);
                if (at == CKARRAYTYPE_FLOAT)
                {
                    // float specified
                    array->GetElementValue(i, 4, &flares->m_Flares[i].m_AngularSpeed);
                }

                ///
                // Appearing Time
                at = array->GetColumnType(5);
                if (at == CKARRAYTYPE_INT)
                {
                    // Time specified
                    int t;
                    array->GetElementValue(i, 5, &t);
                    flares->m_Flares[i].m_AppearingTime = t;
                }

                ///
                // Disappearing Time
                at = array->GetColumnType(6);
                if (at == CKARRAYTYPE_INT)
                {
                    // Time specified
                    int t;
                    array->GetElementValue(i, 6, &t);
                    flares->m_Flares[i].m_DisappearingTime = t;
                }

                ///
                // Visibility Focal
                guid = array->GetColumnParameterGuid(7);
                if (guid == CKPGUID_ANGLE)
                {
                    // Angle specified
                    float angle = 0.0f;
                    array->GetElementValue(i, 7, &angle);
                    if (angle == 0.0f)
                        angle = 1.57f;
                    flares->m_Flares[i].m_Aperture = cosf(angle);
                }
            }
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR LensFlareCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        // Default behavior, one Texture at the exact place of the light
        FlaresArray *farray = new FlaresArray;
        beh->SetLocalParameterValue(0, &farray, sizeof(FlaresArray *));
    }
    break;
    case CKM_BEHAVIORDELETE:
    {
        FlaresArray *flares = *(FlaresArray **)beh->GetLocalParameterReadDataPtr(0);
        delete flares;
    }
    break;
    }
    return CKBR_OK;
}

/*******************************************************/
/****************** RENDER FUNCTIONS *******************/
void RenderLensFlare(CKRenderContext *dev, void *arg)
{
    class FunctionCaller
    {
    public:
        CKBOOL FadeOutFlares(Flare *flares, int flarescount)
        {
            CKBOOL somevisibles = FALSE;
            for (int i = 0; i < flarescount; ++i)
            {
                if (flares[i].m_Mode)
                {
                    somevisibles = TRUE;
                    if (!(flares[i].m_Mode & FLARE_DISAPPEARING))
                    {
                        if (flares[i].m_Mode & FLARE_APPEARING)
                        {
                            flares[i].m_CurrentTime = flares[i].m_DisappearingTime * flares[i].m_CurrentTime / flares[i].m_AppearingTime;
                        }
                        else
                            flares[i].m_CurrentTime = flares[i].m_DisappearingTime;
                        flares[i].m_Mode = FLARE_DISAPPEARING;
                    }
                }
            }
            // Nothing visible ?
            return somevisibles;
        }
        CKBOOL FadeInFlares(Flare *flares, int flarescount, float dp)
        {
            CKBOOL somevisibles = FALSE;
            for (int i = 0; i < flarescount; ++i)
            {
                if (flares[i].m_Aperture < dp)
                {
                    // Visible
                    somevisibles = TRUE;
                    // The Flares not visible or disappearing must appear
                    if (!flares[i].m_Mode || flares[i].m_Mode & FLARE_DISAPPEARING)
                    {
                        if (flares[i].m_Mode & FLARE_DISAPPEARING)
                        {
                            flares[i].m_CurrentTime = flares[i].m_AppearingTime * flares[i].m_CurrentTime / flares[i].m_DisappearingTime;
                        }
                        else
                            flares[i].m_CurrentTime = 0;
                        flares[i].m_Mode = FLARE_APPEARING;
                    }
                }
                else
                {
                    // Not Visible
                    if (flares[i].m_Mode && !(flares[i].m_Mode & FLARE_DISAPPEARING))
                    {
                        // Was Visible
                        somevisibles = TRUE;
                        if (flares[i].m_Mode & FLARE_APPEARING)
                        {
                            flares[i].m_CurrentTime = flares[i].m_DisappearingTime * flares[i].m_CurrentTime / flares[i].m_AppearingTime;
                        }
                        else
                            flares[i].m_CurrentTime = flares[i].m_DisappearingTime;
                        flares[i].m_Mode = FLARE_DISAPPEARING;
                    }
                }
            }
            // Nothing visible ?
            return somevisibles;
        }
    };

    FunctionCaller fc;

    CKContext *ctx = dev->GetCKContext();
    CKBehavior *beh = (CKBehavior *)ctx->GetObject((CK_ID)arg);
    if (!beh)
        return;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return;

    FlaresArray *flaresarray = *(FlaresArray **)beh->GetLocalParameterReadDataPtr(0);
    if (!flaresarray)
        return;

    Flare *flares = flaresarray->m_Flares;
    int flarescount = flaresarray->m_Count;

    // Camera
    CKCamera *cam = dev->GetAttachedCamera();
    if (!cam)
        return;
    VxVector lookAt(0, 0, cam->GetFrontPlane() + 0.01f);

    // Light
    VxVector lightpos, worldlightpos;
    light->GetPosition(&worldlightpos);
    cam->InverseTransform(&lightpos, &worldlightpos);

    // Light Transformed
    VxVector light2dpos;
    dev->Transform(&light2dpos, &lightpos, cam);

    VxRect windowRect;
    dev->GetWindowRect(windowRect);

    light2dpos.x += windowRect.left;
    light2dpos.y += windowRect.top;

    // First we try to see if the light will produce lens flares :
    // it must be in the fore plane of the camera
    float dp = DotProduct(Normalize(lookAt), Normalize(lightpos));
    // We must now check if the flare is not occluded
#if CKVERSION == 0x13022002
    CKPICKRESULT res;
    CKObject *object = dev->Pick((int)light2dpos.x, (int)light2dpos.y, &res);
#else
    VxIntersectionDesc res;
    CKObject *object = dev->Pick((int)light2dpos.x, (int)light2dpos.y, &res);
#endif
    if (object)
    {
        VxVector v;
        cam->GetPosition(&v);
        // SquareMagnitude ???
        if (res.Distance < Magnitude(v - worldlightpos))
        {
            // the object is occluding the light
            if (!fc.FadeOutFlares(flares, flarescount))
                return;
        }
        else
        {
            fc.FadeInFlares(flares, flarescount, dp);
        }
    }
    else
    {
        fc.FadeInFlares(flares, flarescount, dp);
    }

    Vx2DVector sizemul(1.0f, 1.0f);
    beh->GetInputParameterValue(2, &sizemul);
    sizemul *= 0.5f;

    if (beh->GetVersion() == 0x00020000)
    {
        sizemul *= lookAt.z / 1.01f;
    }

    // we don't let write to the ZBuffer
    CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(1);
    float invtwidth = 1.0f;
    float invtheight = 1.0f;
    if (tex)
    {
        tex->SetAsCurrent(dev);
        invtwidth = 1.0f / tex->GetWidth();
        invtheight = 1.0f / tex->GetHeight();
    }
    else
        dev->SetTexture(NULL);

    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_WRAP0, 0);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
    dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ONE);
    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    if (tex && tex->GetMipmapCount())
    {
        dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEARMIPLINEAR);
        dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEARMIPLINEAR);
    }
    else
    {
        dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEAR);
        dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEAR);
    }
    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATEALPHA);
    dev->SetTextureStageState(CKRST_TSS_TEXCOORDINDEX, 0);
    dev->SetTextureStageState(CKRST_TSS_TEXTURETRANSFORMFLAGS, 0);

    float deltat = ctx->GetTimeManager()->GetLastDeltaTime();

    ///
    // Prepare the drawPrimitive
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * flarescount);
    int primcount = 6 * flarescount;
    CKWORD *IndicesPtr = dev->GetDrawPrimitiveIndices(primcount);
    CKWORD *indices = IndicesPtr;

    dev->SetWorldTransformationMatrix(cam->GetWorldMatrix());

    // TODO : changer les inverses transform par des multiplications de matrices (peut etre)
    VxVector pos, vec = lightpos - lookAt;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    XPtrStrided<VxUV> uvs(data->TexCoordPtr, data->TexCoordStride);
    XPtrStrided<VxVector4> positions(data->PositionPtr, data->PositionStride);
    XPtrStrided<CKDWORD> colors(data->ColorPtr, data->ColorStride);
#else
    XPtrStrided<VxUV> uvs(data->TexCoord.Ptr, data->TexCoord.Stride);
    XPtrStrided<VxVector4> positions(data->Positions.Ptr, data->Positions.Stride);
    XPtrStrided<CKDWORD> colors(data->Colors.Ptr, data->Colors.Stride);
#endif

    int index = 0;
    for (int i = 0; i < flarescount; ++i)
    {
        Flare *f = flares + i;

        ///
        // Appearing / Disappearing
        float alpha = 1.0f;
        switch (f->m_Mode)
        {
        case FLARE_VISIBLE:
            break;
        case FLARE_APPEARING:
            f->m_CurrentTime += deltat;
            if (f->m_CurrentTime > f->m_AppearingTime)
            {
                f->m_Mode = FLARE_VISIBLE;
            }
            else
                alpha = f->m_CurrentTime / f->m_AppearingTime;
            break;
        case FLARE_DISAPPEARING:
            f->m_CurrentTime -= deltat;
            if (f->m_CurrentTime < 0)
            {
                f->m_Mode = 0;
                alpha = 0.0f;
                primcount -= 6;
                continue;
            }
            else
                alpha = f->m_CurrentTime / f->m_DisappearingTime;
            break;
        default:
        {
            primcount -= 6;
            continue;
        }
        break;
        }

        ///
        // POSITIONS

        pos = lookAt + f->m_Position * vec;
        float hom = lookAt.z / lightpos.z;
        pos.x *= hom;
        pos.y *= hom;
        // TODO : Mode avec ou sans Scale du a la distance de la light... BOOL en input : pour le soleil par exemple...
        pos.z = lookAt.z;

        float hsx = f->m_Scale.x;
        float hsy = f->m_Scale.y;

        float cosa = cosf((pos.x + pos.y) * f->m_AngularSpeed);
        float sina = sinf((pos.x + pos.y) * f->m_AngularSpeed);

        Vx2DVector vhx(cosa, -sina);
        vhx *= f->m_Scale.x * sizemul.x;
        Vx2DVector vhy(sina, cosa);
        vhy *= f->m_Scale.y * sizemul.y;

        positions->x = pos.x - vhx.x + vhy.x;
        positions->y = pos.y - vhx.y + vhy.y;
        positions->z = pos.z;
        ++positions;
        positions->x = pos.x + vhx.x + vhy.x;
        positions->y = pos.y + vhx.y + vhy.y;
        positions->z = pos.z;
        ++positions;
        positions->x = pos.x + vhx.x - vhy.x;
        positions->y = pos.y + vhx.y - vhy.y;
        positions->z = pos.z;
        ++positions;
        positions->x = pos.x - vhx.x - vhy.x;
        positions->y = pos.y - vhx.y - vhy.y;
        positions->z = pos.z;
        ++positions;

        ///
        // COLORS

        // TODO : alpha depending on occlusion
        // Updating du temps
        float oldalpha = f->m_Color.a;
        f->m_Color.a *= alpha;
        CKDWORD col = RGBAFTOCOLOR(&(f->m_Color));
        f->m_Color.a = oldalpha;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxFillStructure(4, data->ColorPtr, data->ColorStride, 4, &col);
#else
        VxFillStructure(4, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif
        colors += 4;

        ///
        // UVS

        float left = 0;
        float right = 1.0f;
        float top = 0.0f;
        float bottom = 1.0f;
        if (!f->m_TextureOffset.IsNull())
        { // We have to calculate uvs
            left = f->m_TextureOffset.left * invtwidth;
            right = f->m_TextureOffset.right * invtwidth;
            top = f->m_TextureOffset.top * invtheight;
            bottom = f->m_TextureOffset.bottom * invtheight;
        }

        // Vertex 0
        uvs->u = left;
        uvs->v = top;
        ++uvs;
        // Vertex 1
        uvs->u = right;
        uvs->v = top;
        ++uvs;
        // Vertex 2
        uvs->u = right;
        uvs->v = bottom;
        ++uvs;
        // Vertex 3
        uvs->u = left;
        uvs->v = bottom;
        ++uvs;

        ///
        // Indices

        indices[0] = index;
        indices[1] = index + 1;
        indices[2] = index + 2;
        indices[3] = index;
        indices[4] = index + 2;
        indices[5] = index + 3;
        indices += 6;
        index += 4;
    }

    // the drawing itself
    if (primcount > 0)
        dev->DrawPrimitive(VX_TRIANGLELIST, IndicesPtr, primcount, data);
}
