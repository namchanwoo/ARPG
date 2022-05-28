#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Combat.generated.h"

class UCBaseImpactEffect;

UINTERFACE(MinimalAPI)
class UInterface_Combat : public UInterface
{
	GENERATED_BODY()
};


class ARPG_API IInterface_Combat
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// #pragma region      Charge Attack
	// public:
	// 	/**스트롱차지 어택을 시작한다*/
	// 	virtual void I_BeginStrongChargeAttack(float ChargeAnimPlayRate) = 0;
	//
	/**스트롱 차지어택을 종료합니다*/
	virtual void I_EndStrongChargeAttack() =0;
	//
	// 	/**차지공격양을 가져옵니다*/
	// 	virtual float I_GetStrongChargeAmount() =0;
	// #pragma endregion   Charge Attack
	//
	//
	// #pragma region      Combat State
public:
	/**전투를 활성화 한다*/
	virtual bool I_CombatEnabled() =0;

	// 	/**캐릭터 스테이트를 가져온다*/
	// 	virtual ECharacterState I_GetCharacterState() =0;
	// #pragma endregion   Combat State
	//
	//
	// #pragma region      Finisher Mode
	// public:
	// 	virtual void I_EnterPunisherMode() =0;
	//
	// 	virtual void I_ExitPunisherMode() =0;
	//
	// 	/**실행 거리를 설정한다*/
	// 	virtual void I_SetExecutionDistance(float InExecutionDistance, float InAttackRotationOffset, float InTransitionSpeedMultiplier) =0;
	//
	//
	// 	virtual void I_SetExecutionDistance_Forced(float InExecutionDistance, float InAttackRotationOffset, float InTransitionSpeedMultiplier,
	// 	                                           bool FaceActor) =0;
	// #pragma endregion   Finisher Mode
	//
	//
	// #pragma region      Fatality System
	// public:
	// 	/**실행을 시작한다 피니셔 또는 실행애니메이션 시퀀스를 수행하려는 경우 이 함수를 호출한다*/
	// 	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	// 	void I_StartExecutionTL(AActor* InExecutedActor, bool InFacingActor);
	// 	virtual void I_StartExecutionTL_Implementation(AActor* InExecutedActor, bool InFacingActor) =0;
	//
	// #pragma endregion   Fatality System
	//
	//
	// #pragma region      Blocking
	// public:
	// 	/**퍼펙트 블록을 성공했다면 true 아니라면 false를 반환한다*/
	// 	virtual bool PerfectBlockSuccessful() =0;
	// #pragma endregion   Blocking
	//
	//
	// #pragma region      AI Boss Spline Attack
	// public:
	// 	virtual FTransform GetMeshTransform() =0;
	// #pragma endregion   AI Boss Spline Attack
	//
	// #pragma region      Combat
	// public:
	// 	/**일반 전투 타입을 가져옵니다*/
	// 	virtual EGeneralCombatType GetGeneralCombatType() =0;
	// 	/**전투 데이터테이블을 가져온다*/
	// 	virtual UDataTable* GetCombatDataTable() =0;
	//
	// #pragma endregion   Combat
	//
	//
	// #pragma region      Stealth System
	// public:
	// 	/**은신을 감지합니다*/
	// 	virtual bool CheckStealth(AActor* InCheckingWith) =0;
	// #pragma endregion   Stealth System
	//
	//
	// #pragma region      Archery
public:
	/**화살의 메쉬를 가져온다*/
	virtual UStaticMesh* I_GetArrowMesh() =0;
	// #pragma endregion   Archery


	/***************************************************************Default************************************************************/
	// /**크리티멀 데미지를 적용 시킨다*/
	// virtual void I_BufferInput(EBufferKey InBufferKey) =0;
	//
	// /**일어서다*/
	// virtual void I_GetUp_KnockedDown() =0;
	//
	// /**공격 정보를 업데이트 한다*/
	// virtual void I_UpdateAttackInfo(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower, ECustomAttackTypes InCustomAttackTypes,
	//                                 bool InCauseKnockback, float InKnockbackTime,
	//                                 float InKnockbackSpeed, bool InIsUnblockableAttack, EAttackCategory InAttackCategory,
	//                                 int32 RegularDamageMultiplier,
	//                                 int32 PoiseDamageMultiplier) =0;
	//
	// virtual void I_ReleaseArcheryProjectile(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower,
	//                                         ECustomAttackTypes InCustomAttackTypes, bool InCauseKnockback, float InKnockbackTime,
	//                                         float InKnockbackSpeed, bool InIsUnblockableAttack, float InRegularDamageMultiplier,
	//                                         float InPoiseDamageMultiplier) =0;

	/**임팩트 이펙트를 플레이 합니다*/
	virtual void I_PlayImpactEffect(TSubclassOf<UCBaseImpactEffect> InImpactEffect, FVector InImpactPoint,
	                                EPhysicalSurface InSurfaceType, bool UseScreenShake) =0;

	//
	// /**크리티멀 데미지를 적용 시킨다*/
	// virtual void I_ApplyCriticalDamage(float CriticalDamageMultiplier) =0;
	//
	// /**드로우 스트링을 할 수 있는지*/
	// virtual bool CanDrawString() =0;
};
