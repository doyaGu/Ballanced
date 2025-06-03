/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            2d Picking
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKALL.h"

CKObjectDeclaration *FillBehaviorPicking2dDecl();
CKERROR CreatePicking2dProto(CKBehaviorPrototype **);
int Picking2d(const CKBehaviorContext &context);
CKERROR Picking2dCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPicking2dDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("2D Picking");
    od->SetDescription("Returns the 3D Entity, the normal and the exact position of the 2d mouse picking");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True : </SPAN>is activated when the ray intersects an object or a sprite.<BR>
    <SPAN CLASS=out>False : </SPAN>is activated when the ray doesn't intersect an object.<BR>
    <SPAN CLASS=pin>Window Relative : </SPAN>Check this parameter if you give coordinates in the render window coordinate system,
    or leave it uncheck if you give them in the entire screen coordinates (like the values returned by the
    GetMousePosition behavior). For fullscreen mode, this parameter makes no difference.
    <BR>
    <SPAN CLASS=setting>Object intersected : </SPAN>The first object in Z order, under the picking.<BR>
    <SPAN CLASS=setting>Intersection Point : </SPAN>exact 3d point coordinates of the intersection point (this point is given in world space coordinates).<BR>
    <SPAN CLASS=setting>Intersection Normal : </SPAN>coordinates of the normal vector on the picked object.<BR>
    <SPAN CLASS=setting>UV Coordinates : </SPAN>coordinates on the 'texel' picked (pixel on the texture).<BR>
    <SPAN CLASS=setting>Distance from The Viewpoint : </SPAN>distance of the exact 3d point from the viewpoint<BR>
    <SPAN CLASS=setting>Face Index : </SPAN>index, on the object, of the picked face<BR>
    <SPAN CLASS=setting>Sprite: </SPAN>picked sprite, if any.<BR>
    */
    od->SetCategory("Interface/Screen");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1616120f, 0x5b2f48ce));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreatePicking2dProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreatePicking2dProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("2D Picking");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Pos", CKPGUID_2DVECTOR, "0");
    proto->DeclareInParameter("Absolute Coordinates", CKPGUID_BOOL, "FALSE");

    proto->DeclareSetting("Use 2D Vector?", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Use Mouse Coordinates?", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Object Picked", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Intersection Point", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Intersection Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("UV Coordinates", CKPGUID_2DVECTOR);
    proto->DeclareOutParameter("Distance from The Viewpoint", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Face Index", CKPGUID_INT);
    proto->DeclareOutParameter("Sprite", CKPGUID_SPRITE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(Picking2d);
    proto->SetBehaviorCallbackFct(Picking2dCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int Picking2d(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    Vx2DVector point;

    CKBOOL mouse = FALSE;
    beh->GetLocalParameterValue(1, &mouse);

    CKRenderContext *rend = ctx->GetPlayerRenderContext();

    if (mouse)
    {
        CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
        if (!input)
            return CKBR_GENERICERROR;

        input->GetMousePosition(point, TRUE);

        rend->ScreenToClient(&point);
    }
    else
    {
        CKBOOL use2dvector = FALSE;
        beh->GetLocalParameterValue(0, &use2dvector);

        CKBOOL wr = FALSE;
        if (use2dvector)
        {
            beh->GetInputParameterValue(0, &point);
            beh->GetInputParameterValue(1, &wr);
        }
        else
        {
            int x, y;
            beh->GetInputParameterValue(0, &x);
            beh->GetInputParameterValue(1, &y);
            point.Set(x, y);
            beh->GetInputParameterValue(2, &wr);
        }

        if (!wr)
        {
            rend->ScreenToClient(&point);
        }
    }

#if CKVERSION == 0x13022002
    CKPICKRESULT res;
    CK3dEntity *ent = (CK3dEntity *)(rend->Pick(point.x, point.y, &res));
#else
    VxIntersectionDesc res;
    CK3dEntity *ent = (CK3dEntity *)(rend->Pick(point.x, point.y, &res));
#endif
    if (ent || res.Sprite)
    {
        beh->SetOutputParameterObject(0, ent);

        if (ent)
        {
            VxVector p;
            ent->Transform(&p, &(res.IntersectionPoint));
            beh->SetOutputParameterValue(1, &p);

            VxVector n;
            ent->TransformVector(&n, &(res.IntersectionNormal));
            beh->SetOutputParameterValue(2, &n);

            Vx2DVector tex;
            tex.x = res.TexU;
            tex.y = res.TexV;
            beh->SetOutputParameterValue(3, &tex);

            beh->SetOutputParameterValue(4, &(res.Distance));

            beh->SetOutputParameterValue(5, &(res.FaceIndex));
        }

        beh->SetOutputParameterValue(6, &res.Sprite);

        beh->ActivateOutput(0);
    }
    else
    {
        beh->SetOutputParameterObject(0, ent); // Clearing purpose

        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR Picking2dCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    {
        //--- if version < 2 create setting
        if (beh->GetVersion() < 0x00020000)
        {

            CKParameterLocal *p;

            p = beh->CreateLocalParameter("Use 2d Vector ?", CKPGUID_BOOL);
            CKBOOL k = FALSE;
            p->SetValue(&k);
        }
    }
    break;

    case CKM_BEHAVIORCREATE:
    {
        CKBOOL mouse = FALSE;
        beh->GetLocalParameterValue(1, &mouse);

        if (mouse)
        {
            beh->EnableInputParameter(0, FALSE);
            beh->EnableInputParameter(1, FALSE);
            beh->EnableInputParameter(2, FALSE);
        }
        else
        {
            CKBOOL use2dvector = FALSE;
            beh->GetLocalParameterValue(0, &use2dvector);

            beh->EnableInputParameter(0, TRUE);
            beh->EnableInputParameter(1, TRUE);
            beh->EnableInputParameter(2, TRUE);
        }
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL mouse = FALSE;
        beh->GetLocalParameterValue(1, &mouse);

        if (mouse)
        {
            beh->EnableInputParameter(0, FALSE);
            beh->EnableInputParameter(1, FALSE);
            beh->EnableInputParameter(2, FALSE);
        }
        else
        {
            CKBOOL use2dvector = FALSE;
            beh->GetLocalParameterValue(0, &use2dvector);

            beh->EnableInputParameter(0, TRUE);
            beh->EnableInputParameter(1, TRUE);
            beh->EnableInputParameter(2, TRUE);

            CKParameterIn *pin = beh->GetInputParameter(0);
            if (!pin)
                return CKBR_OK;

            if (use2dvector) // if we check 2dvector
            {
                if (pin->GetGUID() == CKPGUID_INT)
                {
                    CKDestroyObject(beh->RemoveInputParameter(0));
                    CKDestroyObject(beh->RemoveInputParameter(0));
                    CKDestroyObject(beh->RemoveInputParameter(0));

                    beh->CreateInputParameter("Pos", CKPGUID_2DVECTOR);
                    beh->CreateInputParameter("Window Relative", CKPGUID_BOOL);
                }
            }
            else // if we do not check 2dvector
            {
                if (pin->GetGUID() != CKPGUID_INT)
                {
                    CKDestroyObject(beh->RemoveInputParameter(0));
                    CKDestroyObject(beh->RemoveInputParameter(0));

                    beh->CreateInputParameter("X", CKPGUID_INT);
                    beh->CreateInputParameter("Y", CKPGUID_INT);
                    beh->CreateInputParameter("Window Relative", CKPGUID_BOOL);
                }
            }
        }
    }
    break;
    }

    return CK_OK;
}
