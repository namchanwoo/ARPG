#pragma once

#include "CoreMinimal.h"
#include "Datas/CombatData.h"
#include "Datas/StatData.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interface_Combat.h"
#include "Interfaces/Interface_GeneralCharacter.h"
#include "CRPGCharacter.generated.h"

class ACCombatPlayerController;

UCLASS(config=Game)
class ACRPGCharacter : public ACharacter, public IInterface_GeneralCharacter, public IInterface_Combat
{
private:
	GENERATED_BODY()
public:
	ACRPGCharacter();

	
	virtual void BeginPlay() override;

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnJumped_Implementation() override;

	/**캐릭터가 표면에서 걸어가다가 떨어지려할 대 호출됩니다*/
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
	                                              const FVector& PreviousLocation, float TimeDelta) override;

	UFUNCTION()
	void EventOnOnCombatTypeChanged(ECombatType InCombatType);

	UFUNCTION()
	void EventOnCombatTypeEnd(ECombatType InCombatType);
	
protected:
	/**
	 * @brief 차지어택을 재설정합니다
	 */
	void ResetChargeAttack();


	/**
	 * @brief 현재 공격키를 누른시간을 설정하고 차지어택을 체크합니다
	 * @param InAttackKey 공격 키 입니다 
	 */
	UFUNCTION()
	void LightChargeAttackTimerSetOn(const FKey& InAttackKey);

	/**
	 * @brief 라이트차지어택키를 체크합니다
	 * @param InAttackKey 현재 LightAttack Key
	 */
	void CheckLightChargeAttack(const FKey& InAttackKey);

	/**
	 * @brief 선택적 능력을 수행할 수 있습니까?
	 */
	bool CanPerformOptionalAbility(EOptionalCombatAbilities InOptionalActivity);

	/**
	 * @brief 포커스 상태에 진입할 수 있는가?
	 * @return 진입할 수 있다면 true를 반환합니다 
	 */
	bool CanEnterFocusState();

	/**
	 * @brief 현재캐릭터행동과 같은지 결과를 반환합니다
	 * @param CompareAction 비교할 캐릭터행동 
	 * @return 같다면 true를 반환합니다
	 */
	bool IsActionEqual(ECharacterAction CompareAction);

	/**
	* @brief 현재캐릭터행동과 다른지 결과를 반환합니다
	* @param CompareAction 비교할 캐릭터행동 
	 * @return 다르다면 true를 반환합니다
	 */
	bool IsActionNotEqual(ECharacterAction CompareAction);

	/**
	 * @brief 연속공격인지 확인합니다
	 * @param InAttackType 확인할 공격유형
	 * @return 연속공격이 맞다면 true를 반환합니다
	 */
	bool CheckChainAttack(EAttackTypes InAttackType);

	/**
	 * @brief 원하는 공격유형을 선택합니다
	 * @param InAttackType 원하는 공격유형
	 */
	bool SelectDesiredAttackType(EAttackTypes InAttackType);

	/**라이트 어택키를 눌렸을 때 호출됩니다*/
	void Event_LightAttackPressed(FKey InAttackKey);

	/**라이트 어택키를 누르고 땠을 때 호출됩니다*/
	void Event_LightAttackReleased();

	/**질주키를 눌렀을 때 호출됩니다*/
	void Event_SprintPressed();

	/**점프키를 눌렀을 때 호출됩니다*/
	void Event_JumpPressed();

	/**점프키를 눌렀다 땠을 때 호출됩니다*/
	void Event_JumpReleased();

	/**Crouch 키를 입력했을 때 호출됩니다*/
	void Event_Crouch();

	/**회피키를 눌렀을 때 호출됩니다*/
	void Event_RollPressed(FKey InKey);

	/**회피키를 누르고 땠을 때 호출됩니다*/
	void Event_RollReleased();

	/**아이템을 장착합니다*/
	void Event_EquipItem();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


#pragma region                                Movement Methods
protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**컨트롤하고 있는 방향을 가져옵니다*/
	void GetControlDirection(FVector& OutForwardVector, FVector& OutRightVector);

	/**걷기를 토글합니다*/
	void ToggleWalk();

	UFUNCTION()
	/**닷지를 누르고 0.45초가 지난 뒤의 로직입니다*/
	void DodgeAfter();

	/**이동 시도를 취소할 때 호출됩니다*/
	UFUNCTION()
	void AttemptMovementCancel();

