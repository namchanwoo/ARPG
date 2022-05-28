
#pragma once

#include "CoreMinimal.h"
#include "Datas/CharacterData.h"
#include "Datas/WidgetData.h"
#include "UObject/Interface.h"
#include "Interface_GeneralCharacter.generated.h"


UINTERFACE(MinimalAPI)
class UInterface_GeneralCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_API IInterface_GeneralCharacter
{
	GENERATED_BODY()

public:

	/**제자리에서 회전을 해야하는가?*/
	virtual bool I_TurnInPlace(bool InTurnPlace) =0;

	/**질주를 중지한다*/
	virtual void I_StopSprinting() =0;

	/**카메라와 스프링암을 반환해줍니다*/
	virtual void I_SendCharacterCameraAndSpringArm(class UCameraComponent** OutCameraComponent, class USpringArmComponent** OutSpringArmComponent) =0;

	/**발자국 사운드*/
	virtual void I_FootStep(EPhysicalSurface InSurfaceType, FVector StepLocation) =0;

	/**슬로우 모션*/
	virtual void I_SlowMotion(float InSlowMotionStrength, float TimeInSlowMotion) =0;
	
	/**캐릭터메쉬를 레글돌 합니다*/
	virtual void I_RagDollCharacterMesh() =0;

	/**모닥불에서 휴식*/
	virtual void I_RestAtBonfire() =0;

	/**캐릭터가 질주중인가?*/
	virtual bool I_IsCharacterSprinting() =0;

	/**낙하 데미지를 받아야하는가?*/
	virtual void I_FallDamage(bool InFallDamage) =0;

	/**카메라 움직임을 토글합니다*/
	virtual void I_ToggleCameraMovement(bool InDisableCameraMovement) =0;

	/**액션몽테이지를 플레이합니다*/
	virtual void I_PlayActionMontage(ECharacterAction InCombatAction, int32 MontageIndex) =0;

	/**활을 그려야하는 지 설정 합니다*/
	virtual void I_ShouldDrawBowString(bool InShouldDrawBowString) =0;

	/**어빌리티 작업을 수행합니다*/
	virtual void I_PerformAbility(ECharacterAction InAction, ECharacterState InState, float
								MontageStartPosition, int32 MontageIndex, bool RandomIndex, bool AutoReset) =0;


	
	
	
	/**일시정지 메뉴를 토글합니다*/
	virtual void I_TogglePauseMenu() =0;

	/**모닥불 휴식 메뉴를 토글합니다*/
	virtual void I_ToggleBonfireMenu() =0;

	/**유저 인터페이스 메뉴를 토글합니다*/
	virtual void I_ToggleUserInterface(EUserInterface InUserInterface) =0;

	/**스크롤 키를 가져옵니다*/
	virtual FKey I_GetScrollKey() =0;
};
