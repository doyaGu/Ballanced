//////////////////////////////////
//////////////////////////////////
//
//        TT_ReplacePath
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorReplacePathDecl();
CKERROR CreateReplacePathProto(CKBehaviorPrototype **pproto);
int ReplacePath(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReplacePathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReplacePath");
    od->SetDescription("Copy a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x524a6bcb, 0x66f67774));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReplacePathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReplacePathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ReplacePath");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Ok");
    proto->DeclareOutput("Failed");

    proto->DeclareInParameter("Old_relative_Path(\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("New_relative_Path(\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("CurrentWorking_Directory(Drive:\\..\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("PathIDX(0=Texture/1=Data/2=Sound)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReplacePath);

    *pproto = proto;
    return CK_OK;
}

int ReplacePath(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKPathManager *pm = context->GetPathManager();

    CKSTRING oldPath = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    CKSTRING newPath = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
    CKSTRING curDir = (CKSTRING)beh->GetInputParameterReadDataPtr(2);
    int catIdx;
    beh->GetInputParameterValue(3, &catIdx);

    if (catIdx > SOUND_PATH_IDX)
    {
        context->OutputToConsoleExBeep("TT_ReplacePath: invalid PathIDX");
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    char path1[260];
    char path2[260];
    strcpy(path1, curDir);
    strcpy(path2, curDir);
    strcat(path1, oldPath);
    strcat(path2, newPath);

    int pathCount = pm->GetPathCount(catIdx);
    if (pathCount <= 0)
    {
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    XString pathName(260);
    for (int pathIdx = 0; pathIdx < pathCount; ++pathIdx)
    {
        pm->GetPathName(catIdx, pathIdx, pathName);
        if (pathName == path1)
        {
            pathName = path2;
            pm->RenamePath(catIdx, pathIdx, pathName);
            break;
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}