/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              ReadConfig
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorReadConfigDecl();
CKERROR CreateReadConfigProto(CKBehaviorPrototype **);
int ReadConfig(const CKBehaviorContext &behcontext);
CKERROR ReadConfigCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReadConfigDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Read Config");
    od->SetDescription("Read Data in a user specific storage place.");
    /* rem:
     */
    od->SetCategory("Narratives/Config");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a888d94, 0x21ccccfa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReadConfigProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReadConfigProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Read Config");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Entry Not Present");

    proto->DeclareInParameter("Section", CKPGUID_STRING, "MyGame\\HighScores");
    proto->DeclareInParameter("Entry", CKPGUID_STRING, "John");

    proto->DeclareOutParameter("Data", CKPGUID_INT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReadConfig);

    *pproto = proto;
    return CK_OK;
}

enum { SECTION, ENTRY, };

enum { DATA };

int ReadConfig(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // IO Activation
    beh->ActivateInput(0, FALSE);

    CKSTRING section = (CKSTRING)beh->GetInputParameterReadDataPtr(SECTION);

    CKSTRING entry = (CKSTRING)beh->GetInputParameterReadDataPtr(ENTRY);

    char buffer[256] = {0};
    buffer[0] = '\0';

    VxConfig cfg;

    cfg.OpenSection(section, VxConfig::READ);

    int len = cfg.ReadStringEntry(entry, buffer);

    cfg.CloseSection(section);

    if (len < 0)
    {
        beh->ActivateOutput(1);
    }
    else
    {
        beh->ActivateOutput(0);
    }

    CKParameter *p = beh->GetOutputParameter(DATA);
    if (p)
    {
        p->SetStringValue(buffer);
    }
    return CKBR_OK;
}
