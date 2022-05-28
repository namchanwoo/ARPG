#pragma once

#include "CoreMinimal.h"
#include "Datas/CombatData.h"
#include "UObject/NoExportTypes.h"
#include "CAttackInfo.generated.h"


UCLASS(Blueprintable)
class ARPG_API UCAttackInfo : public UObject
{
	GENERATED_BODY()


public:
	/**공격정보를 저장합니다*/
	void StoreAttackInfo(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower,
	                     ECustomAttackTypes InCustomAttackTypes,
	                     bool InCauseKnockBack, float InKnockBackTime,
	                     float InKnockBackSpeed, bool InIsUnblockableAttack,
	                     EAttackCategory InAttackCategory, ECombatType InCombatType, int32 InAttackIndex);

	/**사용자지정 공격 유형을 가져옵니다*/
	EHitReactionType GetCustomAttackType(EImpactDecision InImpactDecision);
public:
	/**어택 인덱스*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AttackIndex;

	/**잡기의 성공 여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bGrabSuccess;

	/**잡기 공격의 시작 위치*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GrabAttackStartPosition;

	/**넉백의 원인*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCauseKnockBack;

	/**넉백 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KnockBackTime;

	/**넉백 스피드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KnockBackSpeed;

	/**차단 불가 공격의 여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsUnBlockableAttack;

	/**근접 무기 방향*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMeleeSwingDirection MeleeSwingDirection;

	/**어택의 강도*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackPower AttackPower;

	/**사용자 지정 공격 유형*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECustomAttackTypes CustomAttackTypes;

	/**어택 카테고리*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackCategory AttackCategory;

	/**전투 타입*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECombatType CombatType;

	TMap<EImpactDecision,EHitReactionType> TestCustomAttackType;
};
