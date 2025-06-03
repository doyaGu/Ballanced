/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AnimRecorder
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAnimRecorderDecl();
CKERROR CreateAnimRecorderProto(CKBehaviorPrototype **pproto);
int AnimRecorder(const CKBehaviorContext &behcontext);
CKERROR AnimRecorderCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAnimRecorderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Animation Recorder");
    od->SetDescription("Records the Position+Orientation+Scale as an animation.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>starts recording.<BR>
    <SPAN CLASS=in>Off: </SPAN>stops recording.<BR>
    <BR>
    <SPAN CLASS=out>Out On: </SPAN>activated after entering by ON.<BR>
    <SPAN CLASS=out>Out Off: </SPAN>activated after entering by OFF.<BR>
    <BR>
    <SPAN CLASS=pin>Object(s): </SPAN>object(s) which motion should be recorded.<BR>
    <SPAN CLASS=pin>Animation Name: </SPAN>name of the animation to be created (or used if it already exists).<BR>
    <SPAN CLASS=pin>Frame Step: </SPAN>step of frame to wait between each animation key creation.<BR>
    <BR>
    <SPAN CLASS=pout>Animation: </SPAN>recorded animation.<BR>
    <BR>
    <SPAN CLASS=setting>Use Grouped Animation: </SPAN>if FALSE, the building block records the motion of one single object (type of 'Object(s)' becomes 3dEntity, and output parameter becomes a Object Animation).<BR>
    if TRUE, the building block records the motion of all the object in the group (type of 'Object(s)' becomes group, and output parameter becomes a Global Animation).<BR>
    <BR>
    See Also: 'Play Animation 3DEntity', 'Play Global Animation', 'Set Animation Step on 3D entity'.<BR>
    */
    /* warning:
    - you shouldn't modify input parameters during recording time.<BR>
    - if an animation with the same name already exist, it will be replaced by the new one.<BR>
    */
    od->SetCategory("3D Transformations/Animation");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x532d07, 0x1db631cc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAnimRecorderProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateAnimRecorderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Animation Recorder");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out On");
    proto->DeclareOutput("Out Off");

    proto->DeclareInParameter("Object(s)", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Animation Name", CKPGUID_STRING, "Record Anim");
    proto->DeclareInParameter("Frame Step", CKPGUID_INT, "1");

    proto->DeclareOutParameter("Animation", CKPGUID_OBJECTANIMATION);

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // inner struct ANIMRECORDER
    proto->DeclareSetting("Use Grouped Animation", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AnimRecorder);
    proto->SetBehaviorCallbackFct((CKBEHAVIORCALLBACKFCT)AnimRecorderCallBackObject);

    *pproto = proto;
    return CK_OK;
}

//--- Inner struct
class A_ANIMRECORDER
{
public:
    CKObjectAnimation **anim;    // object anim array
    CKKeyedAnimation *keyedanim; // global anim
    float frameindex;            // index of the current frame
    int framestep;               // step (increment for FPS)
    int objcount;                // 1 if entity, N if group

    void SetObjAnim(int a, char *anim_name, CK3dEntity *ent, CKContext *ctx)
    {
        anim[a] = (CKObjectAnimation *)ctx->GetObjectByNameAndClass(anim_name, CKCID_OBJECTANIMATION, NULL);
        if (!anim[a])
        {
            anim[a] = (CKObjectAnimation *)ctx->CreateObject(CKCID_OBJECTANIMATION, anim_name);
            anim[a]->Set3dEntity(ent);
            CKLevel *level = ctx->GetCurrentLevel();
            level->AddObject(anim[a]);
        }
        else
        {
            anim[a]->Clear();
            anim[a]->Set3dEntity(ent);
        }
    }

