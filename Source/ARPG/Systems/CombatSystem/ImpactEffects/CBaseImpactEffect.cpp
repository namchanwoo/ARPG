

#include "CBaseImpactEffect.h"



UCBaseImpactEffect::UCBaseImpactEffect()
{
	
	ImpactEffectIndexType = EHitReactIndexType::CustomIndex;
	ImpactEmitterSpawnType = EImpactEmitterSpawnType::AtLocation;

	
	CameraShakeIndexType = EHitReactIndexType::CustomIndex;
	
	CameraShakeScale.Add(1.0f);
	CameraShakeScale.Add(1.25f);
	CameraShakeScale.Add(1.5f);
	CameraShakeScale.Add(2.0f);
	
}
