/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Display2DSprite
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDisplay2DSpriteDecl();
CKERROR CreateDisplay2DSpriteProto(CKBehaviorPrototype **pproto);
int Display2DSprite(const CKBehaviorContext &behcontext);

CKERROR Display2DSpriteCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKObjectDeclaration *FillBehaviorDisplay2DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Edit 2D Entity");
    od->SetDescription("Changes the 2d entity parameters.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>X: </SPAN>Horizontal position of the 2d Entity.<BR>
    <SPAN CLASS=pin>Y: </SPAN>Vertical position of the 2d Entity.<BR>
    <SPAN CLASS=pin>Z Order: </SPAN>display order relatively to the other 2d entity.<BR>
    <SPAN CLASS=pin>Background: </SPAN>Checked = Behind every object of the scene, Not Checked = (Foreground) In front of every object of the scene.<BR>
    <SPAN CLASS=pin>Pickable: </SPAN>Checked = The sprite can be picked.<BR>
    <SPAN CLASS=pin>Clip To Parent: </SPAN>Checked = The sprite will be clipped to the extents of its parent.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7fc54c91, 0x3f486670));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDisplay2DSpriteProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Visuals/2D");
    return od;
}

#define CKPGUID_SPRITEEDITION CKDEFINEGUID(0x11cb48a5, 0x47e5424b)

CKERROR CreateDisplay2DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Edit 2D Entity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Pos X", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Pos Y", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Z Order", CKPGUID_INT, "0");
    proto->DeclareInParameter("Background", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Pickable", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Clip To Parent", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Stick Left", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Stick Right", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Stick Top", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Stick Bottom", CKPGUID_BOOL, "FALSE");

    proto->DeclareSetting("Edited Components :", CKPGUID_SPRITEEDITION, "Position,Z Ordering,Background,Pickable,Clip To Parent,Parent Alignment");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Display2DSprite);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_TARGETABLE));

    proto->SetBehaviorCallbackFct(Display2DSpriteCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR Display2DSpriteCallBack(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        CKParameterLocal *l = beh->GetLocalParameter(0);
        if (l->GetGUID() != CKPGUID_SPRITEEDITION)
        {
            l->SetGUID(CKPGUID_SPRITEEDITION); // change type from 'sprite' to 'flags'
            int flag = 7;
            l->SetValue(&flag);
        }
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        int flag = 7;
        beh->GetLocalParameterValue(0, &flag);

        int count = beh->GetInputParameterCount();

        char in_str[32];
        int in = 0;

        CKParameterIn *pin = NULL;
        CKParameter *pout = NULL;

        if (flag & 1)
        { // Position
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Pos X");
                if (pin->GetGUID() != CKPGUID_FLOAT)
                {
                    pin->SetGUID(CKPGUID_FLOAT, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Pos X");
                beh->CreateInputParameter(in_str, CKPGUID_FLOAT);
            }
            ++in;

            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Pos Y");
                if (pin->GetGUID() != CKPGUID_FLOAT)
                {
                    pin->SetGUID(CKPGUID_FLOAT, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Pos Y");
                beh->CreateInputParameter(in_str, CKPGUID_FLOAT);
            }
            ++in;
        }

        if (flag & 2)
        { // Z Ordering
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Z Order");
                if (pin->GetGUID() != CKPGUID_INT)
                {
                    pin->SetGUID(CKPGUID_INT, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Z Order");
                beh->CreateInputParameter(in_str, CKPGUID_INT);
            }
            ++in;
        }

        if (flag & 4)
        { // Background
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Background");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Background");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;
        }

        if (flag & 8)
        { // Size
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Size X");
                if (pin->GetGUID() != CKPGUID_FLOAT)
                {
                    pin->SetGUID(CKPGUID_FLOAT, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Size X");
                beh->CreateInputParameter(in_str, CKPGUID_FLOAT);
            }
            ++in;

            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Size Y");
                if (pin->GetGUID() != CKPGUID_FLOAT)
                {
                    pin->SetGUID(CKPGUID_FLOAT, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Size Y");
                beh->CreateInputParameter(in_str, CKPGUID_FLOAT);
            }
            ++in;
        }
        if (flag & 16)
        { // Pickable
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Pickable");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Pickable");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;
        }
        if (flag & 32)
        { // ClipToPArent
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Clip To Parent");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Clip To Parent");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;
        }
        if (flag & 64)
        { // ClipToPArent
            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Stick Left");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Stick Left");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;

            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Stick Right");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Stick Right");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;

            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Stick Top");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Stick Top");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;

            pin = beh->GetInputParameter(in);
            if (pin)
            {
                pin->SetName("Stick Bottom");
                if (pin->GetGUID() != CKPGUID_BOOL)
                {
                    pin->SetGUID(CKPGUID_BOOL, TRUE);
                }
                pout = pin->GetRealSource();
                if (CKIsChildClassOf(pout, CKCID_PARAMETERLOCAL))
                    pout->SetName(pin->GetName(), TRUE);
            }
            else
            {
                sprintf(in_str, "Stick Bottom");
                beh->CreateInputParameter(in_str, CKPGUID_BOOL);
            }
            ++in;
        }

        count = beh->GetInputParameterCount();
        for (int a = in; a < count; a++)
            ctx->DestroyObject(beh->RemoveInputParameter(in));
    }
    break;
    }

    return CKBR_OK;
}

