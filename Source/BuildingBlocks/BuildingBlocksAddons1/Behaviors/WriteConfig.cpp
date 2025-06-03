/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              WriteConfig
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorWriteConfigDecl();
CKERROR CreateWriteConfigProto(CKBehaviorPrototype **);
int WriteConfig(const CKBehaviorContext &behcontext);
CKERROR WriteConfigCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWriteConfigDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Write Config");
    od->SetDescription("Write Data in a user specific storage place.");
    /* rem:
     */
    od->SetCategory("Narratives/Config");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a797d94, 0x21ab5cfa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWriteConfigProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWriteConfigProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Write Config");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Section", CKPGUID_STRING, "MyGame\\HighScores");
    proto->DeclareInParameter("Entry", CKPGUID_STRING, "John");
    proto->DeclareInParameter("Data", CKPGUID_INT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WriteConfig);

    *pproto = proto;
    return CK_OK;
}

enum
{
    SECTION,
    ENTRY,
    DATA
};

int WriteConfig(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // IO Activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKSTRING section = (CKSTRING)beh->GetInputParameterReadDataPtr(SECTION);
    CKSTRING entry = (CKSTRING)beh->GetInputParameterReadDataPtr(ENTRY);

    CKParameterIn *pin = beh->GetInputParameter(DATA);
    CKParameter *p = pin->GetRealSource();

    char buffer[256] = {0};

    if (p)
    {
        p->GetStringValue(buffer);
    }
    else
    {
        buffer[0] = '\0';
    }
    VxConfig cfg;

    cfg.OpenSection(section, VxConfig::WRITE);

    cfg.WriteStringEntry(entry, buffer);

    cfg.CloseSection(section);

    return CKBR_OK;
}