public:
	/**회피 유형에 따른 회피를 수행합니다*/
	bool PerformDodge(EDodgeType InDodgeType, EDodgeDirection InDodgeDirection, int32 InMontageIndex = 0);

	/**회피 키를 누른 시간을 설정합니다*/
	UFUNCTION()
	void DodgeHeldTimer();

	/**회피 키를 초기화하고 DodgeHeldTimer를 클리어합니다*/
	UFUNCTION()
	void ResetDodgeHeldTimer();

public:
	UFUNCTION()
	void EventOnStateBegin(ECharacterState InCharacterState);

	UFUNCTION()
	void EventOnStateEnd(ECharacterState InCharacterState);

	UFUNCTION()
	void EventOnMovementCancelSet(bool InCanMovementCancel);

	UFUNCTION()
	void EventOnActionBegin(ECharacterAction InCharacterAction);

	UFUNCTION()
	void EventOnActionEnd(ECharacterAction InCharacterAction);

	UFUNCTION()
	void EventOnOwnerReset();

	UFUNCTION()
	void EventOnBlockStateSet(bool InIsBlocking);

	/*회피하며 일어섭니다**/
	void GetUpRoll();

	/**구르기 회피를 실행합니다*/
	void RollDodge();

	/**스텝 회피를 실행합니다*/
	void StepDodge();

public:
	/**입력받은 보행유형과 같은가?*/
	bool IsGaitEqual(EGait CompareGait);

	/**입력받은 보행유형과 같지않은가?*/
	bool IsGaitNotEqual(EGait CompareGait);

	/**질주를 할수 있는가?*/
	bool CanSprint();

	/**구르기 회피가 가능한가?*/
	bool CanRollDodge();

	/**스텝 회피가 가능한가?*/
	bool CanStepDodge();

	/**스텝 회피가 가능한가?*/
	bool CanStepBack();

	/**현재 추락중인가?*/
	bool IsFalling();

	/**현재 캐릭터가 가속되고 있는 방향으로 회전시키는가?*/
	bool IsOrientRotationToMovement();

	/**이동 입력으로 취소할 수 있습니까?**/
	bool CanCancelIntoMovementInput();
#pragma endregion                             Movement Methods


#pragma region                                Combat Methods
public:
	/**공격이 가능하다면? */
	bool CanAttack();

	/**패리를 수행할 수 있습니까?*/
	bool CanPerformParry();

	/**웨폰기술 공격인가?*/
	bool IsWeaponArtAttack();

	/**대쉬공격인가?*/
	bool IsDashAttack();

	/**차지공격인가?*/
	bool IsChargeAttack();

	/**차지공격 인덱스를 선택합니다*/
	int32 SelectChargeAttackIndex();

	/**컴뱃모드에 진입할 수 있는가?*/
	bool CanEnterCombatMode();

	/**전투상태를 활성화합니다*/
	void EnableCombat();

	/**조준을 취소합니다*/
	void CancelAiming();
	
	/**전투상태를 비활성화 합니다*/
	void DisableCombat();

	/**전투상태를 토글합니다*/
	void ToggleCombat();

	/**공격핸들을 재설정합니다 */
	UFUNCTION()
	void ResetAttackHandle();

	/**
	 * @brief 공격유형이 시작되면 호출됩니다
	 * @param InAttackTypes 시작되는 공격유형
	 */
	void OnAttackTypeBegin(EAttackTypes InAttackTypes);
	
	/**
	 * @brief 공격작업을 수행합니다 
	 * @param InAttackType  공격 유형
	 * @param InAttackIndex 랜덤인덱스가 true가 아니라면 몽테이지를 수행할 지정된 인덱스
	 * @param IsRandomIndex 몽테이지의 재생 인덱스를 랜덤값으로 설정
	 * @return 공격작업을 성공했다면 true 아니라면 false를 반환합니다
	 */
	bool PerformAttack(EAttackTypes InAttackType, int32 InAttackIndex=0, bool IsRandomIndex=false);

	/**
	 * @brief 액션작업을 수행합니다
	 * @param InCombatAction 실행할 액션 유형
	 * @param InActionCombatState 액션을 실행하고 설정될 캐릭터 상태
	 * @param InMontageStartPosition 몽테이지 시작 위치(시간)
	 * @param InMontageIndex 랜덤인덱스가 비활성화라면 실행할 몽테이지 인덱스
	 * @param IsRandomIndex 몽테이지실행 인덱스의 랜덤설정
	 * @param IsAutoResset 캐릭터의 리셋상태 설정
	 * @return 액션작업을 성공했다면 true 아니라면 false를 반환합니다
	 */
	bool PerformAction(ECharacterAction InCombatAction, ECharacterState InActionCombatState, float InMontageStartPosition = 0.0f,
	                   int32 InMontageIndex = 0, bool IsRandomIndex = false, bool IsAutoResset = true);


	/**공격 이벤트를 실행합니다 */
	void AttackEvent();

	UFUNCTION()
	void EventOnInputBufferConsumed(EBufferKey InputBufferKey);

	UFUNCTION()
	void EventOnInputBufferOpen();

	UFUNCTION()
	void EventOnInputBufferClose();

	/**상태입력을 시도합니다*/
	bool AttemptToEnterStates();

	/**방어를 해재합니다*/
	void BlockReleased();

	/**방어중을 중지합니다*/
	UFUNCTION()
	void StopBlocking();

	/**근접공격을 초기화 합니다*/
	UFUNCTION()
	void ResetMeleeAttack();

	void UpdateBlocking(ECharacterState InCharacterState);
