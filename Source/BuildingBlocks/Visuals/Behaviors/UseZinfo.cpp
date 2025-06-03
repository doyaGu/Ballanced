/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		     Use Z information for render
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorUseZDecl();
CKERROR CreateUseZProto(CKBehaviorPrototype **pproto);
CKERROR UseZCallBackObject(const CKBehaviorContext &behcontext);
void ZBufOnCallbackFunc(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);

CKObjectDeclaration *FillBehaviorUseZDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Use Z information");
    od->SetDescription("Hides the object but uses its Z information during rendering");
    /* rem:
    <SPAN CLASS=pin>Clear Background : </SPAN>Sprite to be displayed on the background.<BR>
    <BR>
    This behavior has neither 'Inputs' nor 'Outputs'. It only changes the rendering mode of a 3D Entity that
    has the ZInfo attribute.<BR>
    The Object is then totally translucent, but it is always displayed in the Z-Buffer.<BR>
    You Should use this behavior when rendering a scene with a fixed camera, and a complex 3d graphics image on the background.
    Then apply this behavior on a fixed object which represents the outline of a background part (drawing of some complex object).
    A character (or anything else) will then be able to get behind this object just like if it really exists.<BR>
    */
    /* warning:
    - You should apply only one "Use Z Information" BB to per level (or scene).<BR>
    - You can write into Z Buffer only by changing the properties of a material (see Material Setup),
    But this building block is usefull because you can to it to whole object (ie to all its material at the same time),
    and it does not alter the material properties of other object that use the same material.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1f60adc, 0x39b02cb8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateUseZProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

void ZBufOnCallbackFunc(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg)
{
    CK3dEntity *ent = (CK3dEntity *)obj;
    if (!CKIsChildClassOf(ent, CKCID_3DENTITY))
        return;
    if (Set)
    {
        ent->ModifyMoveableFlags(VX_MOVEABLE_ZBUFONLY | VX_MOVEABLE_RENDERFIRST, 0);
    }
    else
    {
        ent->ModifyMoveableFlags(0, VX_MOVEABLE_ZBUFONLY | VX_MOVEABLE_RENDERFIRST);
    }
}

CKERROR CreateUseZProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Use Z information");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInParameter("Clear Background", CKPGUID_BOOL, "TRUE");

    proto->DeclareLocalParameter("Attribute", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetBehaviorCallbackFct(UseZCallBackObject);

    *pproto = proto;
    return CK_OK;
}

CKERROR
UseZCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    CKLevel *level = behcontext.CurrentLevel;
    CKScene *scene = behcontext.CurrentScene;

    CKBeObject *ent = (CKBeObject *)beh->GetOwner();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORLOAD:
    {
        CKAttributeManager *attman = behcontext.AttributeManager;
        int att = attman->GetAttributeTypeByName("ZBuffer Only");
        const XObjectPointerArray &Array = attman->GetGlobalAttributeListPtr(att);
        for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
        {
            // we get the globalWind
            CK3dEntity *ent = (CK3dEntity *)*it;
            if (!ent)
                continue;
            ent->ModifyMoveableFlags(VX_MOVEABLE_ZBUFONLY | VX_MOVEABLE_RENDERFIRST, 0);
        }
    }
    case CKM_BEHAVIOREDITED:
    {
        CKBOOL ClearBackground = FALSE;
        beh->GetInputParameterValue(0, &ClearBackground);
        if (rcontext)
            rcontext->SetClearBackground(ClearBackground);
    }
    break;
    }
    return CKBR_OK;
}
