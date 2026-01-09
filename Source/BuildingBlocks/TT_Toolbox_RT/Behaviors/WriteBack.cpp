////////////////////////////////
////////////////////////////////
//
//        TT WriteBack
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorWriteBackDecl();
CKERROR CreateWriteBackProto(CKBehaviorPrototype **pproto);
int WriteBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWriteBackDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT WriteBack");
    od->SetDescription("Writes a value to a dynamically substituted parameter.");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78b31127, 0xa875030));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWriteBackProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWriteBackProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT WriteBack");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Off");

    proto->DeclareInParameter("Parameter", CKPGUID_INT, "0");
    proto->DeclareInParameter("Value", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WriteBack);

    proto->SetBehaviorFlags(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS);

    *pproto = proto;
    return CK_OK;
}

int WriteBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKParameterIn *paramIn = beh->GetInputParameter(0);
    CKParameterIn *valueIn = beh->GetInputParameter(1);

    if (paramIn)
    {
        // Treat shared inputs as shortcuts (not directly writable).
        if (!paramIn->GetSharedSource())
        {
            CKParameter *destParam = paramIn->GetDirectSource();
            if (destParam)
            {
                CKParameter *srcParam = valueIn ? valueIn->GetRealSource() : NULL;

                if (srcParam && destParam->IsCompatibleWith(srcParam) && srcParam->IsCompatibleWith(destParam))
                {
                    void *data = srcParam->GetReadDataPtr(TRUE);
                    int dataSize = srcParam->GetDataSize();
                    if (dataSize <= 0)
                        dataSize = destParam->GetDataSize();
                    if (data && dataSize > 0)
                        destParam->SetValue(data, dataSize);
                }
            }
        }
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}
