/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Proximity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorProximityDecl();
CKERROR CreateProximityProto(CKBehaviorPrototype **);
int Proximity(const CKBehaviorContext &behcontext);

CKERROR ProximityCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKSTRING A_outputname[4] = {
    "In Range",
    "Out Range",
    "Enter Range",
    "Exit Range"};

CKObjectDeclaration *FillBehaviorProximityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Proximity");
    od->SetDescription("Compares the distance between two 3D Entities with a given threshold distance.");
    /* rem:
    <SPAN CLASS=in>In: triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>InRange: </SPAN>is activated if the calculated distance is inferior to the threshold distance.<BR>
    <SPAN CLASS=out>OutRange: </SPAN>is activated if the calculated distance is superior to the threshold distance.<BR>
    <SPAN CLASS=out>EnterRange: </SPAN>is activated when swithcing from OutRange to InRange (out->in).<BR>
    <SPAN CLASS=out>ExitRange: </SPAN>is activated when swithcing from InRange to OutRange (in->out).<BR>
    <SPAN CLASS=out>Else: </SPAN>is activated if none of the other outputs are activated.<BR>
    <BR>
    <SPAN CLASS=pin>Distance: </SPAN>threshold distance.<BR>
    <SPAN CLASS=pin>Object A: </SPAN>first 3D entity.<BR>
    <SPAN CLASS=pin>Object B: </SPAN>second 3D entity.<BR>
    <SPAN CLASS=pin>Barycenter: </SPAN>test the proximity from the barycenters of the objects, otherwise, distance is calculate by considering the object basis.<BR>
    <BR>
    <SPAN CLASS=pout>Current Distance: </SPAN>current distance between the 3D Entities.<BR>
    <BR>
    <SPAN CLASS=setting>Use Outputs: </SPAN>check only the outputs that may be relevant to your use.<BR>
    The useless outputs will then be removed, and only those useful will be kept.<BR>
    */
    /* warning:
    - There's always only one output activated at a time. This means that if a object enters
    the range, the only activated output will be "EnterRange", even if in fact it is also inside the range.<BR>
    - If the "EnterRange" output doesn't exist but the "InRange" output exists, then the "InRange" ouput will
    be activated as soon as the object enter the range.<BR>
    - If the "ExitRange" output doesn't exist but the "OutRange" output exists, then the "OutRange" ouput will
    be activated as soon as the object leave the range.<BR>
    */
    od->SetCategory("Logics/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5321cacb, 0xdcdc5213));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateProximityProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);

    return od;
}

#define CKPGUID_PROXIMITY CKDEFINEGUID(0x7fff5699, 0x7571336d)

CKERROR CreateProximityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Proximity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput(A_outputname[0]);
    proto->DeclareOutput(A_outputname[1]);
    proto->DeclareOutput(A_outputname[2]);
    proto->DeclareOutput(A_outputname[3]);

    proto->DeclareInParameter("Distance", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Object A", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Object B", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Barycenter", CKPGUID_BOOL);

    proto->DeclareOutParameter("Current Distance", CKPGUID_FLOAT);

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "2"); // "wasinrange"
    proto->DeclareSetting("Use Ouputs:", CKPGUID_PROXIMITY, "In Range,Out Range,Enter Range,Exit Range");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Proximity);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));

    proto->SetBehaviorCallbackFct(ProximityCallBack);

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

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR ProximityCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:

        //_________________________ Get old flag
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

        //_________________________ Get flag
        int flag = 15;
        beh->GetLocalParameterValue(1, &flag);

        if (old_flag != flag)
        {
            int count = beh->GetOutputCount();
            for (a = 0; a < count; a++)
                beh->DeleteOutput(0);

            //__________________________ Adding Outputs
            for (a = 0; a < 4; a++)
            {
                if (flag & (1 << a))
                {
                    beh->AddOutput(A_outputname[a]);
                }
            }

            if ((flag & 3) != 3)
            {
                beh->AddOutput("Else");
            }
        }

        break;
    }

    return CKBR_OK;
}

/*******************************************************/
/*             BUILDING BLOCK FUNCTION                 */
/*******************************************************/
int Proximity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    float distance;
    beh->GetInputParameterValue(0, &distance); // Get the distance
    CKBOOL barycenter = FALSE;
    beh->GetInputParameterValue(3, &barycenter); // Get the distance

    // Get Object A
    CK3dEntity *ckA = (CK3dEntity *)beh->GetInputParameterObject(1);
    // Get Object B
    CK3dEntity *ckB = (CK3dEntity *)beh->GetInputParameterObject(2);

    VxVector posA, posB;
    if (barycenter)
    {
        if (ckA->GetClassID() == CKCID_CHARACTER)
        {
            const VxBbox &box = ckA->GetBoundingBox(FALSE);
            posA = (box.Min + box.Max) * 0.5f;
        }
        else
        {
            ckA->GetBaryCenter(&posA);
        }
        if (ckB->GetClassID() == CKCID_CHARACTER)
        {
            const VxBbox &box = ckB->GetBoundingBox(FALSE);
            posB = (box.Min + box.Max) * 0.5f;
        }
        else
        {
            ckB->GetBaryCenter(&posB);
        }
    }
    else
    {
        ckA->GetPosition(&posA, NULL);
        ckB->GetPosition(&posB, NULL);
    }

    float current_distance = Magnitude(posA - posB);

    beh->SetOutputParameterValue(0, &current_distance); // Set the current distance

    //___________________// Outputs
    int flag = 15;
    beh->GetLocalParameterValue(1, &flag); // Get settings flag

    int wasin;
    beh->GetLocalParameterValue(0, &wasin);

    int activation = 0;

    if (current_distance < distance)
    {

        if ((wasin == WASOUT) && (flag & A_ENTERRANGE))
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

        if ((wasin == WASIN) && (flag & A_EXITRANGE))
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

    int a, b = 0;
    for (a = 0; a < 4; a++)
    {
        if (flag & (1 << a))
        {
            if (activation & (1 << a))
            {
                beh->ActivateOutput(b);
                return CKBR_OK;
            }
            ++b;
        }
    }

    beh->ActivateOutput(b); // Else

    return CKBR_OK;
}
