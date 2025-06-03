/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MoviePlayer
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMoviePlayerDecl();
CKERROR CreateMoviePlayerProto(CKBehaviorPrototype **);
int MoviePlayer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMoviePlayerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Movie Player");
    od->SetDescription("Plays a Movie (avi)");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Synchro Out: </SPAN>activated each time a slot changes.<BR>
    <SPAN CLASS=out>One Loop Played: </SPAN>is activated once the animation has been performed.<BR>
    <BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>Loop: </SPAN>tells wether or not the movie has to loop.<BR>
    <SPAN CLASS=pin>Starting Slot: </SPAN>the starting frame of the movie.<BR>
    <SPAN CLASS=pin>Ending Slot: </SPAN>the ending frame of the movie ( this frame will not be played !!! )<BR>
    <BR>
    <SPAN CLASS=pout>Current Slot: </SPAN>the current frame being played.<BR>
    <BR>
    By default, 'Starting Slot'=0 and 'Ending Slot'=0, which means the movie has to be played from front to end.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x778d16d4, 0x1dd60060));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMoviePlayerProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    od->SetCategory("Materials-Textures/Animation");
    return od;
}

CKERROR CreateMoviePlayerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Movie Player");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Synchro Out");
    proto->DeclareOutput("One Loop Played");

    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 5s 0ms");
    proto->DeclareInParameter("Loop", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Starting Slot", CKPGUID_INT, "0");
    proto->DeclareInParameter("Ending Slot", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Current Slot", CKPGUID_INT);

    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MoviePlayer);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int MoviePlayer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // if we enter by OFF
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;

    float current_time;
    int old_slot;

    // we get the Duration
    float fduration = 5000.0f;
    CKParameterIn *pin = beh->GetInputParameter(0);
    if (pin && pin->GetGUID() == CKPGUID_INT) // back compatibility purpose
    {
        int duration = 5000;
        pin->GetValue(&duration);
        fduration = (float)duration;
    }
    else
    {
        beh->GetInputParameterValue(0, &fduration);
    }

    if (fduration == 0.0f)
        return CKBR_OK;

    int total_slot_capacity = tex->GetSlotCount();
    if (!total_slot_capacity)
        return CKBR_OK;

    int start_slot = 0;
    beh->GetInputParameterValue(2, &start_slot);
    if (start_slot < 0)
        start_slot = 0;
    if (start_slot > total_slot_capacity - 1)
        start_slot = total_slot_capacity;

    int end_slot = 0;
    beh->GetInputParameterValue(3, &end_slot);
    if (end_slot < 0)
        end_slot = 0;
    if (end_slot > total_slot_capacity - 1)
        end_slot = total_slot_capacity;

    // the number of slots to be played is equal to the total slot capacity of the texture, only if...
    int slot_count;
    if ((start_slot == 0) && (end_slot == 0))
    {
        slot_count = tex->GetSlotCount();
    }
    else
    {
        slot_count = abs(end_slot - start_slot);
    }

    float delta_to_change_frame = (float)slot_count / fduration;

    if (beh->IsInputActive(0)) // it's the first time we activate the behavior
    {
        beh->ActivateInput(0, FALSE);
        old_slot = -1;
        current_time = 0;
    }
    else
    {
        beh->GetLocalParameterValue(0, &current_time);
        old_slot = (int)(current_time * delta_to_change_frame);
        current_time += behcontext.DeltaTime;
    }

    if (current_time >= fduration) // If we exceed the total duration
    {
        beh->ActivateOutput(1, TRUE); // activation of the 'One Loop Played' output

        // we get the Looping Parameter
        CKBOOL b = TRUE;
        beh->GetInputParameterValue(1, &b);

        if (b) // if we ask to loop then loop
        {
            do
                current_time -= fduration;
            while (current_time >= fduration);
        }
        else // if we ask not to loop then stop playing
        {
            return CKBR_OK;
        }
    }

    beh->SetLocalParameterValue(0, &current_time);

    int new_slot = (int)(current_time * delta_to_change_frame);

    if (new_slot != old_slot) // we change the slot only if we need to.
    {
        int real_current_slot;

        if (start_slot <= end_slot)
            real_current_slot = start_slot + new_slot;
        else
            real_current_slot = start_slot - 1 - new_slot;

        tex->SetCurrentSlot(real_current_slot);
        beh->SetOutputParameterValue(0, &real_current_slot);
        beh->ActivateOutput(0, TRUE); // activation of the synchro output
    }

    return CKBR_ACTIVATENEXTFRAME;
}
