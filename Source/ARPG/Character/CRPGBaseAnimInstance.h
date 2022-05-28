#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Datas/CombatData.h"
#include "GlobalFunctions/DoOnce.h"
#include "Interfaces/Interface_AnimInstance.h"
#include "Interfaces/Interface_Rotation.h"
#include "CRPGBaseAnimInstance.generated.h"

class UCEquipmentManagerComponent;
class ACRPGCharacter;
class UCCharacterStateManagerComponent;
class UCCombatComponent;


UCLASS()
class ARPG_API UCRPGBaseAnimInstance : public UAnimInstance,
                                       public IInterface_AnimInstance,
                                       public IInterface_Rotation
{
	GENERATED_BODY()
public:
	UCRPGBaseAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void EventOnCombatTypeChanged(ECombatType InCombatType);

	UFUNCTION()
	void EventOnGeneralCombatTypeChanged(EGeneralCombatType InGeneralCombatType);

	UFUNCTION()
	void EventOnCombatChanged(bool bInCombatEnabled);

	UFUNCTION()
	void EventOnFinisherModeChanged(bool InPunisherMode);

	UFUNCTION()
	void EventOnStateBegin(ECharacterState InCharacterState);

	UFUNCTION()
	void EventOnActionBegin(ECharacterAction InCharacterAction);

public:
	/**
	 * @brief 기울기를 계산합니다
	 */
	void CalculateLeanAngles(float InDeltaSeconds);

	/**
	 * @brief 캐릭터를 부드럽게 회전 시킵니다
	 */
	void SmoothCharacterRotation();

	/**
	 * @brief 조준중 회전을 합니다
	 */
	void AimingRotation();

	/**
	 * @brief IK HIT 반응을 실행합니다
	 */
	void IkHitReactions();

	/**
	 * @brief 공중 확인을 재설정합니다
	 */
	void ResetAirCheck();

	/**
	 * @brief 공중에 있다면 체크합니다
	 */
	void InAirCheck();

	/**
	 * @brief Root Yaw Offset을 계산합니다
	 */
	void CalculateRootYawOffset();

	/**
	 * @brief 낙하거리를 확인합니다
	 * @param InLocation 체크할 지점
	 * @return 낙하거리를 반환합니다
	 */
	float CheckFallDistance(const FVector_NetQuantize& InLocation);

	/**
	 * @brief  공중에 있을 때 라인트레이스를 발사하여 체크합니다
	 * @return 낙하속도다 -200보다 크다면 false를 반한합니다. -200보다 작다면 라인트레이스를 발사하여 캐릭터가 걸을 수있고 (그리고) 히트된 것이 있다면 true를 반환합니다
	 */
	bool JumpLineTrace();


#pragma region                                IInterface_AnimInstance
public:
	/**
	* @brief 애님인스턴스의 BowDrawSpeed 값을 설정합니다
	* @param InBowDrawSpeed 활을 그려야 할 스피드
	*/
	virtual void I_SetBowDrawSpeed(float InBowDrawSpeed) override;

	/**
	 * @brief 타겟의 룩(잠금)을 설정합니다
	 * @param IsTargetLocked 타겟이 록(잠금)의 설정여부
	 */
	virtual void I_IsTargetLocked(bool IsTargetLocked) override;

	/**
	 * @brief 질주상태를 설정합니다
	 * @param IsSprinting 질주설정
	 */
	virtual void I_IsSprinting(bool IsSprinting) override;

	/**
	 * @brief Leg IK Blend Weight 값을 설정합니다
	 * @param InLegBlendWeightMultiplier 
	 */
	virtual void I_SetLegIKBlendWeightMultiplier(float InLegBlendWeightMultiplier) override;

	/**
	 * @brief 플레이어의 그려야 할 손의 소켓을 설정합니다
	 * @param HandStringSocketName 그려야 할 손의 소켓네임
	 */
	virtual void I_SetPlayerHandStringSocketName(FName HandStringSocketName) override;

	/**
	 * @brief 제자리회전을 해야하는 지 설정합니다
	 * @param InShouldTurnInPlace 제자리 회전을 해야하는 설정값
	 */
	virtual void I_SetShouldTurnInPlace(bool InShouldTurnInPlace) override;
#pragma endregion                             IInterface_AnimInstance


#pragma region                                IInterface_Rotation
public:
	/**
	 * @brief 캐릭터의 회전을 가져옵니다
	 * @return 캐릭터의 회전 값
	 */
	virtual FRotator I_GetCharacterRotation() override;

	/**
	 * @brief 회전을 시작합니다
	 * @param InRotationInterpSpeed 회전 할때 인텁스피드 값 
	 */
	virtual void I_StartRotating(float InRotationInterpSpeed) override;

	/**
	 * @brief 회전을 중지합니다
	 */
	virtual void I_StopRotating() override;
#pragma endregion                             IInterface_Rotation

	/********************************************************************************************************************************Field Members***/
	/********************************************************************************************************************************Field Members***/
	/********************************************************************************************************************************Field Members***/

#pragma region               Component Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Component", meta=(AllowPrivateAccess="true"))
	UCEquipmentManagerComponent* EquipmentComponent;

	// UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Component", meta=(AllowPrivateAccess="true"))
	// UTargetingComponent* TargetingComponent;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Component", meta=(AllowPrivateAccess="true"))
	UCCombatComponent* CombatComponent;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Component", meta=(AllowPrivateAccess="true"))
	UCCharacterStateManagerComponent* CharacterStateManager;

	// UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Component", meta=(AllowPrivateAccess="true"))
	// UHitReactionComponent* HitReactionComponent;

#pragma endregion            Component Members


#pragma region               Leans Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Leans", meta=(AllowPrivateAccess="true"))
	float YawDelta;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Leans", meta=(AllowPrivateAccess="true"))
	FRotator RotationLastTick;
#pragma endregion            Leans Members


#pragma region               Aim Rotation Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | AnimRotation", meta=(AllowPrivateAccess="true"))
	float AimRotationYaw;
#pragma endregion            Aim Rotation Members


#pragma region               Turn In Place Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Turn In Place", meta=(AllowPrivateAccess="true"))
	float RootYawOffset;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Turn In Place", meta=(AllowPrivateAccess="true"))
	float YawLastTick;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Turn In Place", meta=(AllowPrivateAccess="true"))
	float YawChangeOverFrame;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Turn In Place", meta=(AllowPrivateAccess="true"))
	float DistanceCurveValue;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Turn In Place", meta=(AllowPrivateAccess="true"))
	float DistanceCurveValueLastFrame;
#pragma endregion            Turn In Place Members


#pragma region               Combat Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	ECombatType CombatType;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	bool bTargetLocked;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	bool bIsFinisherEnabled;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	ECharacterAction CharacterAction;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Combat", meta=(AllowPrivateAccess="true"))
	EGeneralCombatType GeneralCombatType;

#pragma endregion            Combat Members


#pragma region               Montages Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Montages", meta=(AllowPrivateAccess="true"))
	bool bIsMontagePlaying;
#pragma endregion            Montages Members


#pragma region               Character Rotation Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Character Rotation", meta=(AllowPrivateAccess="true"))
	bool bRotateCharacter;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Character Rotation", meta=(AllowPrivateAccess="true"))
	float RotationInterpSpeed;
#pragma endregion            Character Rotation Members


#pragma region               Aiming Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Aiming", meta=(AllowPrivateAccess="true"))
	float AimingRotationInterpSpeed;
#pragma endregion            Aiming Members


#pragma region               Movement Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float Roll;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float Pitch;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float Yaw;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float DirectionStop;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float CurrentAcceleration;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	float CurrentAccelerationWalk;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance | Movement", meta=(AllowPrivateAccess="true"))
	bool bTurnInPlace;

#pragma endregion            Movement Members


#pragma region               Default Members
private:
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	ACharacter* CharacterRef;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bIsTurning;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float FallingSpeed;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector LandingPoint;

	/**땅에서와의 거리*/
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float DistanceFromGround;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float StartingDistanceFromGround;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bBlockTransition;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bFocusTransition;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float AimRotationPitch;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bBowCharge;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bBowDrawn;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bReloadBow;

	/**기울기 강도 스케일링*/
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float LeanIntensityScaling;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bIsCombatEnabled;

	/**큰 착륙입니까?*/
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bLargeLanding;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float BowDrawSpeed;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float LerpAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float LClavAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_LClav;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_RClav;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float RClavAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_Spine03;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_Neck;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float Spine_Alpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float NeckAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_HandL;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	FVector WS_HandR;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float HandRAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float HandLAlpha;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float Leg_R;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float Leg_L;

	/**제자리 회전을 해야하는가?*/
	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bShouldTurnInPlace;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float TurnInPlacePlayRate;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	float LegBlendWeightMultiplier;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bAimTransition;

	UPROPERTY(BlueprintReadOnly, Category="RPG Animinstance", meta=(AllowPrivateAccess="true"))
	bool bCharacterInitialized;

	FDoOnce AirCheck_DoOnce;
	FDoOnce RootTurning_DoOnce;
#pragma endregion            Default Members
};
