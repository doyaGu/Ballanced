/////////////////////////////////////////////////////
//		            Set 2DPosition
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSet2DPositionDecl();
CKERROR CreateSet2DPositionProto(CKBehaviorPrototype **pproto);
int Set2DPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set 2D Position");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>Position of the 2D entity, absolute or relative to a referential.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>Referential in wich the position is given (NULL means the screen).<BR>
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DPositionProto);
    od->SetDescription("Set the position of a 2D Entity");
    od->SetCategory("Visuals/2D");
    od->SetGuid(CKGUID(0x32d30604, 0x23b163fc));
    od->SetAuthorGuid(CKGUID(0x56495254, 0x4f4f4c53));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_2DENTITY);

    return od;
}

CKERROR CreateSet2DPositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set 2D Position");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_2DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(Set2DPosition);

    *pproto = proto;
    return CK_OK;
}

int Set2DPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OK;

    Vx2DVector pos;
    beh->GetInputParameterValue(0, &pos);

    CK2dEntity *ref = (CK2dEntity *)beh->GetInputParameterObject(1);

    // Sets the position of the 2D entity
    ent->SetPosition(pos, FALSE, FALSE, ref);

    // active ios
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
