/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					Get Nearest In Group
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include <float.h>

CKObjectDeclaration *FillBehaviorGetNearestInGroupDecl();
CKERROR CreateGetNearestInGroupProto(CKBehaviorPrototype **);
int GetNearestInGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetNearestInGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Nearest In Group");
    od->SetDescription("Get the nearest object in the selected group from the given position.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>name of the group in which to search.<BR>
    <SPAN CLASS=pin>Position: </SPAN>vector expressed in cartesian coordinates.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>coordinate system in which the position is expressed.<BR>
    <BR>
    <SPAN CLASS=pout>Nearest Object: </SPAN>nearest object from the owner. It can't be the same than the given referential.<BR>
    <SPAN CLASS=pout>Distance: </SPAN>distance to the nearest object.<BR>
    */
    /* warning:
    - as many other distances given in Virtools, the distance here is the distance between the two objects basis.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x85207eb, 0x584950d8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetNearestInGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateGetNearestInGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Nearest In Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);
    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);

    proto->DeclareOutParameter("Nearest Object", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Distance", CKPGUID_FLOAT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetNearestInGroup);

    *pproto = proto;
    return CK_OK;
}

int GetNearestInGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        return CKBR_PARAMETERERROR;

    int c = group->GetObjectCount();

    CK3dEntity *ent;
    VxVector pos(0.0f);
    beh->GetInputParameterValue(1, &pos);

    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(2);
    if (ref)
    {
        ref->Transform(&pos, &pos);
    }

    beh->SetOutputParameterObject(0, NULL);

    float min = FLT_MAX;
    CK3dEntity *minent = NULL;
    int i = 0;
    while (i < c)
    {
        ent = (CK3dEntity *)group->GetObject(i);
        if (ref != ent)
        {
            if (CKIsChildClassOf(ent, CKCID_3DENTITY))
            {
                VxVector p;
                ent->GetPosition(&p);
                float m = SquareMagnitude(p - pos);
                if (m < min)
                {
                    min = m;
                    minent = ent;
                }
            }
        }
        i++;
    }

    // write outputs parameters
    beh->SetOutputParameterObject(0, minent);
    min = sqrtf(min);
    beh->SetOutputParameterValue(1, &min);

    return CKBR_OK;
}