/*******************************************************/
/*                   BEH FUNCTION                      */
/*******************************************************/
int Display2DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dEntity *sprite = (CK2dEntity *)beh->GetTarget();
    if (!sprite)
        return CKBR_OWNERERROR;

    CKBOOL homogen = FALSE; // homogeneous coordinates ?
    homogen = sprite->IsHomogeneousCoordinates();

    int flag = 7; // flags
    beh->GetLocalParameterValue(0, &flag);

    int b = 0;

    if (flag & 1)
    { // position
        CKParameterIn *pin = beh->GetInputParameter(0);
        if (pin->GetGUID() == CKPGUID_INT)
        {
            if (homogen)
            {
                behcontext.Context->OutputToConsoleEx("Edit 2D Entity can't stay with 'Integer' for position if coordinates are Homogenous ! (Need to Drag'N Drop again)");
            }
            else
            {
                int x = 0;
                int y = 0;
                beh->GetInputParameterValue(b + 0, &x);
                beh->GetInputParameterValue(b + 1, &y);
                sprite->SetPosition(Vx2DVector((float)x, (float)y));
            }
        }
        else
        {
            float x = 0;
            float y = 0;
            beh->GetInputParameterValue(b + 0, &x);
            beh->GetInputParameterValue(b + 1, &y);
            sprite->SetPosition(Vx2DVector(x, y), homogen);
        }
    }
    else
        b -= 2;

    if (flag & 2)
    { // Z Order
        int z = 0;
        beh->GetInputParameterValue(b + 2, &z);
        sprite->SetZOrder(z);
    }
    else
        b--;

    if (flag & 4)
    { // Background
        CKBOOL back = FALSE;
        beh->GetInputParameterValue(b + 3, &back);
        sprite->SetBackground(back);
    }
    else
        b--;

    if (flag & 8)
    { // size
        float sizex;
        float sizey;
        beh->GetInputParameterValue(b + 4, &sizex);
        beh->GetInputParameterValue(b + 5, &sizey);
        sprite->SetSize(Vx2DVector(sizex, sizey), homogen);
    }
    else
        b -= 2;

    if (flag & 16)
    { // Pickable
        CKBOOL pick = FALSE;
        beh->GetInputParameterValue(b + 6, &pick);
        sprite->SetPickable(pick);
    }
    else
        b--;

    if (flag & 32)
    { // Clip To Parent
        CKBOOL ctp = FALSE;
        beh->GetInputParameterValue(b + 7, &ctp);
        sprite->SetClipToParent(ctp);
    }
    else
        b--;

    if (flag & 64)
    { // Clip To Parent
        CKBOOL sl = FALSE, sr = FALSE, st = FALSE, sb = FALSE;
        beh->GetInputParameterValue(b + 8, &sl);
        beh->GetInputParameterValue(b + 9, &sr);
        beh->GetInputParameterValue(b + 10, &st);
        beh->GetInputParameterValue(b + 11, &sb);
        if (sl)
            sprite->ModifyFlags(CK_2DENTITY_STICKLEFT, 0);
        else
            sprite->ModifyFlags(0, CK_2DENTITY_STICKLEFT);
        if (sr)
            sprite->ModifyFlags(CK_2DENTITY_STICKRIGHT, 0);
        else
            sprite->ModifyFlags(0, CK_2DENTITY_STICKRIGHT);
        if (st)
            sprite->ModifyFlags(CK_2DENTITY_STICKTOP, 0);
        else
            sprite->ModifyFlags(0, CK_2DENTITY_STICKTOP);
        if (sb)
            sprite->ModifyFlags(CK_2DENTITY_STICKBOTTOM, 0);
        else
            sprite->ModifyFlags(0, CK_2DENTITY_STICKBOTTOM);
    }
    else
        b -= 4;

    //---
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}
