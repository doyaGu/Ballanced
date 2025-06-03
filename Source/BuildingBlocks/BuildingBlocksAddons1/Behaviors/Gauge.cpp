/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Gauge2
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define SIZEMODE_CUT        0
#define SIZEMODE_STRETCH    1

#define IN_ON               0
#define IN_OFF              1
#define IN_PAUSE            2

#define OUT_EXITON          0
#define OUT_EXITOFF         1
#define OUT_EXITPAUSE       2

#define PIN_PROGRESSION     0
#define PIN_ORIENTATION     1
#define PIN_STRETCH         2

// Version 1
#define PLOCAL_RECT         0
#define PLOCAL_TARGET_V1    1

// Version2
#define PLOCAL_RECT         0
#define PLOCAL_ON           1
#define PLOCAL_PAUSED       2
#define PLOCAL_TARGET_V2    3

CKObjectDeclaration *FillBehaviorGaugeDecl();
CKERROR CreateGaugeProto(CKBehaviorPrototype **pproto);
int Gauge(const CKBehaviorContext &behcontext);
CKERROR GaugeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGaugeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Gauge");
    od->SetDescription("Displays a progression bar.");

    /* rem:
    <SPAN CLASS=in>On : </SPAN>Activates the Progression Bar.<BR>
    <SPAN CLASS=in>Off : </SPAN>Deactivates the Progression Bar.<BR>
    <SPAN CLASS=in>Pause/Resume : </SPAN>Pause/Resume the Progression Bar.<BR>
    <BR>
    <SPAN CLASS=out>Exit On : </SPAN>This output is activated after entering by 'On'.<BR>
    <SPAN CLASS=out>Exit Off : </SPAN>This output is activated after entering by 'Off'.<BR>
    <SPAN CLASS=out>Exit Off : </SPAN>This output is activated after entering by 'Pause/Resume'.<BR>
    <BR>
    <SPAN CLASS=setting>Size Mode : </SPAN> - <BR>
    <SPAN CLASS=setting>Orientation : </SPAN> - <BR>
    <BR>
    */
    /* Note:
    Activate the "on" entry each time the target of the behavior is modified.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6a777fc4, 0x1dc17b23));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateGaugeProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Visuals/2D");
    return od;
}

CKERROR CreateGaugeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Gauge");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Pause/Resume");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Exit Pause/Resume");

    proto->DeclareInParameter("Bar Progression", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Orientation", CKPGUID_SPRITETEXTALIGNMENT, "4");
    proto->DeclareInParameter("Stretch", CKPGUID_BOOL, "TRUE");

    proto->DeclareLocalParameter("", CKPGUID_RECT);
    proto->DeclareLocalParameter("", CKPGUID_BOOL, FALSE);
    proto->DeclareLocalParameter("", CKPGUID_BOOL, FALSE);

    proto->SetFunction(Gauge);
    proto->SetBehaviorCallbackFct(GaugeCallBack);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

void ResetTarget(CKBehavior *beh, CK2dEntity *e)
{
    if (CKIsChildClassOf(e, CKCID_2DENTITY))
    {
        // restore initial condition if object has initial conditions
        CKScene *scene = beh->GetCKContext()->GetCurrentScene();
        if (e->IsInScene(scene))
        {
            CKStateChunk *chunk = scene->GetObjectInitialValue(e);
            if (chunk)
            {
                e->Load(chunk, NULL);
                return;
            }
        }

        VxRect rect;
        beh->GetLocalParameterValue(PLOCAL_RECT, &rect);

        // rect has not been initialized, break
        if (rect.left + rect.right + rect.top + rect.bottom == 0)
            return;

        e->UseSourceRect(FALSE);
        if (e->IsHomogeneousCoordinates())
            e->SetHomogeneousRect(rect);
        else
            e->SetRect(rect);
        if (!CKIsChildClassOf(e, CKCID_SPRITE))
        {
            VxRect r;
            r.left = r.top = 0;
            r.right = r.bottom = 1;
            e->SetSourceRect(r);
        }
    }
}

int Gauge(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();

    int version = beh->GetVersion();
    if (version == 0x00020000)
    {
        CKBOOL isOn = FALSE;
        CKBOOL isPaused = FALSE;
        beh->GetLocalParameterValue(PLOCAL_ON, &isOn);
        beh->GetLocalParameterValue(PLOCAL_PAUSED, &isPaused);

        // OFF
        if (beh->IsInputActive(IN_OFF))
        {
            beh->ActivateInput(IN_OFF, FALSE);
            beh->ActivateOutput(OUT_EXITOFF);

            if (isOn)
            {
                // restore the initial rect of the 2D Frame
                CK2dEntity *e = (CK2dEntity *)beh->GetTarget();
                ResetTarget(beh, e);

                isOn = FALSE;
                beh->SetLocalParameterValue(PLOCAL_ON, &isOn);
                isPaused = FALSE;
                beh->SetLocalParameterValue(PLOCAL_PAUSED, &isPaused);
            }
        }

        // ON
        else if (beh->IsInputActive(IN_ON))
        {
            beh->ActivateOutput(OUT_EXITON);
            beh->ActivateInput(IN_ON, FALSE);

            if (!isOn)
            {
                // store the current rect of the 2D Frame as the 100% size
                CK2dEntity *e = (CK2dEntity *)beh->GetTarget();
                if (CKIsChildClassOf(e, CKCID_2DENTITY))
                {
                    VxRect rect;
                    if (e->IsHomogeneousCoordinates())
                        e->GetHomogeneousRect(rect);
                    else
                        e->GetRect(rect);
                    beh->SetLocalParameterValue(PLOCAL_RECT, &rect);
                }

                isOn = TRUE;
                beh->SetLocalParameterValue(PLOCAL_ON, &isOn);
                isPaused = FALSE;
                beh->SetLocalParameterValue(PLOCAL_PAUSED, &isPaused);
            }
        }

        // PAUSE
        else if (beh->IsInputActive(IN_PAUSE))
        {
            beh->ActivateInput(IN_PAUSE, FALSE);
            beh->ActivateOutput(OUT_EXITPAUSE);

            if (isOn)
            {
                isPaused = !isPaused;
                beh->SetLocalParameterValue(PLOCAL_PAUSED, &isPaused);
            }
        }

        if (!isOn || isPaused)
            return CKBR_OK;
    }
    else
    { // version 0x00010000

        // OFF
        if (beh->IsInputActive(IN_OFF))
        {
            beh->ActivateInput(IN_OFF, FALSE);
            beh->ActivateOutput(OUT_EXITOFF);
            return CKBR_OK;
        }

        // ON
        else if (beh->IsInputActive(IN_ON))
        {
            beh->ActivateOutput(OUT_EXITON);
            beh->ActivateInput(IN_ON, FALSE);
        }
    }

    if (ent)
    {
        if (ent->IsVisible())
        {
            VxRect rect, srcRect, destRect;

            // get pins values
            beh->GetLocalParameterValue(PLOCAL_RECT, &rect);
            float p = 0;
            beh->GetInputParameterValue(PIN_PROGRESSION, &p);
            int orientation = 0;
            beh->GetInputParameterValue(PIN_ORIENTATION, &orientation);
            CKBOOL stretch = TRUE;
            beh->GetInputParameterValue(PIN_STRETCH, &stretch);

            // get entity size
            float w = rect.GetWidth();
            float h = rect.GetHeight();

            // take orientation into account
            destRect = rect;
            float x = 1.0f;
            float y = 1.0f;
            switch (orientation)
            {
            case CKSPRITETEXT_RIGHT:
                x = p;
                break;
            case CKSPRITETEXT_BOTTOM:
                y = p;
                break;
            case CKSPRITETEXT_LEFT:
                x = -p;
                break;
            case CKSPRITETEXT_TOP:
                y = -p;
                break;
            case CKSPRITETEXT_CENTER:
                x = p;
                y = p;
                destRect.left = destRect.left + ((1 - x) * w / 2);
                destRect.top = destRect.top + ((1 - y) * h / 2);
                break;
            case CKSPRITETEXT_VCENTER:
                y = p;
                destRect.top = destRect.top + ((1 - y) * h / 2);
                break;
            case CKSPRITETEXT_HCENTER:
                x = p;
                destRect.left = destRect.left + ((1 - x) * w / 2);
                break;
            }

            // update dest rect
            destRect.right = destRect.left + w * x;
            destRect.bottom = destRect.top + h * y;

            if (destRect.right < destRect.left)
            {
                float tmp = destRect.left;
                destRect.left = destRect.right;
                destRect.right = tmp;
            }
            if (destRect.bottom < destRect.top)
            {
                float tmp = destRect.bottom;
                destRect.bottom = destRect.top;
                destRect.top = tmp;
            }

            // set source rect
            if (!stretch)
            {
                float img_w = 0;
                float img_h = 0;
                if (CKIsChildClassOf(ent, CKCID_SPRITE))
                {
                    img_w = (float)((CKSprite *)ent)->GetWidth();
                    img_h = (float)((CKSprite *)ent)->GetHeight();
                }
                else
                {
                    img_w = 1.0f;
                    img_h = 1.0f;
                    /*
                    CKMaterial * mat = ent->GetMaterial();
                    if (mat)
                    {
                        CKTexture * tex = mat->GetTexture();
                        if (tex)
                        {
                            img_w=(float)tex->GetWidth();
                            img_h=(float)tex->GetHeight();
                        }
                    }
                    */
                }
                srcRect.left = 0;
                srcRect.top = 0;
                if (orientation & CKSPRITETEXT_VCENTER)
                    srcRect.top = srcRect.top + ((1 - y) * img_h * 0.5f);
                if (orientation & CKSPRITETEXT_HCENTER)
                    srcRect.left = srcRect.left + ((1 - x) * img_w * 0.5f);
                if (orientation & CKSPRITETEXT_CENTER)
                {
                    srcRect.left = srcRect.left + ((1 - x) * img_w * 0.5f);
                    srcRect.top = srcRect.top + ((1 - y) * img_h * 0.5f);
                }

                srcRect.right = srcRect.left + img_w * (float)fabs(x);
                srcRect.bottom = srcRect.top + img_h * (float)fabs(y);

                /*
                if (!CKIsChildClassOf(ent,CKCID_SPRITE))
                {
                    srcRect.left=((1-x)*0.5f);
                    srcRect.top=((1-y)*0.5f);
                    srcRect.right=srcRect.left+x;
                    srcRect.bottom=srcRect.top+y;
                }
                */

                ent->UseSourceRect(TRUE);
                ent->SetSourceRect(srcRect);
            }
            else
                ent->UseSourceRect(FALSE);

            // set dest rect
            if (ent->IsHomogeneousCoordinates())
                ent->SetHomogeneousRect(destRect);
            else
                ent->SetRect(destRect);
        }

        return CKBR_ACTIVATENEXTFRAME;
    }
    return CKBR_OK; // error, no target
}

