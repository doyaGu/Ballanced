/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              TT Extra
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorExtraDecl();
CKERROR CreateExtraProto(CKBehaviorPrototype **);
int Extra(const CKBehaviorContext &behcontext);
CKERROR ExtraCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorExtraDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Extra");
    od->SetDescription("Manages the Powerballextras.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x36106BD9, 0x51813906));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExtraProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateExtraProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Extra");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Initialize");

    proto->DeclareOutput("Activated");
    proto->DeclareOutput("Hit");
    proto->DeclareOutput("Ready");

    proto->DeclareInParameter("Ball", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Number of Smallballs", CKPGUID_INT, "6");
    proto->DeclareInParameter("Activationdistance", CKPGUID_FLOAT, "2");
    proto->DeclareInParameter("Extra_Points CollDistance", CKPGUID_FLOAT, "2");
    proto->DeclareInParameter("Rotationspeed", CKPGUID_FLOAT, "5");
    proto->DeclareInParameter("Awayforce", CKPGUID_FLOAT, "50");
    proto->DeclareInParameter("Awaydamping", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Force", CKPGUID_FLOAT, "0.06");
    proto->DeclareInParameter("Damping", CKPGUID_FLOAT, "0.9");
    proto->DeclareInParameter("Forcewidth", CKPGUID_FLOAT, "0.06");
    proto->DeclareInParameter("Flyawaytime", CKPGUID_FLOAT, "1000.0");
    proto->DeclareInParameter("Hitframegroup", CKPGUID_GROUP);
    proto->DeclareInParameter("Flying Extra?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Exactness Framedelay", CKPGUID_INT, "2");
    proto->DeclareInParameter("Current Hits", CKPGUID_INT, "0");

    proto->DeclareLocalParameter("Activationstatus", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("Smallballlist", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Shadowobject", CKPGUID_3DENTITY);
    proto->DeclareLocalParameter("Centerobject", CKPGUID_3DENTITY);
    proto->DeclareLocalParameter("Status", CKPGUID_INT, "-1");
    proto->DeclareLocalParameter("Initialized", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("Timecounter", CKPGUID_FLOAT, "0");
    proto->DeclareLocalParameter("Ballcounter", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("Timevalue", CKPGUID_FLOAT, "0");
    proto->DeclareLocalParameter("Next Check", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("Away Positionsave", CKPGUID_VECTOR);
    proto->DeclareSetting("Exactness?", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Extra);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(ExtraCallBack);

    *pproto = proto;
    return CK_OK;
}

#define BALL 0
#define NUM_SMALLBALLS 1
#define ACTIVATION_DISTANCE 2
#define EXTRA_POINTS_COLLDISTANCE 3
#define ROTATION_SPEED 4
#define AWAY_FORCE 5
#define AWAY_DAMPING 6
#define FORCE 7
#define DAMPING 8
#define FORCE_WIDTH 9
#define FLYAWAY_TIME 10
#define HIT_FRAME_GROUP 11
#define FLYING_EXTRA 12
#define EXACTNESS_FRAME_DELAY 13
#define CURRENT_HITS 14

#define ACTIVATION_STATUS 0
#define SMALLBALL_LIST 1
#define SHADOW_OBJECT 2
#define CENTER_OBJECT 3
#define STATUS 4
#define INITIALIZED 5
#define TIME_COUNTER 6
#define BALL_COUNTER 7
#define TIME_VALUE 8
#define NEXT_CHECK 9
#define AWAY_POSITION_SAVE 10
#define EXACTNESS 11

struct Smallball
{
    Smallball *next;
    CK3dEntity *ball;
    VxVector position;
    float force;
    bool flag;

    Smallball(Smallball *node, CK3dEntity *b, const VxVector &pos, float f, bool init)
        : next(node), ball(b), position(pos), force(f), flag(init) {}
};

typedef Smallball *SmallballList;

const char *indices[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

CK3dEntity *GetChildOfTarget(int index, const char *name, CK3dEntity *target)
{
    if (!target)
    {
        return NULL;
    }

    int childrenCount = target->GetChildrenCount();
    if (childrenCount <= 0)
    {
        return NULL;
    }

    int i;
    CK3dEntity *child;
    for (i = 0; i < childrenCount; ++i)
    {
        child = target->GetChild(i);

        CKSTRING childName = child->GetName();
        int nameSize = strlen(name) + 1;
        CKSTRING str = new char[nameSize - 1];
        for (int s = 0; s < nameSize; ++s)
        {
            str[s] = childName[index + s];
        }
        str[nameSize - 1] = '\0';

        if (strcmp(name, str) == 0)
        {
            delete[] str;
            break;
        }
        delete[] str;
    }

    if (i >= childrenCount)
    {
        return NULL;
    }

    return child;
}

int sub_10001F10(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    if (smallballs)
    {
        CKScene *scene = context->GetCurrentScene();
        if (!scene)
        {
            return CKBR_PARAMETERERROR;
        }

        CK3dEntity *child;
        CKBehavior *script;

        Smallball *current = smallballs;
        Smallball *next = NULL;
        do
        {
            child = current->ball->GetChild(0);
            script = child->GetScript(0);
            if (!script)
            {
                context->OutputToConsoleEx("4Script not found %s", child->GetName());
            }
            scene->DeActivate(script);

            child = current->ball->GetChild(0);
            script = child->GetScript(0);
            if (!script)
            {
                context->OutputToConsoleEx("5Script not found %s", child->GetName());
            }
            scene->DeActivate(script);
            current->ball->Show(CKHIDE);

            next = current->next;
            delete current;
            current = next;
        } while (current);

        beh->SetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    }

    return CKBR_OK;
}

int sub_10001EC0(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (sub_10001F10(behcontext) != CKBR_OK)
    {
        return CKBR_PARAMETERERROR;
    }

    BOOL initialized = FALSE;
    beh->SetLocalParameterValue(INITIALIZED, &initialized);

    int status = -1;
    beh->SetLocalParameterValue(STATUS, &status);

    return CKBR_OK;
}

int InitializeExtra(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKScene *scene = context->GetCurrentScene();
    if (!scene)
    {
        return CKBR_PARAMETERERROR;
    }

    int status;
    beh->GetLocalParameterValue(STATUS, &status);

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);

    if (status == -1)
    {
        if (smallballs)
        {
            smallballs = NULL;
        }
    }
    else if (status == 1)
    {
        if (sub_10001EC0(behcontext) != CKBR_OK)
        {
            return CKBR_PARAMETERERROR;
        }
    }

    if (smallballs)
    {
        return CKBR_OK;
    }

    int smallballCount;
    beh->GetInputParameterValue(NUM_SMALLBALLS, &smallballCount);
    if (smallballCount <= 0)
    {
        smallballCount = 1;
    }

    beh->SetLocalParameterValue(BALL_COUNTER, &smallballCount);

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
    {
        return CKBR_OWNERERROR;
    }

    BOOL flyingExtra;
    beh->GetInputParameterValue(FLYING_EXTRA, &flyingExtra);

    CK3dEntity *shadowObject = GetChildOfTarget(14, "Floor", target);
    CK3dEntity *centerObject = GetChildOfTarget(14, "Ball0", target);

    if (!shadowObject && !flyingExtra)
    {
        context->OutputToConsole("There is no Floorglowobject in hirachy!", FALSE);
        return CKBR_PARAMETERERROR;
    }

    if (!centerObject)
    {
        context->OutputToConsole("There is no Centerbillboard in hirachy!", FALSE);
        return CKBR_PARAMETERERROR;
    }

    beh->SetLocalParameterObject(SHADOW_OBJECT, shadowObject);
    beh->SetLocalParameterObject(CENTER_OBJECT, centerObject);

    if (shadowObject)
    {
        scene->DeActivate(shadowObject);
        shadowObject->Show(CKHIDE);
    }

    CK3dEntity *child = centerObject->GetChild(0);
    CKBehavior *script = child->GetScript(0);
    if (!script)
    {
        context->OutputToConsoleEx("1Script not found %s", child->GetName());
    }
    scene->DeActivate(script);
    centerObject->Show(CKHIDE);

    float force;
    float forceWidth;
    beh->GetInputParameterValue(FORCE, &force);
    beh->GetInputParameterValue(FORCE_WIDTH, &forceWidth);
    forceWidth /= smallballCount;

    Smallball *prev = NULL;
    for (int i = 0; i < smallballCount; ++i)
    {
        CK3dEntity *ball = GetChildOfTarget(18, indices[i], target);
        if (!ball)
        {
            context->OutputToConsole("Missing one or more Billboards.", FALSE);
            return CKBR_PARAMETERERROR;
        }

        child = ball->GetChild(0);
        script = child->GetScript(0);
        if (!script)
        {
            context->OutputToConsoleEx("2Script not found %s", child->GetName());
        }
        scene->DeActivate(script);
        ball->Show(CKHIDE);

        VxVector childPos;
        ball->GetPosition(&childPos);

        Smallball *current = new Smallball(NULL, ball, childPos, force, true);
        if (i == 0)
        {
            smallballs = current;
        }

        child = ball->GetChild(0);
        if (!child)
        {
            context->OutputToConsole("There is no Frame at one of the Billboards.", FALSE);
            return CKBR_PARAMETERERROR;
        }

        if (!child->GetScript(0))
        {
            context->OutputToConsole("There is no Script at one of the Billboardframes.", FALSE);
            return CKBR_PARAMETERERROR;
        }

        script = child->GetScript(0);
        if (!script)
        {
            context->OutputToConsoleEx("2Script not found %s", child->GetName());
        }
        script->Activate(FALSE);

        if (prev)
        {
            prev->next = current;
        }
        prev = current;
    }

    beh->SetLocalParameterValue(SMALLBALL_LIST, &smallballs, sizeof(SmallballList));

    float flyawayTime;
    beh->GetInputParameterValue(FLYAWAY_TIME, &flyawayTime);
    beh->SetLocalParameterValue(TIME_COUNTER, &flyawayTime);

    BOOL initialized = TRUE;
    beh->SetLocalParameterValue(INITIALIZED, &initialized);

    int status = 0;
    beh->SetLocalParameterValue(STATUS, &status);

    return CKBR_OK;
}

int ActivateExtra(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    if (!smallballs)
    {
        return CKBR_PARAMETERERROR;
    }

    CKScene *scene = context->GetCurrentScene();
    if (!scene)
    {
        return CKBR_PARAMETERERROR;
    }

    BOOL flyingExtra;
    beh->GetInputParameterValue(FLYING_EXTRA, &flyingExtra);
    CK3dEntity *shadowObject = (CK3dEntity *)beh->GetLocalParameterObject(SHADOW_OBJECT);
    CK3dEntity *centerObject = (CK3dEntity *)beh->GetLocalParameterObject(CENTER_OBJECT);

    if (centerObject)
    {
        centerObject->Show();
    }

    if (shadowObject && !flyingExtra)
    {
        shadowObject->Show();
    }

    CK3dEntity *child;
    CKBehavior *script;
    for (Smallball *it = smallballs; it != NULL; it = it->next)
    {
        child = it->ball->GetChild(0);
        script = child->GetScript(0);
        if (!script)
        {
            context->OutputToConsoleEx("8Script not found %s", child->GetName());
        }
        scene->Activate(script, TRUE);
        it->ball->Show();

        child = it->ball->GetChild(0);
        script = (child) ? child->GetScript(0) : NULL;
        if (!script)
        {
            context->OutputToConsoleEx("8Script not found %s", child->GetName());
        }
        scene->Activate(script, TRUE);
    }

    return CKBR_OK;
}

float CalcDistanceSquare(CK3dEntity *target, VxVector pos)
{
    if (!target)
    {
        return -1.0f;
    }

    VxVector targetPos;
    target->GetPosition(&targetPos);
    return SquareMagnitude(pos - targetPos);
}

int sub_10002400(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
    {
        return CKBR_OWNERERROR;
    }

    CK3dEntity *ball = (CK3dEntity *)beh->GetInputParameterObject(BALL);

    VxVector ballPos;
    ball->GetPosition(&ballPos);

    float activationDistance;
    beh->GetInputParameterValue(ACTIVATION_DISTANCE, &activationDistance);
    activationDistance *= activationDistance;

    if (CalcDistanceSquare(target, ballPos) > activationDistance)
    {
        float rotationSpeed;
        beh->GetInputParameterValue(ROTATION_SPEED, &rotationSpeed);

        float timeValue;
        beh->GetLocalParameterValue(TIME_VALUE, &rotationSpeed);

        SmallballList smallballs = NULL;
        beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
        if (!smallballs)
        {
            return CKBR_PARAMETERERROR;
        }

        int i = 0;
        VxVector pos;
        VxVector vec1;
        VxVector newPos;
        VxMatrix mat;

        for (Smallball *it = smallballs; it != NULL; it = it->next)
        {
            switch (i)
            {
            case 0:
                vec1.Set(0.0f, 1.0f, 0.0f);
                break;
            case 1:
                vec1.Set(0.0f, 0.0f, 1.0f);
                break;
            case 2:
                vec1.Set(1.0f, 0.0f, 0.0f);
                break;
            case 3:
                vec1.Set(0.5f, 0.5f, 0.7070000171661377f);
                break;
            case 4:
                vec1.Set(0.7070000171661377f, 0.7070000171661377f, 0.0f);
                break;
            case 5:
                vec1.Set(-0.5f, -0.5f, 0.7070000171661377f);
                break;
            default:
                break;
            }

            it->ball->GetPosition(&pos, target);
            Vx3DMatrixFromRotation(mat, vec1, rotationSpeed);
            Vx3DRotateVector(&newPos, mat, &pos);
            float v20 = newPos.Magnitude();
            if (v20 != 2.0f)
            {
                newPos *= (1.0f / v20) * 2;
            }
            it->ball->SetPosition(&newPos, target);
        }
    }
    else
    {
        int status = 1;
        beh->SetLocalParameterValue(STATUS, &status);

        BOOL flyingExtra;
        beh->GetInputParameterValue(FLYING_EXTRA, &flyingExtra);

        CK3dEntity *shadowObject = (CK3dEntity *)beh->GetLocalParameterObject(SHADOW_OBJECT);
        CK3dEntity *centerObject = (CK3dEntity *)beh->GetLocalParameterObject(CENTER_OBJECT);

        CKScene *scene = context->GetCurrentScene();
        if (!scene)
        {
            return CKBR_PARAMETERERROR;
        }

        if (shadowObject && !flyingExtra)
        {
            shadowObject->Show(CKHIDE);
        }

        if (centerObject)
        {
            CK3dEntity *child = centerObject->GetChild(0);
            CKBehavior *script = child->GetScript(0);
            if (!script)
            {
                context->OutputToConsoleEx("14Script not found %s", child->GetName());
            }
            scene->DeActivate(script);
            centerObject->Show(CKHIDE);

            child = centerObject->GetChild(0);
            if (child)
            {
                script = child->GetScript(0);
                child->Show();
                if (!script)
                {
                    context->OutputToConsoleEx("15Script not found %s", child->GetName());
                }
                scene->DeActivate(script);
            }
        }

        SmallballList smallballs = NULL;
        beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
        if (!smallballs)
        {
            return CKBR_PARAMETERERROR;
        }

        for (Smallball *it = smallballs; it != NULL; it = it->next)
        {
            it->ball->GetPosition(&it->position);
        }

        BOOL activated = TRUE;
        beh->SetOutputParameterValue(0, &activated);
        beh->ActivateOutput(0);
        ballPos.y -= 2.1f;
        beh->SetLocalParameterValue(AWAY_POSITION_SAVE, &ballPos);
    }

    return CKBR_OK;
}

int sub_10002870(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    VxVector awayPos;
    beh->GetLocalParameterValue(AWAY_POSITION_SAVE, &awayPos);
    awayPos -= 3.0f;

    float timeCount;
    beh->GetLocalParameterValue(TIME_COUNTER, &timeCount);
    timeCount -= behcontext.DeltaTime;

    if (timeCount <= 0)
    {
        int status = 2;
        beh->SetLocalParameterValue(STATUS, &status);
    }

    beh->SetLocalParameterValue(TIME_COUNTER, &timeCount);

    float awayForce;
    beh->GetInputParameterValue(AWAY_FORCE, &awayForce);

    float awayDamping;
    beh->GetInputParameterValue(AWAY_DAMPING, &awayDamping);

    float timeValue;
    beh->GetLocalParameterValue(TIME_VALUE, &timeValue);
    awayForce *= timeValue;

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    if (!smallballs)
    {
        return CKBR_PARAMETERERROR;
    }

    VxVector pos;
    for (Smallball *it = smallballs; it != NULL; it = it->next)
    {
        VxVector oldPos = it->position;

        it->ball->GetPosition(&pos);
        it->position = pos;

        VxVector newPos = pos + (pos - oldPos) * awayDamping + (pos - awayPos) * awayForce;
        it->ball->SetPosition(&newPos);
    }

    return CKBR_OK;
}

int sub_10002A50(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int ballCount;
    beh->GetLocalParameterValue(BALL_COUNTER, &ballCount);
    if (ballCount <= 0)
    {
        int status = 3;
        beh->SetLocalParameterValue(STATUS, &status);
        beh->ActivateOutput(2);
        return CKBR_OK;
    }

    CK3dEntity *ball = (CK3dEntity *)beh->GetInputParameterObject(BALL);
    if (!ball)
    {
        return CKBR_PARAMETERERROR;
    }

    VxVector ballPos;
    ball->GetPosition(&ballPos);

    float damping;
    beh->GetInputParameterValue(DAMPING, &damping);

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    if (!smallballs)
    {
        return CKBR_PARAMETERERROR;
    }

    CKScene *scene = context->GetCurrentScene();
    if (!scene)
    {
        return CKBR_PARAMETERERROR;
    }

    float timeValue;
    beh->GetLocalParameterValue(TIME_VALUE, &timeValue);

    CK3dEntity *child;
    CKBehavior *script;
    VxVector pos;
    int activatedBallCount = 0;
    int i = 0;
    for (Smallball *it = smallballs; it != NULL; it = it->next, ++i)
    {
        if (it->flag)
        {
            it->ball->GetPosition(&pos);

            float collDistance = 2.0f;
            beh->GetInputParameterValue(EXTRA_POINTS_COLLDISTANCE, &collDistance);
            if (SquareMagnitude(ballPos - pos) <= collDistance)
            {
                ++activatedBallCount;
                --ballCount;
                beh->SetLocalParameterValue(BALL_COUNTER, &ballCount);
                child = it->ball->GetChild(0);
                if (child)
                {
                    script = child->GetScript(0);
                    if (!script)
                    {
                        context->OutputToConsoleEx("17Script not found %s", child->GetName());
                    }
                    scene->DeActivate(script);

                    child = it->ball->GetChild(0);
                    script = child->GetScript(0);
                    if (!script)
                    {
                        context->OutputToConsoleEx("18Script not found %s", child->GetName());
                    }
                    scene->DeActivate(script);
                }
                it->ball->Show(CKHIDE);

                CKGroup *hitFrameGroup = (CKGroup *)beh->GetInputParameterObject(HIT_FRAME_GROUP);
                CK3dEntity *hitFrame = (CK3dEntity *)hitFrameGroup->GetObject(i);
                if (hitFrame)
                {
                    hitFrame->SetPosition(&pos);

                    script = hitFrame->GetScript(0);
                    if (!script)
                    {
                        context->OutputToConsoleEx("19Script not found %s", child->GetName());
                    }
                    scene->DeActivate(script);
                }
            }
            else
            {
                VxVector oldPos = it->position;
                it->position = pos;

                VxVector newPos = pos + (pos - oldPos) * damping + (ballPos - pos) * it->force * timeValue;
                it->ball->SetPosition(&newPos);
            }
        }
    }

    if (activatedBallCount > 0)
    {
        beh->SetOutputParameterValue(0, &activatedBallCount);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

int ExecuteExtra(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int status;
    beh->GetLocalParameterValue(STATUS, &status);

    int ret = CKBR_OK;
    switch (status)
    {
    case 0:
        ret = sub_10002400(behcontext);
        break;
    case 1:
        ret = sub_10002870(behcontext);
        break;
    case 2:
        ret = sub_10002A50(behcontext);
        break;
    default:
        break;
    }

    return (ret == CKBR_OK) ? CKBR_OK : CKBR_PARAMETERERROR;
}

int DestroyExtra(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    SmallballList smallballs = NULL;
    beh->GetLocalParameterValue(SMALLBALL_LIST, &smallballs);
    if (!smallballs)
    {
        return CKBR_PARAMETERERROR;
    }

    CKScene *scene = context->GetCurrentScene();
    if (!scene)
    {
        return CKBR_PARAMETERERROR;
    }

    BOOL flyingExtra;
    beh->GetInputParameterValue(FLYING_EXTRA, &flyingExtra);
    CK3dEntity *shadowObject = (CK3dEntity *)beh->GetLocalParameterObject(SHADOW_OBJECT);
    CK3dEntity *centerObject = (CK3dEntity *)beh->GetLocalParameterObject(CENTER_OBJECT);

    if (centerObject)
    {
        CK3dEntity *child = centerObject->GetChild(0);
        CKBehavior *script = child->GetScript(0);
        if (!script)
        {
            context->OutputToConsoleEx("9Script not found %s", child->GetName());
        }
        scene->DeActivate(script);
        centerObject->Show(CKHIDE);
    }

    if (shadowObject && !flyingExtra)
    {
        shadowObject->Show(CKHIDE);
    }

    CK3dEntity *child;
    CKBehavior *script;
    for (Smallball *it = smallballs; it != NULL; it = it->next)
    {
        child = it->ball->GetChild(0);
        script = child->GetScript(0);
        if (!script)
        {
            context->OutputToConsoleEx("11Script not found %s", child->GetName());
        }
        scene->DeActivate(script);
        it->ball->Show(CKHIDE);

        child = it->ball->GetChild(0);
        script = (child) ? child->GetScript(0) : NULL;
        if (!script)
        {
            context->OutputToConsoleEx("12Script not found %s", child->GetName());
        }
        scene->DeActivate(script);

        if (child)
        {
            script = child->GetScript(0);
            if (!script)
            {
                context->OutputToConsoleEx("13Script not found %s", child->GetName());
            }
            scene->DeActivate(script);
        }
    }

    return CKBR_OK;
}

int Extra(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    BOOL activationStatus = FALSE;
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->SetLocalParameterValue(ACTIVATION_STATUS, &activationStatus);
        DestroyExtra(behcontext);
        return CKBR_OK;
    }

    BOOL initialized;
    beh->GetLocalParameterValue(INITIALIZED, &initialized);

    BOOL exactness;
    int exactnessFrameDelay;
    float timeValue;

    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);
        if (!initialized)
        {
            if (InitializeExtra(behcontext) != CKBR_OK)
            {
                context->OutputToConsole("Couldn't intialize the Extra.");
                return CKBR_PARAMETERERROR;
            }
            return CKBR_OK;
        }
    }
    else if (!beh->IsInputActive(0))
    {
        beh->GetLocalParameterValue(ACTIVATION_STATUS, &activationStatus);
    }

    if (!activationStatus || !initialized)
    {
        return CKBR_ACTIVATENEXTFRAME;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        if (!initialized)
        {
            context->OutputToConsole("Extra isn't initialized yet, this could slow all down.", FALSE);
            if (InitializeExtra(behcontext) != CKBR_OK)
            {
                context->OutputToConsole("Couldn't intialize the Extra.");
                return CKBR_PARAMETERERROR;
            }
            initialized = TRUE;
        }

        activationStatus = TRUE;
        beh->SetLocalParameterValue(ACTIVATION_STATUS, &activationStatus);

        timeValue = behcontext.DeltaTime * 0.001;
        beh->SetLocalParameterValue(TIME_VALUE, &timeValue);

        beh->GetLocalParameterValue(EXACTNESS, &exactness);
        if (exactness)
        {
            beh->GetInputParameterValue(EXACTNESS_FRAME_DELAY, &exactnessFrameDelay);
            beh->SetLocalParameterValue(NEXT_CHECK, &exactnessFrameDelay);
        }

        if (ActivateExtra(behcontext))
        {
            context->OutputToConsole("Couldn't activate the Extra.");
            return CKBR_PARAMETERERROR;
        }
    }

    beh->GetLocalParameterValue(EXACTNESS, &exactness);
    if (exactness)
    {
        beh->GetLocalParameterValue(NEXT_CHECK, &exactnessFrameDelay);
        --exactnessFrameDelay;
        if (exactnessFrameDelay == 0)
        {
            timeValue = behcontext.DeltaTime * 0.001;
            beh->SetLocalParameterValue(TIME_VALUE, &timeValue);
            beh->GetInputParameterValue(EXACTNESS_FRAME_DELAY, &exactnessFrameDelay);
        }
        beh->SetLocalParameterValue(NEXT_CHECK, &exactnessFrameDelay);
    }

    if (ExecuteExtra(behcontext) != CKBR_OK)
    {
        context->OutputToConsole("Couldn't execute the Extra.");
        return CKBR_PARAMETERERROR;
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR ExtraCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDELETE:
    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        BOOL initialized;
        beh->GetLocalParameterValue(INITIALIZED, &initialized);
        if (initialized)
        {
            sub_10001EC0(behcontext);
        }
    }
    break;
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    {
        SmallballList smallballs = NULL;
        beh->SetLocalParameterValue(SMALLBALL_LIST, &smallballs);

        BOOL initialized = FALSE;
        beh->SetLocalParameterValue(INITIALIZED, &initialized);

        int status = -1;
        beh->SetLocalParameterValue(STATUS, &status);

        BOOL activationStatus = FALSE;
        beh->SetLocalParameterValue(ACTIVATION_STATUS, &activationStatus);
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        BOOL exactness;
        beh->GetLocalParameterValue(EXACTNESS, &exactness);
        int inParamCount = beh->GetInputParameterCount();
        if (exactness && inParamCount < 14)
        {
            beh->CreateInputParameter("Exactness Framedelay", CKPGUID_INT);
        }
        else if (inParamCount == 14)
        {
            CKParameterIn *param = beh->RemoveInputParameter(13);
            CKDestroyObject(param);
        }
    }
    break;
    default:
        break;
    }

    return CKBR_OK;
}
