/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DragNDrop
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorDragNDropDecl();
CKERROR CreateDragNDropProto(CKBehaviorPrototype **);
int DragNDrop(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDragNDropDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Drag and Drop");
    od->SetDescription("Enables you to drag and drop a 2D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Limit Rectangle: </SPAN>Rectangle limiting the entity. Let 0,0,0,0 if the entity
    must not be constraint.<BR>
    This behavior allow you to pick and move any 2D Entity, with its
    hierarchy, with the possibility of constraint its moving to a given rectangle.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39c45869, 0x3dcf3fc1));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDragNDropProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Interface/Controls");
    return od;
}

CKERROR CreateDragNDropProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Drag and Drop");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Dragged");
    proto->DeclareOutput("Dropped");

    proto->DeclareInParameter("Limit Rectangle", CKPGUID_RECT, "(0,0),(0,0)");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0");      // State
    proto->DeclareLocalParameter(NULL, CKPGUID_2DVECTOR, "0"); // First Mouse Position

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DragNDrop);

    *pproto = proto;
    return CK_OK;
}

#define RELEASED 1
#define PRESSED 0

int DragNDrop(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK2dEntity *frame = (CK2dEntity *)beh->GetTarget();

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        int state = RELEASED;
        beh->SetLocalParameterValue(0, &state);
    }
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!im)
        return CKBR_ACTIVATENEXTFRAME;

    CKFontManager *fm = (CKFontManager *)behcontext.Context->GetManagerByGuid(FONT_MANAGER_GUID);
    CK2dEntity *ent = fm->GetEntityUnderMouse();

    VxRect lrect;
    beh->GetInputParameterValue(0, &lrect);

    // TODO : button configurable
    int button = 0;

    CKBOOL inside = FALSE;
    if (ent == frame)
        inside = TRUE;

    int state = 0;
    beh->GetLocalParameterValue(0, &state);

    Vx2DVector mp;
    im->GetMousePosition(mp, FALSE);

    if (state == PRESSED)
    {
        Vx2DVector offset;
        beh->GetLocalParameterValue(1, &offset);

        offset += mp;

        if (!lrect.IsEmpty())
            lrect.Clip(offset);
        frame->SetPosition(offset);
    }

    int oldState = state;

    if (im->IsMouseClicked(CK_MOUSEBUTTON(button)) && inside)
    {
        Vx2DVector offset;

        frame->GetPosition(offset);
        offset -= mp;

        beh->SetLocalParameterValue(1, &offset);
        state = PRESSED;
    }
    else
    {
        if (im->IsMouseButtonDown(CK_MOUSEBUTTON(button)))
        {
        }
        else
        {
            if (oldState == PRESSED)
            {
                state = RELEASED;
            }
            else
                state = RELEASED;
        }
    }

    if (state != oldState) // State change
    {
        // we activate the corresponding output
        beh->ActivateOutput(state);

        // we store the new state
        beh->SetLocalParameterValue(0, &state);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
