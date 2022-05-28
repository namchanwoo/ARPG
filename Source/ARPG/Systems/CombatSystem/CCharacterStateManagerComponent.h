#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Datas/CharacterData.h"
#include "CCharacterStateManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, CharacterState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, CharacterState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlockStateSet, bool, IsBlocking);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwnerReset);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionBegin, ECharacterAction, CharacterAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnd, ECharacterAction, CharacterAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementCancelSet, bool, CanMovementCancel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimingStateSet, bool, Aiming);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCCharacterStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UCCharacterStateManagerComponent();

protected:
	virtual void BeginPlay() override;


public:
	/**CharacterState를 설정한다*/
	UFUNCTION(BlueprintCallable, Category="Character State")
	void SetState(ECharacterState NewState);

	/**CharacterAction 을 설정한다*/
	void SetAction(ECharacterAction NewAction);


public:
	/**전투가 비활성화상태인인가?*/
	bool IsCombatDisabled() const;

	/**Hit에 반응하고 있는가? */
	bool IsReactingToHits() const;

	/**블록킬 상태를 설정합니다*/
	void SetBlockingState(bool NewBlockingState);

	/**입력받은 비활성화설정에 따라 캐릭터의 가속도값을 설정합니다*/
	void DisableMovement(bool InDisableMovement);

	/**이동취소 가능한 상태를 설정합니다*/
	void SetCanMovementCancel(bool InCanMovementCancel);

	/**에임 상태를 설정*/
	void SetAimingState(bool NewAimingState);

public:
	/**오너캐릭터를 리셋합니다* 리셋타임이 0.0f보다 크지않다면 바로 리셋합니다*/
	void ResetOwner(float ResetTime);
	
	/**	Idle로 상태를 설정합니다*/
	void ResetState();

private:
	/**오너캐릭터의 상태를 리셋합니다 */
	UFUNCTION()
	void OwnerReset();
	
	/**OwnerCharacter의 CharacterMovementComponent를 가져온다*/
	class UCharacterMovementComponent* GetCharacterMovement();

#pragma region               Default Members
private:
	/**일반적인 소유자 캐릭터*/
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateManager | Default", meta=(AllowPrivateAccess="true"))
	ACharacter* OwnerCharacterRef;

	/**프레임 취소할 수 있는가?*/
	bool bCancelFrames;

	/**죽음상태인가?*/
	bool bDead;

	/**움직임이 비활성화인가?*/
	bool bMovementDisabled;

	/**기본 최대 가속*/
	float DefaultMaxAcceleration;

	/**무적 상태인가?*/
	bool bIsInvulnerable;

	/**방어 상태인가?*/
	bool bIsBlocking;

	/**캐릭터 액션*/
	ECharacterAction CharacterAction;

	/**움직임이 취소 될 수 있는가?*/
	bool bCanMovementCancel;

	/**에임 상태인가?*/
	bool bIsAiming;
#pragma endregion            Default Members


#pragma region               Combat State Members
private:
	/**현재 캐릭터 상태*/
	UPROPERTY(BlueprintReadOnly, Category="CharacterStateManager | Combat State", meta=(AllowPrivateAccess="true"))
	ECharacterState CharacterState;

	/**이전 캐릭터 상태*/
	ECharacterState PrevCharacterState;
#pragma endregion            Combat State Members


#pragma region               Dynamic Delegate Members
public:
	/**상태가 시작될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnStateBegin OnStateBegin;

	/**상태그 종료될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnStateEnd OnStateEnd;

	/**방어상태가 설정될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnBlockStateSet OnBlockStateSet;

	/**오너캐릭터가 리셋될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnOwnerReset OnOwnerReset;

	/**액션이 시작될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnActionBegin OnActionBegin;

	/**액션이 종료될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnActionEnd OnActionEnd;

	/**이동 취소가 설정될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnMovementCancelSet OnMovementCancelSet;

	/**조준상태가 설정될 때 호출됩니다*/
	UPROPERTY(BlueprintAssignable)
	FOnAimingStateSet OnAimingStateSet;
#pragma endregion            Dynamic Delegate Members


#pragma region                                FORCEINLINE
public:
	/**입력받은 캐릭터상태와 같은가?*/
	FORCEINLINE bool IsStateEqual(ECharacterState StateToCompare) const { return CharacterState == StateToCompare; }

	/**입력받은 캐릭터상태와 다른가?*/
	FORCEINLINE bool IsStateNotEqual(ECharacterState StateToCompare) const { return CharacterState != StateToCompare; }

	/**죽었는가?*/
	FORCEINLINE bool IsDead() const { return bDead; }

	/**조준중인가?*/
	FORCEINLINE bool IsAiming() const { return bIsAiming; }

	/**방어중인가?*/
	FORCEINLINE bool IsBlocking() const { return bIsBlocking; }

	/**무적 상태인가?*/
	FORCEINLINE bool IsInvulnerable() const { return bIsInvulnerable; }

	/**프레임을 취소할수있는 상태를 가져옵니다*/
	FORCEINLINE bool GetCancelFrames() const { return bCancelFrames; }

	/**이동을 취소할 수 있는가?*/
	FORCEINLINE bool CanMovementCancel() const { return bCanMovementCancel; }

	/**이동이 비활성화 상태인가?*/
	FORCEINLINE bool IsMovementDisabled() const { return bMovementDisabled; }

	/**현재 캐릭터 상태를 가져옵니다*/
	FORCEINLINE ECharacterState GetCurrentState() const { return CharacterState; }

	/**이전 캐릭터 상태를 가져옵니다*/
	FORCEINLINE ECharacterState GetPrevState() const { return PrevCharacterState; }

	/**현재 캐릭터 액션을 가져옵니다*/
	FORCEINLINE ECharacterAction GetCurrentAction() const { return CharacterAction; }

	/**죽음 상태를 설정합니다*/
	FORCEINLINE void SetDead(bool InDead) { bDead = InDead; }

	/**프레임 취소를 설정합니다*/
	FORCEINLINE void SetCancelFrames(bool InCancelFrames) { bCancelFrames = InCancelFrames; }

	/**무적 상태를 설정합니다*/
	FORCEINLINE void SetInvulnerable(bool InInvulnerable) { bIsInvulnerable = InInvulnerable; }
#pragma endregion                             FORCEINLINE
};
