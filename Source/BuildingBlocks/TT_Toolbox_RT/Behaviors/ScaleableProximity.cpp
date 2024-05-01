//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT Scaleable Proximity
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorScaleableProximityDecl();
CKERROR CreateScaleableProximityProto(CKBehaviorPrototype **pproto);
int ScaleableProximity(const CKBehaviorContext &behcontext);
CKERROR ScaleableProximityCallBack(const CKBehaviorContext &behcontext);

CKSTRING A_outputname[4] = {
    "In Range",
    "Out Range",
    "Enter Range",
    "Exit Range"};

CKObjectDeclaration *FillBehaviorScaleableProximityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Scaleable Proximity");
    od->SetDescription("Compares the distance between two 3D Entities with a given threshold distance.");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2a2a63ca, 0x826175));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateScaleableProximityProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateScaleableProximityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Scaleable Proximity");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput(A_outputname[0]);
    proto->DeclareOutput(A_outputname[1]);
    proto->DeclareOutput(A_outputname[2]);
    proto->DeclareOutput(A_outputname[3]);

    proto->DeclareInParameter("Distance", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("ObjectA", CKPGUID_3DENTITY);
    proto->DeclareInParameter("ObjectB", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Barycenter?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Exactness min. Distance", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Exactness max. Distance", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Minimum Framedelay", CKPGUID_INT, "1");
    proto->DeclareInParameter("Maximum Framedelay", CKPGUID_INT, "10");

    proto->DeclareOutParameter("Current Distance", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("", CKPGUID_INT, "2");
    proto->DeclareLocalParameter("Last Check", CKPGUID_INT, "1");

    proto->DeclareSetting("Use Ouputs:", CKPGUID_PROXIMITY, "InRange,OutRange,EnterRange,ExitRange");
    proto->DeclareSetting("Check Axis:", CKPGUID_CHECKAXIS, "X,Y,Z");
    proto->DeclareSetting("Squared Distance?", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ScaleableProximity);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(ScaleableProximityCallBack);

    *pproto = proto;
    return CK_OK;
}

#define WASOUT 0
#define WASIN 1

#define A_ALL 15
#define A_INRANGE 1
#define A_OUTRANGE 2
#define A_ENTERRANGE 4
#define A_EXITRANGE 8

#define A_X 1
#define A_Y 2
#define A_Z 4

int ScaleableProximity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int flag = A_ALL;
    beh->GetLocalParameterValue(2, &flag);

    int checkAxis = 0;
    beh->GetLocalParameterValue(3, &checkAxis);

    CKBOOL squaredDistance = TRUE;
    beh->GetLocalParameterValue(4, &squaredDistance);

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        int wasin = A_OUTRANGE;
        beh->SetLocalParameterValue(0, &wasin);
    }

    int lastCheck = 1;
    beh->GetLocalParameterValue(1, &lastCheck);

    if (--lastCheck > 0)
    {
        beh->SetLocalParameterValue(1, &lastCheck);
        return CKBR_ACTIVATENEXTFRAME;
    }

    float distance;
    beh->GetInputParameterValue(0, &distance);
    CKBOOL barycenter = FALSE;
    beh->GetInputParameterValue(3, &barycenter);

    // Get Object A
    CK3dEntity *entA = (CK3dEntity *)beh->GetInputParameterObject(1);
    // Get Object B
    CK3dEntity *entB = (CK3dEntity *)beh->GetInputParameterObject(2);
    if (!entA || !entB)
        return CKBR_PARAMETERERROR;

    VxVector posA, posB;
    if (barycenter)
    {
        if (entA->GetClassID() == CKCID_CHARACTER)
        {
            const VxBbox &box = entA->GetBoundingBox();
            posA = (box.Min + box.Max) * 0.5f;
        }
        else
        {
            entA->GetBaryCenter(&posA);
        }

        if (entB->GetClassID() == CKCID_CHARACTER)
        {
            const VxBbox &box = entB->GetBoundingBox();
            posB = (box.Min + box.Max) * 0.5f;
        }
        else
        {
            entB->GetBaryCenter(&posB);
        }
    }
    else
    {
        entA->GetPosition(&posA);
        entB->GetPosition(&posB);
    }

    VxVector aToB;
    float currentDistance = 0;
    if ((checkAxis & A_Z) != 0)
    {
        float z = posB.z - posA.z;
        aToB.z = z;
        currentDistance += z * z;
    }
    if ((checkAxis & A_Y) != 0)
    {
        float y = posB.y - posA.y;
        aToB.y = y;
        currentDistance += y * y;
    }
    if ((checkAxis & A_X) != 0)
    {
        float x = posB.x - posA.x;
        aToB.x = x;
        currentDistance += x * x;
    }

    float minDistanceExactness, maxDistanceExactness;
    int minFrameDelay, maxFrameDelay;
    beh->GetInputParameterValue(4, &minDistanceExactness);
    beh->GetInputParameterValue(5, &maxDistanceExactness);
    beh->GetInputParameterValue(6, &minFrameDelay);
    beh->GetInputParameterValue(7, &maxFrameDelay);

    if (squaredDistance)
    {
        distance = distance * distance;
        minDistanceExactness = minDistanceExactness * minDistanceExactness;
        maxDistanceExactness = maxDistanceExactness * maxDistanceExactness;
    }
    else
    {
        currentDistance = powf(currentDistance, 0.5f);
    }

    beh->SetOutputParameterValue(0, &currentDistance);
    beh->SetOutputParameterValue(1, &aToB);

    if (currentDistance < maxDistanceExactness)
    {
        if (currentDistance > minDistanceExactness)
        {
            lastCheck = minFrameDelay + (int) ((currentDistance - minDistanceExactness) /
                                (double) (maxDistanceExactness - minDistanceExactness) *
                                (maxFrameDelay - minFrameDelay));
        }
        else
        {
            lastCheck = minFrameDelay;
        }
    }
    else
    {
        lastCheck = maxFrameDelay;
    }

    if (lastCheck < 1)
        lastCheck = 1;

    beh->SetLocalParameterValue(1, &lastCheck);

    int wasin;
    beh->GetLocalParameterValue(0, &wasin);

    int activation;
    if (currentDistance < distance)
    {
        if (wasin == WASOUT || wasin == 2)
        {
            activation = A_ENTERRANGE;
        }
        else
        {
            activation = A_INRANGE;
        }
        wasin = WASIN;
    }
    else
    {
        if (wasin == WASIN || wasin == 2)
        {
            activation = A_EXITRANGE;
        }
        else
        {
            activation = A_OUTRANGE;
        }
        wasin = WASOUT;
    }

    beh->SetLocalParameterValue(0, &wasin);

    int pos = 0;
    for (int i = 0; i < 4; ++i)
    {
        if ((flag & (1 << i)) != 0)
        {
            if ((activation & (1 << i)) != 0)
            {
                beh->ActivateOutput(pos);
            }
            ++pos;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR ScaleableProximityCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
        // Get old flag
        int old_flag = 0;
        CKBehaviorIO *io;
        int a, b = 0;
        for (a = 0; a < 4; ++a)
        {
            char *name = A_outputname[a];

            io = beh->GetOutput(b);
            if (!io)
                break;

            if (!strcmp(name, io->GetName()))
            {
                old_flag += 1 << a;
                ++b;
            }
        }

        // Get flag
        int flag = A_ALL;
        beh->GetLocalParameterValue(2, &flag);

        if (old_flag != flag)
        {
            int count = beh->GetOutputCount();
            for (a = 0; a < count; a++)
                beh->DeleteOutput(0);

            // Adding Outputs
            for (a = 0; a < 4; a++)
            {
                if (flag & (1 << a))
                {
                    beh->AddOutput(A_outputname[a]);
                }
            }
        }
        break;
    }

    return CKBR_OK;
}