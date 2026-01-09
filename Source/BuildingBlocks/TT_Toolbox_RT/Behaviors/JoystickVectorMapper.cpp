///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT_JoystickVectorMapper
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorJoystickVectorMapperDecl();
CKERROR CreateJoystickVectorMapperProto(CKBehaviorPrototype **pproto);
int JoystickVectorMapper(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorJoystickVectorMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_JoystickVectorMapper");
    od->SetDescription("TT_JoystickVectorMapper");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f80efd, 0x76b47b33));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateJoystickVectorMapperProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateJoystickVectorMapperProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_JoystickVectorMapper");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Do");

    proto->DeclareOutput("Done");

    proto->DeclareInParameter("InVector", CKPGUID_VECTOR);
    proto->DeclareInParameter("JX+", CKPGUID_INT);
    proto->DeclareInParameter("JX-", CKPGUID_INT);
    proto->DeclareInParameter("JY+", CKPGUID_INT);
    proto->DeclareInParameter("JY-", CKPGUID_INT);

    proto->DeclareOutParameter("OutVector", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(JoystickVectorMapper);

    *pproto = proto;
    return CK_OK;
}

int JoystickVectorMapper(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxVector inVector(0.0f, 0.0f, 0.0f);
    VxVector outVector(0.0f, 0.0f, 0.0f);
    int unused = 0;

    beh->GetInputParameterValue(0, &inVector);

    int jxPlus = 0;
    beh->GetInputParameterValue(1, &jxPlus);

    int jxMinus = 0;
    beh->GetInputParameterValue(2, &jxMinus);

    int jyPlus = 0;
    beh->GetInputParameterValue(3, &jyPlus);

    int jyMinus = 0;
    beh->GetInputParameterValue(4, &jyMinus);

    // Map JX+ to output X positive
    switch (jxPlus)
    {
    case 0: // X+
        if (inVector.x >= 0.0f)
            outVector.x = inVector.x;
        break;
    case 1: // X-
        if (inVector.x < 0.0f)
            outVector.x = -inVector.x;
        break;
    case 2: // Y+
        if (inVector.y >= 0.0f)
            outVector.x = inVector.y;
        break;
    case 3: // Y-
        if (inVector.y < 0.0f)
            outVector.x = -inVector.y;
        break;
    case 4: // Z+
        if (inVector.z >= 0.0f)
            outVector.x = inVector.z;
        break;
    case 5: // Z-
        if (inVector.z < 0.0f)
            outVector.x = -inVector.z;
        break;
    }

    // Map JX- to output X negative
    switch (jxMinus)
    {
    case 0: // X+
        if (inVector.x >= 0.0f)
            outVector.x = -inVector.x;
        break;
    case 1: // X-
        if (inVector.x < 0.0f)
            outVector.x = inVector.x;
        break;
    case 2: // Y+
        if (inVector.y >= 0.0f)
            outVector.x = -inVector.y;
        break;
    case 3: // Y-
        if (inVector.y < 0.0f)
            outVector.x = inVector.y;
        break;
    case 4: // Z+
        if (inVector.z >= 0.0f)
            outVector.x = -inVector.z;
        break;
    case 5: // Z-
        if (inVector.z < 0.0f)
            outVector.x = inVector.z;
        break;
    }

    // Map JY+ to output Y positive
    switch (jyPlus)
    {
    case 0: // X+
        if (inVector.x >= 0.0f)
            outVector.y = inVector.x;
        break;
    case 1: // X-
        if (inVector.x < 0.0f)
            outVector.y = -inVector.x;
        break;
    case 2: // Y+
        if (inVector.y >= 0.0f)
            outVector.y = inVector.y;
        break;
    case 3: // Y-
        if (inVector.y < 0.0f)
            outVector.y = -inVector.y;
        break;
    case 4: // Z+
        if (inVector.z >= 0.0f)
            outVector.y = inVector.z;
        break;
    case 5: // Z-
        if (inVector.z < 0.0f)
            outVector.y = -inVector.z;
        break;
    }

    // Map JY- to output Y negative
    switch (jyMinus)
    {
    case 0: // X+
        if (inVector.x >= 0.0f)
            outVector.y = -inVector.x;
        break;
    case 1: // X-
        if (inVector.x < 0.0f)
            outVector.y = inVector.x;
        break;
    case 2: // Y+
        if (inVector.y >= 0.0f)
            outVector.y = -inVector.y;
        break;
    case 3: // Y-
        if (inVector.y < 0.0f)
            outVector.y = inVector.y;
        break;
    case 4: // Z+
        if (inVector.z >= 0.0f)
            outVector.y = -inVector.z;
        break;
    case 5: // Z-
        if (inVector.z < 0.0f)
            outVector.y = inVector.z;
        break;
    }

    beh->SetOutputParameterValue(0, &outVector);
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}