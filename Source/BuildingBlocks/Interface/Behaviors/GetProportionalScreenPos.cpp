/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       GetProportionalScreenPos
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetProportionalScreenPosDecl();
CKERROR CreateGetProportionalScreenPosProto(CKBehaviorPrototype **);
int GetProportionalScreenPos(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetProportionalScreenPosDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Proportional Screen Pos");
    od->SetDescription("Converts a given 2d position on the screen to a proportional position.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Pos X: </SPAN>X position on screen.<BR>
    <SPAN CLASS=pin>Pos Y: </SPAN>Y position on screen.<BR>
    <BR>
    <SPAN CLASS=pout>%Pos X: </SPAN>proportional X position on screen.<BR>
    <SPAN CLASS=pout>%Pos Y: </SPAN>proportional Y position on screen.<BR>
    <BR>
    eg: if the 3dview size is (400,300), and X=200,Y=150 then %X=50 and %Y=50.<BR>
    */
    /* warning:
    - the position is supposed to be given in the 3dview window.<BR>
    */
    od->SetCategory("Interface/Screen");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x671e53a2, 0x5ba16d62));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetProportionalScreenPosProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetProportionalScreenPosProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Proportional Screen Pos");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Pos X", CKPGUID_INT);
    proto->DeclareInParameter("Pos Y", CKPGUID_INT);
    proto->DeclareOutParameter("%Pos X", CKPGUID_PERCENTAGE);
    proto->DeclareOutParameter("%Pos Y", CKPGUID_PERCENTAGE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetProportionalScreenPos);

    *pproto = proto;
    return CK_OK;
}

int GetProportionalScreenPos(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    int x, y;
    beh->GetInputParameterValue(0, &x);
    beh->GetInputParameterValue(1, &y);

    CKRenderContext *rc = behcontext.CurrentRenderContext;
    if (!rc)
        return CKBR_OK;

    float percx = (float)x / rc->GetWidth();
    float percy = (float)y / rc->GetHeight();

    beh->SetOutputParameterValue(0, &percx);
    beh->SetOutputParameterValue(1, &percy);

    beh->ActivateOutput(0);
    return CKBR_OK;
}
