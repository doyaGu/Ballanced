/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Mimic
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMimicDecl();
CKERROR CreateMimicProto(CKBehaviorPrototype **pproto);
int Mimic(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMimicDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mimic");
    od->SetDescription("Makes a 3D Entity copy the motion of another one.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to copy.<BR>
    <SPAN CLASS=pin>Copy Position: </SPAN>TRUE means the object's position will be copied.<BR>
    <SPAN CLASS=pin>Position Speed: </SPAN>how fast the 3D Entity is to copy the object's position.
    The higher the percentage, the faster the 3D Entity will do this.<BR>
    <SPAN CLASS=pin>Copy Orientation: </SPAN>TRUE means that 3D Entity will copy the object's orientation.<BR>
    <SPAN CLASS=pin>Orientation Speed: </SPAN>how fast the 3D Entity is to copy the object's orientation.
    The higher the percentage, the faster the 3D Entity will do this.<BR>
    <SPAN CLASS=pin>Copy Scale: </SPAN>TRUE means that the 3D Entity will copy the object's scale.<BR>
    <SPAN CLASS=pin>Scale Speed: </SPAN>how fast the 3D Entity is to copy the object's orientation.
    The higher the percentage, the faster the 3D Entity will do this.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame Rate
    dependant. Making this building block Time dependant has the advantage that compositions will run
    at the same rate on all computer configurations.<BR>
    <BR>
    This behavior needs to be looped, if you want the object to constantly mimic the target.<BR>
    */
    /* warning:
    - If you want no attenuation during motion, you shouldn't use the 'Time Based' setting.<BR>
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    Note: if you don't use any '... Speed' value (ie 100), the above remark is useless (because movement is instantaneous).<BR>
    */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12db7b06, 0x74c514c3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMimicProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateMimicProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mimic");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Copy Position", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Position Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Copy Orientation", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Orientation Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Copy Scale", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Scale Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Mimic);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Mimic(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get the Owner Object
    CK3dEntity *Ent1 = (CK3dEntity *)beh->GetTarget();
    if (!Ent1)
        return CKBR_OWNERERROR;

    // Get the Object
    CK3dEntity *Ent2 = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (!Ent2)
        return CKBR_OK;

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(7, &k);
    k = !k;

    CKBOOL b;
    float f = 0.0;

    /////////////////////////////////////////////////////////////////////////////////////
    // Time Based Setting Version ?
    CKBOOL time_based = FALSE;
    beh->GetLocalParameterValue(0, &time_based);

    // Mimic Position
    beh->GetInputParameterValue(1, &b);
    if (b)
    {
        beh->GetInputParameterValue(2, &f);
        if (time_based)
        {
            f *= behcontext.DeltaTime * 0.07f;
            if (f > 1.0f)
                f = 1.0f;
        }

        VxVector p1, p2;
        Ent1->GetPosition(&p1);
        Ent2->GetPosition(&p2);

        p1 += (p2 - p1) * f;

        Ent1->SetPosition(&p1, NULL, k);
    }

    // Mimic orientation
    beh->GetInputParameterValue(3, &b);
    if (b)
    {
        beh->GetInputParameterValue(4, &f);
        if (time_based)
        {
            f *= behcontext.DeltaTime * 0.07f;
            if (f > 1.0f)
                f = 1.0f;
        }

        VxQuaternion quatA, quatB, quatC;

        VxVector scale1, scale2, scaleUnit(1, 1, 1);
        Ent1->GetScale(&scale1);

        Ent1->GetQuaternion(&quatA);
        Ent2->GetQuaternion(&quatB);

        quatC = Slerp(f, quatA, quatB);

        Ent1->SetQuaternion(&quatC, NULL, k);
        Ent1->SetScale(&scale1);
    }

    // Mimic Scale
    beh->GetInputParameterValue(5, &b);
    if (b)
    {
        beh->GetInputParameterValue(6, &f);
        if (time_based)
        {
            f *= behcontext.DeltaTime * 0.07f;
            if (f > 1.0f)
                f = 1.0f;
        }

        VxVector p1, p2;
        Ent1->GetScale(&p1);
        Ent2->GetScale(&p2);

        p1 += (p2 - p1) * f;

        Ent1->SetScale(&p1, k);
    }

    return CKBR_OK;
}
