/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            HardwareLevel
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHardwareLevelDecl();
CKERROR CreateHardwareLevelProto(CKBehaviorPrototype **);
int HardwareLevel(const CKBehaviorContext &behcontext);

#define CKPGUID_APIENUM CKDEFINEGUID(0xebe7d72, 0x4a07b9f)

CKObjectDeclaration *FillBehaviorHardwareLevelDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Hardware Level");
    od->SetDescription("Activates output depending on the hardware acceleration level of the current driver");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>TL: </SPAN>is activated when the driver provides Transform and Lightning acceleration.<BR>
    <SPAN CLASS=out>Hardware: </SPAN>is activated when the driver provides hardware acceleration.<BR>
    <SPAN CLASS=out>Software: </SPAN>is activated when the driver provides software acceleration is available.<BR>
    <BR>
    <BR>
    This building block switch the sequential stream to the "Out K" if "Test"="Pin K".<BR>
    <BR>
    You can add as many 'Outputs' as needed (the number of 'Input Parameters' will automatically be equal to the number of 'Inputs').<BR>
    You can change the type of the "Test" input parameter (FLOAT, VECTOR, MESH, COLOR ... anything !).
    The type of all other input parameters will be cast to the type of "Test".<BR>
    This building block is massively used (especially with the "Parameter Selection").<BR>
    */
    /*
    - Do not change the type of another Input Parameter than the "Test" one, because its type will automatically match the type of "Test".<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1b241485, 0x1f572aac));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHardwareLevelProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Optimizations/System");
    return od;
}

CKERROR CreateHardwareLevelProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Hardware Level");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("TL");
    proto->DeclareOutput("Hardware");
    proto->DeclareOutput("Software");

    proto->DeclareOutParameter("Current Api", CKPGUID_APIENUM);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CK_BEHAVIORPROTOTYPE_NORMAL));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(HardwareLevel);

    *pproto = proto;
    return CK_OK;
}

int HardwareLevel(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *rc = NULL;

    rc = behcontext.Context->GetRenderManager()->GetRenderContext(0);
    if (!rc)
        return (CKBR_BEHAVIORERROR);

    int currentdriver;

    currentdriver = rc->GetDriverIndex();

    VxDriverDesc *drdesc;
    drdesc = behcontext.Context->GetRenderManager()->GetRenderDriverDescription(currentdriver);

    int api = 0;
    switch (drdesc->Caps2D.Family)
    {
    case CKRST_DIRECTX:
        if (drdesc->Caps3D.CKRasterizerSpecificCaps & CKRST_SPECIFICCAPS_DX5)
        {
            api = 2;
        }
        else if (drdesc->Caps3D.CKRasterizerSpecificCaps & CKRST_SPECIFICCAPS_DX7)
        {
            api = 3;
        }
        else if (drdesc->Caps3D.CKRasterizerSpecificCaps & CKRST_SPECIFICCAPS_DX8)
        {
            api = 4;
        }
        else
        {
            api = 0;
        }
        break;

    case CKRST_OPENGL:
        api = 1;
        break;

    default:
        api = 0;
        break;
    }

    beh->SetOutputParameterValue(0, &api);

    beh->ActivateInput(0, FALSE);
    if (drdesc->Caps3D.CKRasterizerSpecificCaps & CKRST_SPECIFICCAPS_HARDWARETL)
        beh->ActivateOutput(0);
    if (drdesc->IsHardware)
        beh->ActivateOutput(1);
    else
        beh->ActivateOutput(2);
    return CKBR_OK;
}