/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetZOrder
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetZOrderDecl();
CKERROR CreateSetZOrderProto(CKBehaviorPrototype **pproto);
int SetZOrder(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetZOrderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Render Order");
    od->SetDescription("Tells the object to be displayed FIRST, LAST, or in a normal way.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Display First: </SPAN>boolean value to check if you want the 3dobject to be render FIRST in the rendering process.<BR>
    <SPAN CLASS=pin>Display Last: </SPAN>boolean value to check if you want the 3dobject to be render LAST in the rendering process.<BR>
    <BR>
    Useful to tell an object as to be rendered first (as a sky plane), or last (as an inventory object).<BR>
    */
    /* warning:
    - This building block sets a flag in the object state, therefore, as for 'set diffuse' or building block like that, once it's done, the object will remain in the new state.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6f8b6782, 0x21554283));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetZOrderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateSetZOrderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Render Order");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Display First", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Display Last", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Rendering Priority", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetZOrder);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetZOrder(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *obj = (CK3dEntity *)beh->GetTarget();
    if (!obj)
        return CKBR_OWNERERROR;

    CKBOOL first = FALSE;
    beh->GetInputParameterValue(0, &first);

    CKBOOL last = FALSE;
    beh->GetInputParameterValue(1, &last);

    CKDWORD flags = obj->GetMoveableFlags();

    if (first)
        flags |= VX_MOVEABLE_RENDERFIRST;
    else
        flags &= ~VX_MOVEABLE_RENDERFIRST;

    if (last)
        flags |= VX_MOVEABLE_RENDERLAST;
    else
        flags &= ~VX_MOVEABLE_RENDERLAST;

    if (!first)
    {
        // If Render first is set the priority is ignored as
        // it is force to Max Priority
        int zOrder = 0;
        beh->GetInputParameterValue(2, &zOrder);
        obj->SetZOrder(zOrder);
    }

    obj->SetMoveableFlags(flags);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
