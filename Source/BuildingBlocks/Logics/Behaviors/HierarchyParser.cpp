/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Hierarchy Parser
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHierarchyParserDecl();
CKERROR CreateHierarchyParserProto(CKBehaviorPrototype **);
int HierarchyParser(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHierarchyParserDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Hierarchy Parser");
    od->SetDescription("Iterate recursively on an object and all its children.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Max Recursivity: </SPAN>Maximum depth of childhood covered. 0 means infinite.<BR>
    <BR>
    <SPAN CLASS=pout>Current: </SPAN>Current object parsed.<BR>
    <SPAN CLASS=pout>Recursivity Level: </SPAN>Depth of the current object (0 means the root, 1 a direct child of the root, etc.)<BR>
    <BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5f4a214b, 0x6817452c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHierarchyParserProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateHierarchyParserProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Hierarchy Parser");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Max Depth", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Current", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Depth", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(HierarchyParser);

    *pproto = proto;
    return CK_OK;
}

int HierarchyParser(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int rlevel = 0;

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);

        CK3dEntity *father = (CK3dEntity *)beh->GetTarget();
        if (father)
        {
            if (father->GetClassID() == CKCID_CHARACTER)
                father = ((CKCharacter *)father)->GetRootBodyPart();
            beh->SetOutputParameterObject(0, father);
            beh->SetOutputParameterValue(1, &rlevel);
            beh->ActivateOutput(1);
        }
        else
        {
            beh->ActivateOutput(0);
        }
        return CKBR_OK;
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(1, &rlevel);
    }

    CK3dEntity *current = (CK3dEntity *)beh->GetOutputParameterObject(0);
    CK3dEntity *next = NULL;

    int maxr = 0;
    beh->GetInputParameterValue(0, &maxr);

    if ((!maxr || (rlevel < (maxr - 1))) && current->GetChildrenCount()) // Current has child
    {
        next = current->GetChild(0);
        ++rlevel;
    }
    else
    {

        CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
        if (target)
        {
            if (target->GetClassID() == CKCID_CHARACTER)
                target = ((CKCharacter *)target)->GetRootBodyPart();
        }

        if (current != target) // we do not go upper the target where we begin
        {
            CK3dEntity *father;
            while (father = current->GetParent())
            {
                int fccount = father->GetChildrenCount();
                int i = 0;
                while (father->GetChild(i++) != current)
                    ;
                if (i == fccount) // no more brother on this level... looking for my father's brother
                {
                    --rlevel;
                    if (rlevel == 0)
                        break;
                    current = father;
                }
                else // brother found
                {
                    next = father->GetChild(i);
                    break;
                }
            }
        }
    }

    if (next)
    {
        beh->SetOutputParameterObject(0, next);
        beh->SetOutputParameterValue(1, &rlevel);
        beh->ActivateOutput(1);
    }
    else
    {
        beh->ActivateOutput(0);
    }

    return CKBR_OK;
}