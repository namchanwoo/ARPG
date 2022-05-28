
#pragma once

#include "CoreMinimal.h"
#include "Datas/CharacterData.h"
#include "Datas/CombatData.h"
#include "UObject/NoExportTypes.h"
#include "CBaseHitReaction.generated.h"



UCLASS(Blueprintable)
class ARPG_API UCBaseHitReaction : public UObject
{
	GENERATED_BODY()

public:
	UCBaseHitReaction();

	
public:
	/**히트방향에 따른 히트반응 몽테이지들*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Montage")
	TMap<EHitReactionDirection, FActionMontage> HitReactMontages;

	/**공중에서의 히트방향에 따른 히트반응 몽테이지들*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Montage")
	TMap<EHitReactionDirection, FActionMontage> HitReactMontages_InAir;

	/**컴벳 타입에 따른 치명적인 반응 몽타주*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fatality")
	TMap<ECombatType, FActionMontage> FatalityMontages;
	

	
	/**방향성을 사용하겠는가?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool Directional;

	/**공중 히트 애니메이션 사용하시나이까?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool UseInAirMontages;

	/**사용자 지정 히트 반응 이벤트로 재정의여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool OverrideWithCustomHitReactEvent;

	/**캐릭터 상태 리셋 자동 재설정 여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	bool bAutoResetCharacterState;

	/**히트 반응 카테고리*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	EHitReactionCategory HitReactionCategory;

	/**히트를 반응할 때의 캐릭터 상태*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	ECharacterState HitReactionCharacterState;

	

	/**히트 반응 인덱스 타입*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	EHitReactIndexType HitReactIndexType;

	/**커스텀 히트 반응 인덱스*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	int32 CustomHitReactIndex;

	/**최소 평정 임계값 필요 여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	bool bRequireMinimumPoiseThreshold;

	/**공격 파워 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	TMap<EAttackPower, int32> AttackPowerIndex;

	/**공격력 지수 백분율을 사용여부*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	bool bUseAttackPowerIndexPercentage;

	/**공격 파워 인덱스 퍼센트*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Hit Reaction Index")
	TMap<EAttackPower, int32> AttackPowerIndexPercentage;

	/**평정 무게 인덱스*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	TMap<EAttackPower, int32> PoiseWeightIndex;

	/**평정 무게 인덱스 퍼센트*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction Index")
	TMap<EAttackPower, int32> PoiseWeightMaxPercentage;



	/**스윙 방향 사용 하시겠습니까?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Direction")
	bool UseSwingDirection;

	/***/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Direction")
	TMap<EMeleeSwingDirection, EHitReactionDirection> SwingDirection_Front;

	/***/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Direction")
	TMap<EMeleeSwingDirection, EHitReactionDirection> SwingDirection_Back;

	/***/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Direction")
	TMap<EMeleeSwingDirection, EHitReactionDirection> SwingDirection_Right;

	/***/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Direction")
	TMap<EMeleeSwingDirection, EHitReactionDirection> SwingDirection_Left;



	/**최대 회전 값*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction Rotation")
	float MaxRotation;

	/**초당 회전 값*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction Rotation")
	float RotationPerSecond;

	/**선택한 방향이 반응 적용자를 향하도록 적중 캐릭터를 회전해야 하는지의 여부.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction Rotation")
	TMap<EHitFacingDirection, bool> RotateToFaceApplier;



	/**히트 위치에 따른 히트방향을 담은 액션몽테이지*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Bone Reaction")
	TMap<EBodyHitLocations, FDirectionalHitReactMontageMap> HitBoneMontage;

	/**공중에서의 히트 위치에 따른 히트방향을 담은 액션몽테이지*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Bone Reaction")
	TMap<EBodyHitLocations, FDirectionalHitReactMontageMap> HitBoneMontage_InAir;

	
	
	/**FullBody IKHit 반응인가?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	bool bIsFullBodyIKHitReaction;

	/**리액션 진입 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	float ReactionIn;

	/**리액션 아웃 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	float ReactionOut;

	/**다리 혼합 가중치 배율*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	float LegBlendWeightMultiplier;

	/**IK 반응 강도*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	TMap<EProceduralHitReactAlphas, float> IKReactionAlphas;

	/**애디티브 레그 리액트 몽타주*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Hit Reaction")
	TMap<EHitFacingDirection, FAnimMontageStruct> AdditiveLegReactMontages;
};
