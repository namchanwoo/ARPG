
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_AnimInstance.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_AnimInstance : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_API IInterface_AnimInstance
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 애님인스턴스의 BowDrawSpeed 값을 설정합니다
	 * @param InBowDrawSpeed 활을 그려야 할 스피드
	 */
	virtual void I_SetBowDrawSpeed(float InBowDrawSpeed) =0;

	/**
	 * @brief 타겟의 룩(잠금)을 설정합니다
	 * @param IsTargetLocked 타겟이 록(잠금)의 설정여부
	 */
	virtual void I_IsTargetLocked(bool IsTargetLocked) =0;

	/**
	 * @brief 질주상태를 설정합니다
	 * @param IsSprinting 질주설정
	 */
	virtual void I_IsSprinting(bool IsSprinting) =0;

	/**
	 * @brief Leg IK Blend Weight 값을 설정합니다
	 * @param InLegBlendWeightMultiplier 
	 */
	virtual void I_SetLegIKBlendWeightMultiplier(float InLegBlendWeightMultiplier) =0;

	/**
	 * @brief 플레이어의 그려야 할 손의 소켓을 설정합니다
	 * @param HandStringSocketName 그려야 할 손의 소켓네임
	 */
	virtual void I_SetPlayerHandStringSocketName(FName HandStringSocketName) =0;

	/**
	 * @brief 제자리회전을 해야하는 지 설정합니다
	 * @param InShouldTurnInPlace 제자리 회전을 해야하는 설정값
	 */
	virtual void I_SetShouldTurnInPlace(bool InShouldTurnInPlace) =0;
};
