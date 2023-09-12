//////////////////////////////////
//////////////////////////////////
//
//        TT_SplitString
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <string.h>

CKObjectDeclaration *FillBehaviorSplitStringDecl();
CKERROR CreateSplitStringProto(CKBehaviorPrototype **pproto);
int SplitString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSplitStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SplitString");
    od->SetDescription("Split a string into Sub-Elements.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ae74e2f, 0x799c49b5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSplitStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSplitStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SplitString");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Delimiter", CKPGUID_STRING);

    proto->DeclareOutParameter("Elements Found", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SplitString);

    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS);

    *pproto = proto;
    return CK_OK;
}

int SplitString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKSTRING text = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!text)
        return CKBR_OK;

    const char *str = CKStrdup(text);
    const char *delim = (const char *)beh->GetInputParameterReadDataPtr(1);
    if (!delim || *delim == '\0')
        delim = " ";

    size_t delimLen = strlen(delim);
    int elements = 0;

    char buf[512];
    for (const char *p = strstr(str, delim); p != NULL; p = strstr(&p[delimLen], delim))
    {
        if (p != str)
        {
            CKParameterOut *pout = beh->GetOutputParameter(++elements);
            if (!pout)
                break;
            size_t sz = p - str;
            if (sz != 0)
            {
                memcpy(buf, str, sz);
                buf[sz] = '\0';
                pout->SetStringValue(buf);
            }
        }
        str = &p[delimLen];
    }

    size_t remains = strlen(str);
    if (remains != 0)
    {
        CKParameterOut *pout = beh->GetOutputParameter(++elements);
        if (pout)
        {
            memcpy(buf, str, remains);
            buf[remains] = '\0';
            pout->SetStringValue(buf);
        }
    }

    beh->SetOutputParameterValue(0, &elements);
    return CKBR_OK;
}