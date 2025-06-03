/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetScreenPosProportional
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetScreenPosProportionalDecl();
CKERROR CreateGetScreenPosProportionalProto(CKBehaviorPrototype **);
int GetScreenPosProportional(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetScreenPosProportionalDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Screen Proportional Pos");
    od->SetDescription("Converts a position from a 2d proportional position to the screen.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>%Pos X: </SPAN>proportional X position on screen.<BR>
    <SPAN CLASS=pin>%Pos Y: </SPAN>proportional Y position on screen.<BR>
    <BR>
    <SPAN CLASS=pout>Pos X: </SPAN>X position on screen.<BR>
    <SPAN CLASS=pout>Pos Y: </SPAN>Y position on screen.<BR>
    <BR>
    <SPAN CLASS=setting>Window Relative: </SPAN>should the coordinate be window relative.<BR>
    <BR>
    eg: if the 3dview size is (400,300), and %X=50,%Y=50 then X=200 and Y=150.<BR>
    */
    /* warning:
    - the position is to be given in the 3dview window.<BR>
    */
    od->SetCategory("Interface/Screen");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7bdf0404, 0xf765834));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateGetScreenPosProportionalProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetScreenPosProportionalProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Screen Proportional Pos");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("%Pos X", CKPGUID_PERCENTAGE);
    proto->DeclareInParameter("%Pos Y", CKPGUID_PERCENTAGE);
    proto->DeclareOutParameter("Pos X", CKPGUID_INT);
    proto->DeclareOutParameter("Pos Y", CKPGUID_INT);

    proto->DeclareSetting("Window Relative", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetScreenPosProportional);

    *pproto = proto;
    return CK_OK;
}

int GetScreenPosProportional(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    float percx, percy;
    beh->GetInputParameterValue(0, &percx);
    beh->GetInputParameterValue(1, &percy);

    CKRenderContext *rc = behcontext.CurrentRenderContext;
    if (!rc)
        return CKBR_OK;

    int x = (int)(percx * rc->GetWidth());
    int y = (int)(percy * rc->GetHeight());

    CKBOOL wr = TRUE;
    beh->GetLocalParameterValue(0, &wr);

    if (!wr)
    {
        VxRect r;
        rc->GetWindowRect(r);

        x += (int)r.left;
        y += (int)r.top;
    }

    beh->SetOutputParameterValue(0, &x);
    beh->SetOutputParameterValue(1, &y);

    beh->ActivateOutput(0);
    return CKBR_OK;
}