CKERROR GaugeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (beh->GetVersion() == 0x00010000)
    {

        switch (behcontext.CallbackMessage)
        {
        case CKM_BEHAVIOREDITED:
        {
            CK2dEntity *oldTarget = NULL;
            CK2dEntity *target = (CK2dEntity *)beh->GetTarget();

            if (beh->GetTargetParameter())
            {
                // target parameter added
                if (beh->GetLocalParameterCount() == 1)
                {
                    if (CKIsChildClassOf(beh->GetOwner(), CKCID_2DENTITY))
                        oldTarget = (CK2dEntity *)beh->GetOwner();
                    // create local that will memorize target
                    // resetTarget=TRUE;
                    CKParameterLocal *local = beh->CreateLocalParameter("", CKPGUID_2DENTITY);
                    if (local)
                        beh->SetLocalParameterValue(PLOCAL_TARGET_V1, &target);
                }
                // target already exist, check with local that store old target
                else if (beh->GetLocalParameterCount() == 2)
                {
                    beh->GetLocalParameterValue(PLOCAL_TARGET_V1, &oldTarget);
                    if (oldTarget != target) // target change, update local storing target
                        //(available only when target parameter exists)
                        beh->SetLocalParameterValue(PLOCAL_TARGET_V1, &target);
                }
            }
            else
            {
                // target parameter removed
                if (beh->GetLocalParameterCount() == 2)
                {
                    beh->GetLocalParameterValue(PLOCAL_TARGET_V1, &oldTarget);

                    // remove local parameter
                    CKParameterLocal *local = beh->RemoveLocalParameter(PLOCAL_TARGET_V1);
                    if (local)
                        behcontext.Context->DestroyObject(local->GetID(), CK_DESTROY_NONOTIFY);
                }
                // same target as before
                else
                    oldTarget = target;
            }
            if (oldTarget == target)
                break;
            ResetTarget(beh, oldTarget);
            // then continue as an attach / load
        }
        case CKM_BEHAVIORATTACH:
        {
            CK2dEntity *e = (CK2dEntity *)beh->GetTarget();
            if (CKIsChildClassOf(e, CKCID_2DENTITY))
            {
                VxRect rect;
                if (e->IsHomogeneousCoordinates())
                    e->GetHomogeneousRect(rect);
                else
                    e->GetRect(rect);
                beh->SetLocalParameterValue(PLOCAL_RECT, &rect);
            }
        }
        break;
        case CKM_BEHAVIORLOAD:
        case CKM_BEHAVIORRESET:
        case CKM_BEHAVIORDEACTIVATESCRIPT:
        case CKM_BEHAVIORDETACH:
        {
            if (behcontext.Context->IsInClearAll()) // do not restore target state if in clear all
                break;                              // or worse restore IC, since it will crash later because the entity
            // would be reattached to 2droot and crash at the next render
            CK2dEntity *e = (CK2dEntity *)beh->GetTarget();
            ResetTarget(beh, e);
        }
        break;
        case CKM_BEHAVIORPAUSE:
        case CKM_BEHAVIORSETTINGSEDITED:
        case CKM_BEHAVIORRESUME:
        case CKM_BEHAVIORACTIVATESCRIPT:
        case CKM_BEHAVIORNEWSCENE:
            break;
        }
        return CKBR_OK;
    }
    else // beh->GetVersion() == 0x00020000
    {
        CKBOOL isOn = FALSE;
        CKBOOL isPaused = FALSE;
        beh->GetLocalParameterValue(PLOCAL_ON, &isOn);
        beh->GetLocalParameterValue(PLOCAL_PAUSED, &isPaused);

        switch (behcontext.CallbackMessage)
        {
        case CKM_BEHAVIOREDITED:
        {
            CK2dEntity *oldTarget = NULL;
            CK2dEntity *target = (CK2dEntity *)beh->GetTarget();

            if (beh->GetTargetParameter())
            {
                // target parameter added
                if (beh->GetLocalParameterCount() == 3)
                {
                    if (CKIsChildClassOf(beh->GetOwner(), CKCID_2DENTITY))
                        oldTarget = (CK2dEntity *)beh->GetOwner();
                    // create local that will memorize target
                    // resetTarget=TRUE;
                    CKParameterLocal *local = beh->CreateLocalParameter("", CKPGUID_2DENTITY);
                    if (local)
                        beh->SetLocalParameterValue(PLOCAL_TARGET_V2, &target);
                }
                // target already exist, check with local that store old target
                else if (beh->GetLocalParameterCount() == 4)
                {
                    beh->GetLocalParameterValue(PLOCAL_TARGET_V2, &oldTarget);
                    if (oldTarget != target) // target change, update local storing target
                        //(available only when target parameter exists)
                        beh->SetLocalParameterValue(PLOCAL_TARGET_V2, &target);
                }
            }
            else
            {
                // target parameter removed
                if (beh->GetLocalParameterCount() == 4)
                {
                    beh->GetLocalParameterValue(PLOCAL_TARGET_V2, &oldTarget);

                    // remove local parameter
                    CKParameterLocal *local = beh->RemoveLocalParameter(PLOCAL_TARGET_V2);
                    if (local)
                        behcontext.Context->DestroyObject(local->GetID(), CK_DESTROY_NONOTIFY);
                }
                // same target as before
                else
                    oldTarget = target;
            }
            if (oldTarget == target)
                break;

            if (isOn)
            {
                ResetTarget(beh, oldTarget);

                if (CKIsChildClassOf(target, CKCID_2DENTITY))
                {
                    VxRect rect;
                    if (target->IsHomogeneousCoordinates())
                        target->GetHomogeneousRect(rect);
                    else
                        target->GetRect(rect);
                    beh->SetLocalParameterValue(PLOCAL_RECT, &rect);
                }
            }
            break;
        }
        case CKM_BEHAVIORATTACH:
        case CKM_BEHAVIORLOAD:
        {
            isOn = FALSE;
            beh->SetLocalParameterValue(PLOCAL_ON, &isOn);
            isPaused = FALSE;
            beh->SetLocalParameterValue(PLOCAL_PAUSED, &isPaused);
        }
        break;
        case CKM_BEHAVIORRESET:
        case CKM_BEHAVIORDEACTIVATESCRIPT:
        case CKM_BEHAVIORDETACH:
        {
            if (isOn)
            {
                if (behcontext.Context->IsInClearAll()) // do not restore target state if in clear all
                    break;                              // or worse restore IC, since it will crash later because the entity
                // would be reattached to 2droot and crash at the next render
                CK2dEntity *e = (CK2dEntity *)beh->GetTarget();
                ResetTarget(beh, e);
                isOn = FALSE;
                beh->SetLocalParameterValue(PLOCAL_ON, &isOn);
                isPaused = FALSE;
                beh->SetLocalParameterValue(PLOCAL_PAUSED, &isPaused);
            }
        }
        break;
        case CKM_BEHAVIORPAUSE:
        case CKM_BEHAVIORSETTINGSEDITED:
        case CKM_BEHAVIORRESUME:
        case CKM_BEHAVIORACTIVATESCRIPT:
        case CKM_BEHAVIORNEWSCENE:
            break;
        }
        return CKBR_OK;
    }
}
