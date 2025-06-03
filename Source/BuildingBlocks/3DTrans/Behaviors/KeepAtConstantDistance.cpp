/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Keep At Constant Distance
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorKeepAtConstantDistanceDecl();
CKERROR CreateKeepAtConstantDistanceProto(CKBehaviorPrototype **pproto);
int KeepAtConstantDistance(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeepAtConstantDistanceDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Keep At Constant Distance");
    od->SetDescription("Selected object will follow another object wherever it goes, keeping the same distance between them.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>vector expressed in the 'Referential'.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>3D object to follow.<BR>
    <SPAN CLASS=pin>Distance: </SPAN>distance to be kept between the 2 objects.<BR>
    <SPAN CLASS=pin>Attenuation: </SPAN>delay between second object following first object. A value of 0 (zero) would mean an instant response.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    <SPAN CLASS=setting>If checked, this behavior will be Time and not Frame Rate dependant.
    Making this building block Time dependant has the advantage that compositions will run at
    the same rate on all computer configurations.<BR>
    <BR>
    This building block should be looped if the 3D Entity is to keep a constant distance
    from the object all the time.<BR>
    <BR>
    */
    /* warning:
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is that the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    Note: if you don't use any 'Attenuation' value (ie 0), the above remark is useless (because movement is instantaneous).<BR>
    - If the current distance between the two objects is smaller than 0.001 units, the distance vector between objects will
      be cast to [ 0, 0, 0.001 ].<BR>
      */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xb7e6398a, 0x113c66dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeepAtConstantDistanceProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateKeepAtConstantDistanceProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Keep At Constant Distance");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Distance", CKPGUID_FLOAT);
    proto->DeclareInParameter("Attenuation", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeepAtConstantDistance);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int KeepAtConstantDistance(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get the Owner Object
    CK3dEntity *Ent1 = (CK3dEntity *)beh->GetTarget();
    if (!Ent1)
        return CKBR_OWNERERROR;

    // Get the position of the point to keep at distance in Object referential
    VxVector Ent2Pos = VxVector::axis0();
    CKParameterIn *pin = beh->GetInputParameter(0);
    int pindec = 0;
    if (pin->GetGUID() == CKPGUID_VECTOR)
    { // check for back compatibility
        pindec = 1;
        pin->GetValue(&Ent2Pos);
    }

    // Get the Object to keep at distance
    CK3dEntity *Ent2 = (CK3dEntity *)beh->GetInputParameterObject(0 + pindec);

    if (Ent2)
    { // Transform point position to world if referential != world
        if (Ent2Pos == VxVector::axis0())
        { // in many case the vector is null...
            Ent2->GetPosition(&Ent2Pos);
        }
        else
        { // calculate transformation only in this case
            Ent2->Transform(&Ent2Pos, &Ent2Pos);
        }
    }

    // position of Owner Object
    VxVector Ent1Pos;
    Ent1->GetPosition(&Ent1Pos);

    // Get the distance wanted
    float wanted_d = 1.0f;
    beh->GetInputParameterValue(1 + pindec, &wanted_d);

    // Get the current distance
    VxVector v = Ent2Pos - Ent1Pos;
    float current_d = Magnitude(v);

    if (current_d < 0.001f)
    {
        current_d = 0.001f;
        v.Set(0, 0, 0.001f);
    }

    // Get the attenuation value
    float attenuation = 0.0f;
    beh->GetInputParameterValue(2 + pindec, &attenuation);
    if (attenuation < 0)
        attenuation = 0;

    /////////////////////////////////////////////////////////////////////////////////////
    // Time Based Setting Version ?
    CKBOOL time_based = FALSE;
    beh->GetLocalParameterValue(0, &time_based);

    if (time_based)
        attenuation /= behcontext.DeltaTime * 0.1f;
    /////////////////////////////////////////////////////-- End of Version difference

    wanted_d = current_d - wanted_d;

    float tmp;
    if (tmp = current_d * (attenuation + 1.0f))
    {
        float f = wanted_d / tmp;
        v *= f;
    }
    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3 + pindec, &k);
    k = !k;

    Ent1->Translate(&v, NULL, k);

    return CKBR_OK;
}
