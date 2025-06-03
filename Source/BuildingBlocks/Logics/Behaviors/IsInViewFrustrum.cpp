/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//               Is In View Frustrum
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorIsInViewFrustrumDecl();
CKERROR CreateIsInViewFrustrumProto(CKBehaviorPrototype **);
int IsInViewFrustrum(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsInViewFrustrumDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Is In View Frustum");
    od->SetDescription("Tests if the object is in the viewing frustrum of the active camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Inside: </SPAN>is activated if the bounding box of the object is in the view frustrum.<BR>
    <SPAN CLASS=out>Outside: </SPAN>is activated if the bounding box of the object is not in the view frustrum.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>the entity used for the test.<BR>
    <SPAN CLASS=pin>Test Hierarchy: </SPAN>if this boolean is set to true, all the children of the object will be tested too.<BR>
    <BR>
    <SPAN CLASS=pout>Extents: </SPAN>rectangle containing the screen extents of the object.<BR>
    <BR>
    The viewing frustum is a 3D Volume in a scene positionned relatively to the viewport's camera. For perspective viewing, the camera is positioned at the tip of an imaginary pyramid.
    This pyramid is intersected by two clipping planes, the front plane and the far plane.
    The Volume in the pyramid between the front and the back clipping planes is the viewing frustrum. Only objects in the viewing frustum are visible.<BR>
    */
    od->SetCategory("Logics/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45542a7f, 0x4c2f01a6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00030000);
    od->SetCreationFunction(CreateIsInViewFrustrumProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsInViewFrustrumProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Is In View Frustum");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Inside");
    proto->DeclareOutput("Outside");

    proto->DeclareInParameter("Object", CKPGUID_RENDEROBJECT);
    proto->DeclareInParameter("Test Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Extents", CKPGUID_RECT);
    proto->DeclareOutParameter("Fully Inside", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsInViewFrustrum);

    *pproto = proto;
    return CK_OK;
}

int IsInViewFrustrum(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CKRenderObject *ro = (CKRenderObject *)beh->GetInputParameterObject(0);

    // Test also hierarchy
    CKBOOL hierarchy = TRUE;
    beh->GetInputParameterValue(1, &hierarchy);

    CKRenderContext *rc = behcontext.CurrentRenderContext;

    CKBOOL inside = FALSE;
    CKBOOL allInside = FALSE;
    VxRect extents;

    /////////////////////////////
    // 3D ENTITY
    if (CKIsChildClassOf(ro, CKCID_3DENTITY))
    {
        CK3dEntity *ent = (CK3dEntity *)ro;

        // Test frustrum
        if (hierarchy)
            inside = ent->IsInViewFrustrumHierarchic(rc);
        else
            inside = ent->IsInViewFrustrum(rc);

        // All inside ?
        allInside = ent->IsAllInsideFrustrum();

        // Get extents
        ent->GetRenderExtents(extents);
    }
    else
    {
        /////////////////////////////
        // 2D ENTITY
        if (CKIsChildClassOf(ro, CKCID_2DENTITY))
        {
            CK2dEntity *ent = (CK2dEntity *)ro;

            ent->GetRect(extents);

            VxRect screen;
            rc->GetViewRect(screen);

            int res = extents.IsInside(screen);

            if (res != ALLOUTSIDE)
            {
                inside = TRUE;
                allInside = (res == ALLINSIDE);
            }
        }
        else
        { // No supported type (or no object)
            return CKBR_OK;
        }
    }

    if (beh->GetVersion() == 0x00030000)
    { // new version with the boolean all inside
        beh->SetOutputParameterValue(1, &allInside);
    }

    // Inside
    if (inside)
    {

        if (beh->GetOutputParameter(0)->GetGUID() == CKPGUID_RECT)
        {
            beh->SetOutputParameterValue(0, &extents);
        }
        else
        {
            beh->SetOutputParameterValue(0, &extents.GetTopLeft());
            beh->SetOutputParameterValue(1, &extents.GetBottomRight());
        }

        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    // Outside
    beh->ActivateOutput(1);
    return CKBR_OK;
}
