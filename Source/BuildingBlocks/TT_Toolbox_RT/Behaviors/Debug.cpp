////////////////////////////
////////////////////////////
//
//        TT_Debug
//
////////////////////////////
////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "DebugManager.h"

CKObjectDeclaration *FillBehaviorDebugDecl();
CKERROR CreateDebugProto(CKBehaviorPrototype **pproto);
int Debug(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDebugDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Debug");
    od->SetDescription("Debug");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a446c43, 0x66fa2375));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDebugProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDebugProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Debug");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("DebugText", CKPGUID_STRING);
    proto->DeclareInParameter("Console", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Log-File", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Enable", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Debug);

    *pproto = proto;
    return CK_OK;
}

void BuildBehaviorLocationPath(char *buf, CKBehavior *beh, CKBehavior *ownerScript)
{
    char v5[256];
    char v6[256];

    CKBehavior *parent = beh->GetParent();
    strcpy(v6, parent->GetName());
    while (parent != ownerScript)
    {
        strcpy(v5, "/");
        strcat(v5, v6);
        strcpy(v6, v5);
        parent = parent->GetParent();
        strcpy(v5, parent->GetName());
        strcat(v5, v6);
        strcpy(v6, v5);
    }
    strcpy(buf, v6);
}

int Debug(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKBOOL enable;
    beh->GetInputParameterValue(3, &enable);

    if (enable)
    {
        DebugManager *dm = DebugManager::GetManager(context);
        if (dm && dm->IsInDebugMode())
        {
            CKSTRING text = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
            CKBOOL console;
            beh->GetInputParameterValue(1, &console);
            CKBOOL logfile;
            beh->GetInputParameterValue(2, &logfile);

            CKBehavior *ownerScript = beh->GetOwnerScript();
            CKBeObject *owner = beh->GetOwner();
            char buf[256];
            BuildBehaviorLocationPath(buf, beh, ownerScript);

            if (console)
            {
                context->OutputToConsoleEx("Debug Output Behavior \n Location \t: %s \n Owner \t\t: %s \n Comment \t: %s",
                                           buf, owner->GetName(), text);
            }

            if (logfile)
            {
                dm->WriteToLogFile("Debug.log", "Debug Output Behavior \n Location \t: %s \n Owner \t\t: %s \n Comment \t: %s\n",
                                   buf, owner->GetName(), text);
            }

            beh->ActivateInput(0, FALSE);
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}