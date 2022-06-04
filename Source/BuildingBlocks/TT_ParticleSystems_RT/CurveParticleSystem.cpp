/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CurveParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_ParticleSystems_RT.h"

#include "CurveEmitter.h"
#include "DiscEmitter.h"
#include "LineEmitter.h"
#include "PlanarEmitter.h"
#include "PointEmitter.h"

#include "GeneralParticleSystem.h"
#include "ParticleGuids.h"

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
    od->SetCategory("Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from a curve, in the Y local direction of it.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