public:
	/**현재 공격유형과 지정한 공격형이 같은가?*/
	bool IsAttackTypeEqual(EAttackTypes CompareAttackType);

	/**현재 공격유형과 지정한 공격형이 다른가?*/
	bool IsAttackTypeNotEqual(EAttackTypes CompareAttackType);
#pragma endregion                             Combat Methods


#pragma region                                Stat Methods
public:
	UFUNCTION()
	void EventOnCurrentStatValueUpdated(EStats InStatUpdated, float InCurrentValue, float InMaxValue);

	/**지정한 유형의 값이 지정된 최소 값보다 크거나 같은가?*/
	bool HasEnoughOfStat(EStats InStat, float MinimumValue);

#pragma endregion                             Stat Methods


#pragma region                                State Methods
public:
	/**캐릭터의 상태를 리셋합니다*/
	void ResetCharacterStates();
#pragma endregion                             State Methods


#pragma region                                Montage Methods
public:
	/**몽테이지가 종료될 때 호출됩니다*/
	UFUNCTION()
	void EventOnMontageInstancesEnd();

	UFUNCTION()
	void EventOnMontagesUpdated(FDataTableRowHandle InDataTableInfo);

	/**우선 순위가 높은 몽타주 재생*/
	float PlayHighPriorityMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InStartMontageTime, EMontagePlayReturnType MontageReturn,
	                              bool IsStopAllMontages);

private:
	/**현재 지정한 몽테이자가 재생중인가?*/
	bool IsMontagePlaying(UAnimMontage* InMontage);

	/**현재 몽테이자가 재생중인가?*/
	bool IsAnyMontagePlaying();

public:
#pragma endregion                             Montage Methods


#pragma region                                Help / Component Method
public:
	/**
	 * @brief 캐릭터상태매니저의 현재 캐릭터 상태를 가져옵니다
	 * @return 
	 */
	ECharacterState GetCurrentCharacterState();

	/**
	 * @brief 장비매니저컴포넌트의 일반적인 전투유형을 가져옵니다
	 * @return 
	 */
	EGeneralCombatType GetCurrentGeneralCombat();

	/**
	 * @brief 현재 캐릭터의 상태와 같은지 결과를 반환합니다
	 * @param InState 비교할 캐릭터 상태 
	 * @return 같다면 true를 반환합니다
	 */
	bool IsCharacterStateEqual(ECharacterState InState);

	/**
	 * @brief 현재 캐릭터의 상태와 다른지 결과를 반환합니다
	 * @param InState 비교할 캐릭터 상태 
	 * @return 다르다면 true를 반환합니다
	 */
	bool IsCharacterStateNotEqual(ECharacterState InState);

	/**
	 * @brief 컴뱃컴포넌트의 현재 회피(Dodge)유형을 가져옵니다
	 * @return 
	 */
	EDodgeType OnDodgeType();

	/**
	 * @brief 현재캐릭터 상태와 비교할캐릭터상태가 같은가?
	 * @param CompareCharacterState 비교할 캐릭터상태
	 * @return 같다면 true를 반환합니다
	 */
	bool OnGeneralCombatTypeEqual(EGeneralCombatType CompareCharacterState);

	/**
	 * @brief 현재 공격유형과 비교하여 같은지 결과를 반환합니다
	 * @param CompareAttackTypes 비교할 공격유형
	 * @return 같다면 true를 반환합니다
	 */
	bool OnAttackTypeEqual(EAttackTypes CompareAttackTypes);

	/**
	 * @brief 현재 공격유형과 비교하여 다른지 결과를 반환합니다
	 * @param CompareAttackTypes 비교할 공격유형
	 * @return 다르다면 true를 반환합니다
	 */
	bool OnAttackTypeNotEqual(EAttackTypes CompareAttackTypes);

