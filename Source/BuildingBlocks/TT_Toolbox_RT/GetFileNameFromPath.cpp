//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_GetFileNameFromPath
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetFileNameFromPathDecl();
CKERROR CreateGetFileNameFromPathProto(CKBehaviorPrototype **pproto);
int GetFileNameFromPath(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetFileNameFromPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetFileNameFromPath");
    od->SetDescription("Get File Name From Path");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x68e631f7, 0x13f914fe));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetFileNameFromPathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetFileNameFromPathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetFileNameFromPath");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");

    proto->DeclareInParameter("Path+Name", CKPGUID_STRING);

    proto->DeclareOutParameter("FileName", CKPGUID_STRING);

    proto->DeclareSetting("Use Slash '/'", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetFileNameFromPath);

    *pproto = proto;
    return CK_OK;
}

int GetFileNameFromPath(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSTRING path = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    CKBOOL useSlash = FALSE;
    beh->GetLocalParameterValue(0, &useSlash);
    char sep = (useSlash) ? '/' : '\\';
    char *filename = strrchr(path, sep);
    if (filename)
        beh->SetOutputParameterValue(0, filename + 1);
    else
        beh->SetOutputParameterValue(0, path);

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}