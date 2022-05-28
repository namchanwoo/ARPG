#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Datas/CombatData.h"
#include "CBaseImpactEffect.generated.h"


UCLASS(Blueprintable)
class ARPG_API UCBaseImpactEffect : public UObject
{
	GENERATED_BODY()

public:
	UCBaseImpactEffect();


public:
	/**혼합 이미터를 생성하겠는가?*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	bool bSpawnMixedEmitters;
	
	/**이미터 인덱스*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	int32 EmitterIndex;
	
	/**임팩트 효과 인덱스 타입*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	EHitReactIndexType ImpactEffectIndexType;

	/**임팩트 이미터 스폰 타입*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	EImpactEmitterSpawnType ImpactEmitterSpawnType;
	
	/**임팩트 이미터 공격력 인덱스*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	TMap<EAttackPower, int32> ImpactEmitterAttackPowerIndex;
	
	/*물리적 표면에 따른 임팩트 이미터*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Effect")
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactEffects> ImpactEmitters;
	
	/**물리적 표면에 따른 임팩트 사운드*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact Sound")
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactSound> ImpactSound;


#pragma region                                    Camera Shake Members
public:
	/**카메라 흔들림의 인덱스*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CameraShake")
	int32 CameraShakeIndex;
	
	/**카메라 흔들림 인덱스 타입*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CameraShake")
	EHitReactIndexType CameraShakeIndexType;
	
	/**카메라 흔들림의 어택파워 인덱스*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CameraShake")
	TMap<EAttackPower, int32> CameraShakeAttackPowerIndex;
	
	/**카메라 흔들림 크기*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CameraShake")
	TArray<float> CameraShakeScale;

	/**카메라쉐이크 Class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CameraShake")
	TArray<TSubclassOf<UMatineeCameraShake>> CameraShakeClass;

#pragma endregion                                 Camera Shake Members
	
};
