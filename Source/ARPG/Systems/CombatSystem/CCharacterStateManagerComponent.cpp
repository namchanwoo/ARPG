#include "CCharacterStateManagerComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UCCharacterStateManagerComponent::UCCharacterStateManagerComponent()
{
	bMovementDisabled = true;
}

void UCCharacterStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacterRef = Cast<ACharacter>(GetOwner());
	DefaultMaxAcceleration = GetCharacterMovement()->MaxAcceleration;
}

void UCCharacterStateManagerComponent::SetState(ECharacterState NewState)
{
	//OwnerReset Clear
	UKismetSystemLibrary::K2_ClearTimer(this, TEXT("OwnerReset"));

	//현재 캐릭터 상태화 새로운 캐릭터 상태가 같지 않을 때만
	if (CharacterState != NewState)
	{
		PrevCharacterState = CharacterState;//그 전 캐릭터 상태를 설정한다

		//그 전 캐릭터 상태를 끝낸다.
		if (OnStateEnd.IsBound())
			OnStateEnd.Broadcast(PrevCharacterState);

		//현재 캐릭터의 상태를 새로운 상태로
		CharacterState = NewState;

		//현재 캐릭터 상태를 시작한다
		if (OnStateBegin.IsBound())
			OnStateBegin.Broadcast(CharacterState);
	}
}

void UCCharacterStateManagerComponent::SetAction(ECharacterAction NewAction)
{
	//현재 캐릭터 액션과 새로운 액션이 같지 않을때만
	if (CharacterAction != NewAction)
	{
		//OnActionEnd 델리게이트 호출
		if (OnActionEnd.IsBound())
			OnActionEnd.Broadcast(CharacterAction);

		CharacterAction = NewAction;

		if (OnActionBegin.IsBound())
			OnActionBegin.Broadcast(CharacterAction);
	}
}

bool UCCharacterStateManagerComponent::IsCombatDisabled() const
{
	TArray<ECharacterState> DisabledState;
	DisabledState.Add(ECharacterState::Stunned);
	DisabledState.Add(ECharacterState::KnockDown);
	DisabledState.Add(ECharacterState::Dead);
	DisabledState.Add(ECharacterState::Disabled);

	return DisabledState.Contains(CharacterState);
}

bool UCCharacterStateManagerComponent::IsReactingToHits() const
{
	TArray<ECharacterState> L_HitReactions;
	L_HitReactions.Add(ECharacterState::HitStunned);
	L_HitReactions.Add(ECharacterState::BlockStunned);

	return L_HitReactions.Contains(CharacterState);
}

void UCCharacterStateManagerComponent::SetBlockingState(bool NewBlockingState)
{
	//블록킹 상태가 새로운블록킹 상태와 같지 않다면
	if (IsBlocking() != NewBlockingState)
	{
		bIsBlocking = NewBlockingState;
		if (OnBlockStateSet.IsBound())
			OnBlockStateSet.Broadcast(bIsBlocking);
	}
}

void UCCharacterStateManagerComponent::DisableMovement(bool InDisableMovement)
{
	bMovementDisabled = InDisableMovement;

	//움직임이 비활성화 상태면
	if (bMovementDisabled)
		GetCharacterMovement()->MaxAcceleration = 0.0f;
	else
		GetCharacterMovement()->MaxAcceleration = DefaultMaxAcceleration;
}

void UCCharacterStateManagerComponent::SetCanMovementCancel(bool InCanMovementCancel)
{
	bCanMovementCancel = InCanMovementCancel;
	if (OnMovementCancelSet.IsBound())
		OnMovementCancelSet.Broadcast(InCanMovementCancel);
}

void UCCharacterStateManagerComponent::SetAimingState(bool NewAimingState)
{
	//조준 상태가 새로운 조준상태와 다르다면
	if (IsAiming() != NewAimingState)
	{
		bIsAiming = NewAimingState;
		if (OnAimingStateSet.IsBound())
			OnAimingStateSet.Broadcast(bIsAiming);
	}
}

void UCCharacterStateManagerComponent::ResetOwner(float ResetTime)
{
	//ResetTime가 0.0보다 같거나 작다면
	if (ResetTime <= 0.0f)
		OwnerReset();
	else //리셋타임이 0.0f보다 크다면
	{
		const FString SetFunction_OwnerReset = TEXT("OwnerReset");
		UKismetSystemLibrary::K2_SetTimer(this, SetFunction_OwnerReset, ResetTime, false);
	}
}

void UCCharacterStateManagerComponent::ResetState()
{
	SetState(ECharacterState::Idle);
	PrevCharacterState = ECharacterState::Idle;
}

void UCCharacterStateManagerComponent::OwnerReset()
{
	if (OnOwnerReset.IsBound())
		OnOwnerReset.Broadcast();
}

UCharacterMovementComponent* UCCharacterStateManagerComponent::GetCharacterMovement()
{
	return OwnerCharacterRef->GetCharacterMovement();
}

