# TT_ParticleSystems_RT

add_library(TT_ParticleSystems_RT SHARED TT_ParticleSystems_RT.cpp)
set(TT_PARTICLESYSTEMS_RT_HEAHERS
        ParticleGuids.h
        Particle.h
        ParticleTools.h
        ParticleManager.h
		
		ParticleEmitter.h
		PointEmitter.h
		LineEmitter.h
		PlanarEmitter.h
		CubicEmitter.h
		DiscEmitter.h
		ObjectEmitter.h
		CurveEmitter.h
		CylindricalEmitter.h
		SphericalEmitter.h
		TimePointEmitter.h
		WaveEmitter.h
		
        ParticleSystemRenderCallbacks.h
		GeneralParticleSystem.h

)
set(TT_PARTICLESYSTEMS_RT_SOURCES
        Particle.cpp
        ParticleTools.cpp
        ParticleManager.cpp
		
		ParticleEmitter.cpp
		PointEmitter.cpp
		LineEmitter.cpp
		PlanarEmitter.cpp
		CubicEmitter.cpp
		DiscEmitter.cpp
		ObjectEmitter.cpp
		CurveEmitter.cpp
		CylindricalEmitter.cpp
		SphericalEmitter.cpp
		TimePointEmitter.cpp
		WaveEmitter.cpp
		
        ParticleSystemRenderCallbacks.cpp
        ParticleSystemDeflectors.cpp
        ParticleSystemInteractors.cpp
        ParticleSystemMeshes.cpp
	
		GeneralParticleSystem.cpp
		PointParticleSystem.cpp
		LinearParticleSystem.cpp
		PlanarParticleSystem.cpp
		CubicParticleSystem.cpp
		DiscParticleSystem.cpp
		ObjectParticleSystem.cpp
		CurveParticleSystem.cpp
		CylindricalParticleSystem.cpp
		SphericalParticleSystem.cpp
		TimeDependentPointParticleSystem.cpp
		WaveParticleSystem.cpp
        )
target_sources(TT_ParticleSystems_RT PRIVATE
    ${TT_PARTICLESYSTEMS_RT_SOURCES}
    ${TT_PARTICLESYSTEMS_RT_HEAHERS}
    )
target_include_directories(TT_ParticleSystems_RT PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
target_link_libraries(TT_ParticleSystems_RT PRIVATE CK2 VxMath)

install(TARGETS TT_ParticleSystems_RT RUNTIME DESTINATION BuildingBlocks)