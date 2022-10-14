/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CurveParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ParticleGuids.h"
#include "GeneralParticleSystem.h"

CKObjectDeclaration *FillBehaviorCurveParticleSystemDecl();

CKObjectDeclaration *FillBehaviorCurveParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("CurveParticleSystem");
    od->SetDescription("Emits particles from a curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CURVESYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    od->SetCategory("TT_Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from a curve, in the Y local direction of it.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