private:
	/**
	 * @brief 메쉬의 애니메이션 인스턴스를 가져옵니다
	 * @return 메쉬의 애니메이션 인스턴스
	 */
	UAnimInstance* GetAnimInstance();

	/**
	 * @brief 캐릭터상태매니저 컴포넌트의 죽음상태를 가져옵니다
	 * @return 캐릭터가 죽음상태라면 true를 반환합니다
	 */
	bool IsDead();

	/**
	 * @brief 마지막 움직임 입력키를 체크합니다
	 * @return 마지막 움직임 입력키가 ZeroVector이 아니라면 true를 반환합니다
	 */
	bool HasMovementInput();

	/**
	 * @brief 클릭 및 키입력을 감지합니다. 감지한 결과에 따라 함수를 실행합니다
	 * @param DoubleClick 2번이상 클릭했을 경우
	 * @param SingleClick 1번 클릭했을 경우
	 */
	void DetectDoubleClick(TFunctionRef<void(void)> DoubleClick, TFunctionRef<void(void)> SingleClick);

#pragma endregion                             Help / Component Method


#pragma region                                IInterface_GeneralCharacter
public:
	/**제자리에서 회전을 해야하는가?*/
	virtual bool I_TurnInPlace(bool InTurnPlace) override;

	/**질주를 중지한다*/
	virtual void I_StopSprinting() override;

	/**카메라와 스프링암을 반환해줍니다*/
	virtual void I_SendCharacterCameraAndSpringArm(class UCameraComponent** OutCameraComponent, class USpringArmComponent** OutSpringArmComponent)
	override;

	/**발자국 사운드*/
	virtual void I_FootStep(EPhysicalSurface InSurfaceType, FVector StepLocation) override;

	/**슬로우 모션*/
	virtual void I_SlowMotion(float InSlowMotionStrength, float TimeInSlowMotion) override;

	/**캐릭터메쉬를 레글돌 합니다*/
	virtual void I_RagDollCharacterMesh() override;

	/**모닥불에서 휴식*/
	virtual void I_RestAtBonfire() override;

	/**캐릭터가 질주중인가?*/
	virtual bool I_IsCharacterSprinting() override;

	/**낙하 데미지를 받아야하는가?*/
	virtual void I_FallDamage(bool InFallDamage) override;

	/**카메라 움직임을 토글합니다*/
	virtual void I_ToggleCameraMovement(bool InDisableCameraMovement) override;

	/**액션몽테이지를 플레이합니다*/
	virtual void I_PlayActionMontage(ECharacterAction InCombatAction, int32 MontageIndex) override;

	/**활을 그려야하는 지 설정 합니다*/
	virtual void I_ShouldDrawBowString(bool InShouldDrawBowString) override;

	/**어빌리티 작업을 수행합니다*/
	virtual void I_PerformAbility(ECharacterAction InAction, ECharacterState InState, float
	                              MontageStartPosition, int32 MontageIndex, bool RandomIndex, bool AutoReset) override;


	/**일시정지 메뉴를 토글합니다*/
	virtual void I_TogglePauseMenu() override;

	/**모닥불 휴식 메뉴를 토글합니다*/
	virtual void I_ToggleBonfireMenu() override;

	/**유저 인터페이스 메뉴를 토글합니다*/
	virtual void I_ToggleUserInterface(EUserInterface InUserInterface) override;

	/**스크롤 키를 가져옵니다*/
	virtual FKey I_GetScrollKey() override;
#pragma endregion                             IInterface_GeneralCharacter


#pragma region                                IInterface_Combat
public:
	virtual void I_EndStrongChargeAttack() override;
	virtual bool I_CombatEnabled() override;
	virtual UStaticMesh* I_GetArrowMesh() override;
	virtual void I_PlayImpactEffect(TSubclassOf<UCBaseImpactEffect> InImpactEffect, FVector InImpactPoint,
	                                EPhysicalSurface InSurfaceType, bool UseScreenShake) override;
