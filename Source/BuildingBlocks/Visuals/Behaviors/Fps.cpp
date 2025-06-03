/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Fps
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFpsDecl();
CKERROR CreateFpsProto(CKBehaviorPrototype **pproto);
int Fps(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFpsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("FPS");
    od->SetDescription("Writes in the output the FPS (Frames Per Second).");
    /* rem:
    In : triggers the process.<BR>
    Out : is activated when the process is completed.<BR>
    <BR>
    Fps : number of frames par second.<BR>
    This behavior can be used with the "Text Display" behavior to display the current frame rate.
    */
    /* warning:
    - This building block is obsolete : use Statistics instead.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xa58a313a, 0xe7f8d32a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFpsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Visuals/Text");
    return od;
}

CKERROR CreateFpsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("FPS");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Fps", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(Fps);

    *pproto = proto;
    return CK_OK;
}

int Fps(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    float time = 1000.0f / (float)behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &time);

    return CKBR_OK;
}
