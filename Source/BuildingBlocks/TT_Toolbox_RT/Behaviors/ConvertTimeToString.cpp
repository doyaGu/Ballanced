//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT ConvertTimeToString
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorConvertTimeToStringDecl();
CKERROR CreateConvertTimeToStringProto(CKBehaviorPrototype **pproto);
int ConvertTimeToString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorConvertTimeToStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ConvertTimeToString");
    od->SetDescription("Converts time to string of the form 00:00:00");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x69e34443, 0x459f23db));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateConvertTimeToStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateConvertTimeToStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ConvertTimeToString");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->DeclareOutParameter("String Time", CKPGUID_STRING, "00:00:00");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ConvertTimeToString);

    *pproto = proto;
    return CK_OK;
}

int ConvertTimeToString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    float time;
    beh->GetInputParameterValue(0, &time);

    char str[9];
    if (time <= 0.0f || time >= 3600000.0f)
    {
        strcpy(str, "00:00:00");
    }
    else
    {
        int t = (int)time;
        int ms = (t % 1000) / 10;
        int sec = ((t - t % 1000) / 1000) % 60;
        int min = t / (60 * 1000);
        if (min > 59)
            min = 0;

        str[0] = (char)('0' + (min / 10));
        str[1] = (char)('0' + (min % 10));
        str[2] = ':';
        str[3] = (char)('0' + (sec / 10));
        str[4] = (char)('0' + (sec % 10));
        str[5] = ':';
        str[6] = (char)('0' + (ms / 10));
        str[7] = (char)('0' + (ms % 10));
    }

    beh->SetOutputParameterValue(0, str);
    return CKBR_OK;
}