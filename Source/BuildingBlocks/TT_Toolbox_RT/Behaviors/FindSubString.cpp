/////////////////////////////////////
/////////////////////////////////////
//
//        TT Find SubString
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFindSubStringDecl();
CKERROR CreateFindSubStringProto(CKBehaviorPrototype **pproto);
int FindSubString(const CKBehaviorContext &behcontext);
CKERROR FindSubStringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFindSubStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Find SubString");
    od->SetDescription("Searches for strings in object names");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12f31e27, 0x4cea0bf5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFindSubStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateFindSubStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Find SubString");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In0");

    proto->DeclareOutput("None");
    proto->DeclareOutput("StringOut 1");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("String 1", CKPGUID_STRING);

    proto->DeclareOutParameter("String ID", CKPGUID_INT);

    proto->DeclareSetting("String Count", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FindSubString);

    proto->SetBehaviorCallbackFct(FindSubStringCallBack);

    *pproto = proto;
    return CK_OK;
}

static CKBOOL FindSubStringInName(const char *name, const char *substr)
{
    if (!name || !substr)
        return FALSE;
    return strstr(name, substr) != NULL;
}

int FindSubString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CKObject *obj = beh->GetInputParameterObject(0);
    CKSTRING name = obj ? obj->GetName() : NULL;

    int inputCount = beh->GetInputParameterCount();
    for (int i = 1; i < inputCount; i++)
    {
        const char *str = (const char *)beh->GetInputParameterReadDataPtr(i);
        if (FindSubStringInName(name, str))
        {
            beh->ActivateOutput(i, TRUE);
            beh->SetOutputParameterValue(0, &i);
            return CKBR_OK;
        }
    }

    int zero = 0;
    beh->ActivateOutput(0, TRUE);
    beh->SetOutputParameterValue(0, &zero);
    return CKBR_OK;
}

CKERROR FindSubStringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CallbackMessage == CKM_BEHAVIORSETTINGSEDITED)
    {
        int stringCount = 1;
        beh->GetLocalParameterValue(0, &stringCount);

        // Adjust input parameters
        while (beh->GetInputParameterCount() > stringCount + 1)
            beh->RemoveInputParameter(beh->GetInputParameterCount() - 1);

        while (beh->GetInputParameterCount() < stringCount + 1)
        {
            int idx = beh->GetInputParameterCount();
            char name[64];
            sprintf(name, "String %d", idx);
            beh->CreateInputParameter(name, CKPGUID_STRING);
        }

        // Adjust output parameters
        while (beh->GetOutputCount() > stringCount + 1)
            beh->DeleteOutput(beh->GetOutputCount() - 1);

        while (beh->GetOutputCount() < stringCount + 1)
        {
            int idx = beh->GetOutputCount();
            char name[64];
            sprintf(name, "StringOut %d", idx);
            beh->AddOutput(name);
        }
    }

    return CKBR_OK;
}