    void SetKeyedAnim(char *anim_name, CKGroup *grp, CKContext *ctx)
    {
        if (!grp)
            return;
        keyedanim = (CKKeyedAnimation *)ctx->GetObjectByNameAndClass(anim_name, CKCID_KEYEDANIMATION, NULL);
        if (!keyedanim)
        {
            keyedanim = (CKKeyedAnimation *)ctx->CreateObject(CKCID_KEYEDANIMATION, anim_name);
            CKLevel *level = ctx->GetCurrentLevel();
            level->AddObject(keyedanim);
        }
        else
        {
            int count = keyedanim->GetAnimationCount();
            for (int a = 0; a < count; a++)
            {
                keyedanim->RemoveAnimation(keyedanim->GetAnimation(0));
            }
        }

        CK3dEntity *tmpent;
        char subanim_name[256];
        for (int a = 0; a < objcount; a++)
        {
            tmpent = (CK3dEntity *)grp->GetObject(a);
            if (tmpent)
            {
                sprintf(subanim_name, "%s_%s", tmpent->GetName(), anim_name);
                SetObjAnim(a, subanim_name, tmpent, ctx);
                keyedanim->AddAnimation(anim[a]);
            }
        }
    }
};
//---

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR AnimRecorderCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        A_ANIMRECORDER *ar = new A_ANIMRECORDER;
        ar->anim = NULL;
        ar->keyedanim = NULL;
        ar->objcount = 0;
        beh->SetLocalParameterValue(0, &ar, sizeof(ar));
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        if (!beh->GetOwner())
            break;
        A_ANIMRECORDER *ar = NULL;
        beh->GetLocalParameterValue(0, &ar);
        if (ar)
        {
            delete[] ar->anim;
            ar->anim = NULL;
            delete ar;
            ar = NULL;
            beh->SetLocalParameterValue(0, &ar, sizeof(ar));
        }
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL usegroup = FALSE;
        beh->GetLocalParameterValue(1, &usegroup);

        CKParameterIn *pin = beh->GetInputParameter(0);
        if (!pin)
            break;
        CKParameterOut *pout = beh->GetOutputParameter(0);
        if (!pout)
            break;

        if (usegroup)
        {
            if (pin->GetGUID() != CKPGUID_GROUP)
            {
                pin->SetGUID(CKPGUID_GROUP, TRUE);
                pout->SetGUID(CKPGUID_ANIMATION);
            }
        }
        else
        {
            if (pin->GetGUID() != CKPGUID_3DENTITY)
            {
                pin->SetGUID(CKPGUID_3DENTITY, TRUE);
                pout->SetGUID(CKPGUID_OBJECTANIMATION);
            }
        }
    }
    break;

    case CKM_BEHAVIORRESET:
    {
        A_ANIMRECORDER *ar = NULL;
        beh->GetLocalParameterValue(0, &ar);
        if (ar)
        {
            delete[] ar->anim;
            ar->anim = NULL;
        }
    }
    break;
    }

    return CKBR_OK;
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int AnimRecorder(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    A_ANIMRECORDER *ar = NULL; // inner struct
    beh->GetLocalParameterValue(0, &ar);
    if (!ar)
        return CKBR_OK;

    int usegroup = FALSE;
    beh->GetLocalParameterValue(1, &usegroup);

    CK3dEntity *tmpent;
    CKObject *o = beh->GetInputParameterObject(0);
    if (!o)
        return CKBR_OK;

    //--- Enter by ON
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        ar->objcount = usegroup ? ((CKGroup *)o)->GetObjectCount() : 1;
        ar->anim = new CKObjectAnimation *[ar->objcount];

        ar->frameindex = -1;
        ar->framestep = 0;

        char *anim_name = (char *)beh->GetInputParameterReadDataPtr(1);
        if (usegroup)
        {
            ar->SetKeyedAnim(anim_name, (CKGroup *)o, behcontext.Context);
        }
        else
        {
            ar->SetObjAnim(0, anim_name, (CK3dEntity *)o, behcontext.Context);
        }
    }

    //--- Enter by OFF
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        if (usegroup)
        {

            ar->keyedanim->SetLength(ar->frameindex);
            beh->SetOutputParameterObject(0, ar->keyedanim);
        }
        else
        {

            if (!ar->anim || !ar->anim[0])
                return CKBR_OK;
            ar->anim[0]->SetLength(ar->frameindex);
            beh->SetOutputParameterObject(0, ar->anim[0]);
        }

        delete[] ar->anim;
        ar->anim = NULL;

        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    //--- frame step
    int framestep = 1;
    beh->GetInputParameterValue(2, &framestep);

    ++ar->framestep;

    if (ar->framestep < framestep)
        return CKBR_ACTIVATENEXTFRAME;
    ar->framestep = 0;
    //---

    CKObjectAnimation *anim;

    VxQuaternion quat;
    VxVector pos, scale;

    //--- Increment Frame Index
    ++ar->frameindex;

    for (int a = 0; a < ar->objcount; a++)
    {
        tmpent = usegroup ? (CK3dEntity *)((CKGroup *)o)->GetObject(a) : (CK3dEntity *)o;
        const VxMatrix &matrix = tmpent->GetLocalMatrix();

        anim = ar->anim[a];

        Vx3DDecomposeMatrix(matrix, quat, pos, scale);
        anim->AddPositionKey(ar->frameindex, &pos);
        anim->AddScaleKey(ar->frameindex, &scale);
        anim->AddRotationKey(ar->frameindex, &quat);
    }

    return CKBR_ACTIVATENEXTFRAME;
}