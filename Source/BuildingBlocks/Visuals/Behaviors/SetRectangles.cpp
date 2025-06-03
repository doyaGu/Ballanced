/////////////////////////////////////////////////////
//		            Set Rectangles
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetRectanglesDecl();
CKERROR CreateSetRectanglesProto(CKBehaviorPrototype **pproto);
int SetRectangles(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRectanglesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Rectangles");
    od->SetDescription("Set the destination and/or source rectangles of a 2DEntity");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Destination: </SPAN>destination rectangle on the screen of the 2D entity (in screen coordinates).<BR>
    <SPAN CLASS=pin>Source: </SPAN>source rectangle on the texture of the 2D entity (in homogeneous UV coordinates).<BR>
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRectanglesProto);
    od->SetCategory("Visuals/2D");
    od->SetGuid(CKGUID(0x3bcf7052, 0x59f40417));
    od->SetAuthorGuid(CKGUID(0x56495254, 0x4f4f4c53));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_2DENTITY);

    return od;
}

CKERROR CreateSetRectanglesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Rectangles");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Destination", CKPGUID_RECT);
    proto->DeclareInParameter("Source", CKPGUID_RECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(SetRectangles);

    *pproto = proto;
    return CK_OK;
}

int SetRectangles(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OK;

    VxRect rect;

    // Dest Rectangle
    if (!beh->GetInputParameterValue(0, &rect))
    {
        ent->SetRect(rect);
    }

    // Source Rectangle
    if (!beh->GetInputParameterValue(1, &rect))
    {
        ent->SetSourceRect(rect);
    }

    // active ios
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
