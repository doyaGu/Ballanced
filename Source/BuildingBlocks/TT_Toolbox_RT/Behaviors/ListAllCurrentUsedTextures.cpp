/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_ListAllCurrentUsedTextures
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorListAllCurrentUsedTexturesDecl();
CKERROR CreateListAllCurrentUsedTexturesProto(CKBehaviorPrototype **pproto);
int ListAllCurrentUsedTextures(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListAllCurrentUsedTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ListAllCurrentUsedTextures");
    od->SetDescription("List All Current Used Textures in a DataArray");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6b6d781e, 0x4b531860));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListAllCurrentUsedTexturesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListAllCurrentUsedTexturesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ListAllCurrentUsedTextures");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->DeclareSetting("OnInVideoMemory", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListAllCurrentUsedTextures);

    *pproto = proto;
    return CK_OK;
}

int ListAllCurrentUsedTextures(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    // Get the data array to store results
    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!array)
    {
        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    // Get setting: only list textures in video memory?
    CKBOOL onlyInVideoMemory = TRUE;
    beh->GetLocalParameterValue(0, &onlyInVideoMemory);

    // Get all textures in the context (CKCID_TEXTURE = 31)
    int textureCount = ctx->GetObjectsCountByClassID(CKCID_TEXTURE);
    CK_ID *textureList = ctx->GetObjectsListByClassID(CKCID_TEXTURE);

    for (int i = 0; i < textureCount; i++)
    {
        CKTexture *texture = (CKTexture *)ctx->GetObject(textureList[i]);
        if (!texture)
            continue;

        // If setting is TRUE, only include textures currently in video memory
        if (onlyInVideoMemory)
        {
            if (!texture->IsInVideoMemory())
                continue;
        }

        // Get the slot count for this texture
        int slotCount = texture->GetSlotCount();
        for (int slot = 0; slot < slotCount; slot++)
        {
            char *slotFileName = texture->GetSlotFileName(slot);
            if (slotFileName && slotFileName[0])
            {
                // Add a new row and set the filename
                array->InsertRow(-1);
                int rowIndex = array->GetRowCount() - 1;
                if (!array->SetElementStringValue(rowIndex, 0, slotFileName))
                {
                    // If setting failed, remove the row
                    array->RemoveRow(rowIndex);
                }
            }
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}