#pragma endregion                             IInterface_Combat


	/************************************************************************************************************************Field Members**********/
	/************************************************************************************************************************Field Members**********/
	/************************************************************************************************************************Field Members**********/
public:
#pragma region               Default Members
private:
	/**영혼 스폰 트렌스폼*/
	FTransform SpawnSoulTransform;

	/**활을 그려야 하는가?*/
	bool bShouldDrawBowString;

	UPROPERTY(BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	ACCombatPlayerController* CombatPlayerController;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* LockOnCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BossStealthKill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCStatsManagerComponent* StatsManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCCharacterStateManagerComponent* CharacterStateManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCMontageManagerComponent* MontageManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCustomCollisionComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPGCharacter | Default", meta = (AllowPrivateAccess = "true"))
	class UCEquipmentManagerComponent* EquipmentComponent;

#pragma endregion            Default Members


#pragma region               Movement Members
private:
	/**질주중인가?*/
	bool bIsSprinting;

	/**걷기 토글*/
	bool bToggleWalk;

	/**구르기 키를 지속적으로 누른 시간*/
	UPROPERTY(BlueprintReadOnly, Category="RPGCharacter | Movement", meta=(AllowPrivateAccess="true"))
	float DodgeHeldTime;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(BlueprintReadOnly, Category="RPGCharacter | Movement", meta=(AllowPrivateAccess="true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(BlueprintReadOnly, Category="RPGCharacter | Movement", meta=(AllowPrivateAccess="true"))
	float BaseLookUpRate;

#pragma endregion            Movement Members


#pragma region               Combat Members
private:
	/**공격 횟수*/
	int32 AttackCount;

	/**공격 유지 시간*/
	float AttackHeldTime;

	/**특별한 행동(액션)을 눌렀는가?*/
	bool bIsSpecialActionPressed;

	/**블록 유지 시간*/
	UPROPERTY(BlueprintReadWrite, Category="Properties | Combat", meta=(AllowPrivateAccess="true"))
	float BlockHeldTime;

	/**전투 리셋 핸들*/
	FTimerHandle ResetComboHandle;

	/**포커스 상태를 체크*/
	bool bCheckFocusState;

	/**방어의 깨진 상태 여부*/
	bool bIsDefenseBroken;

	/**피니셔 모드를 눌렀는가?*/
	bool bFinisherModePressed;

	/**피니셔모드 공격인가?*/
	bool bFinisherModeAttack;

	/**퍼펙트블록에 성공했는가?*/
	bool bPerfectBlockSuccessful;

	/**강력한 차지 공격량*/
	float StrongChargeAttackAmount;

	/**강한 공격을 눌렀는가?*/
	bool bStrongAttackPressed;
#pragma endregion            Combat Members


#pragma region               Charge Attack Members
private:
	/**강한 차지공격 윈도우 활성화 여부*/
	bool bStrongChargeAttackWindow;

	/**가벼운공격 차지 어택타이머 핸들*/
	FTimerHandle LightChargeAttackHandle;

	/**차지공격 카운트(횟수)*/
	int32 ChargeAttackCount;

	/**차지 공격 지속시간*/
	float ChargeAttackHoldTime;
#pragma endregion            Charge Attack Members


#pragma region               Damage Members
private:
	/**히트 액터*/
	UPROPERTY()
	AActor* HitActor;

	/**히트정보*/
	FHitResult HitInfo;

	/**추락피해입니까?*/
	bool bFallDamage;
#pragma endregion            Damage Members


#pragma region               Interaction Members
private:
	UPROPERTY(BlueprintReadOnly, Category = "RPGCharacter | Interaction", meta = (AllowPrivateAccess = "true"))
	AActor* InteractionHitActor;

	FHitResult InteractionHitResult;
#pragma endregion            Interaction Members


#pragma region               Camera Members
private:
	float DefaultBoomArmLength;

	FVector DefaultBoomOffset;

	/**카메라가 확대되었습니까?*/
	bool bIsCameraZoomed;

	/**카메라 이동을 비활성화하시겠습니까?*/
	bool bDisableCameraMovement;
#pragma endregion            Camera Members


private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	int32 LatentAction;
	FKey RollKey;
	FLatentActionInfo DodgeAfterLogicLatenInfo;

#pragma region                                FORCEINLINE METHODS
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma endregion                             FORCEINLINE METHODS
};


