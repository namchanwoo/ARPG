#include "CCombatComponent.h"

#include "Global.h"
#include "CCharacterStateManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UCCombatComponent::UCCombatComponent()
{
	MinimumAllowedParryDistance = 200.0f; //최소 허용 페리거리

	PerfectBlockTiming = 0.25f; //완벽한 방어 타이밍

	//추적할 소켓
	TraceTargetSockets.Add(ECollisionTraceTarget::RightHand, FName(TEXT("HandRTrace")));
	TraceTargetSockets.Add(ECollisionTraceTarget::LeftHand, FName(TEXT("HandLTrace")));
	TraceTargetSockets.Add(ECollisionTraceTarget::RightFoot, FName(TEXT("FootRTrace")));
	TraceTargetSockets.Add(ECollisionTraceTarget::LeftFoot, FName(TEXT("FootLTrace")));

	CurrentDodgeType = EDodgeType::RollDodge; //현재 구르기 타입
}

void UCCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	//캐릭터 스테이트 매니저 컴포넌트 설정
	StateManagerComponent = Cast<UCCharacterStateManagerComponent>
		(GetOwner()->GetComponentByClass(UCCharacterStateManagerComponent::StaticClass()));

	OwnerCharacterRef = Cast<ACharacter>(GetOwner()); //오너캐릭터를 설정
	GenericPlayerController = Cast<APlayerController>(OwnerCharacterRef->GetController()); //플레이어컨트롤러를 설정
}

void UCCombatComponent::StoreBufferKey(EBufferKey InStoredBufferKey)
{
	//저장된 버퍼 키 업데이트(모든 버퍼링된 작업에 대해 키를 업데이트해야 함)
	StoredBufferKey = InStoredBufferKey;

	//입력 버퍼가 열려 있으면 입력을 저장합니다. 
	if (bIsBufferOpen)
		bSaveInput = true; //입력을 저장한다
	else //열려 있지 않으면 작업을 수행한다
	{
		if (OnInputBufferConsumed.IsBound())
			OnInputBufferConsumed.Broadcast(StoredBufferKey);

		ClearStoredBufferKeys();
	}
}

void UCCombatComponent::OpenInputBuffer()
{
	bCanConsumeInputBuffer = true;
	bIsBufferOpen = true;
	if (OnInputBufferOpen.IsBound())
		OnInputBufferOpen.Broadcast();
}

void UCCombatComponent::CloseInputBuffer()
{
	//저장된 입력이 참이면 버퍼링된 작업을 수행하고 입력 버퍼를 닫습니다. 입력이 저장되지 않으면 입력 버퍼를 닫습니다.

	bIsBufferOpen = false; //입력버퍼를 닫습니다
	bSaveInput = false; //저장된 입력버퍼를 닫습니다

	//입력 버퍼를 사용 할 수 있는 상태면
	if (bCanConsumeInputBuffer)
	{
		ResetCombatState(); //컴뱃스테이트를 초기화합니다

		//버퍼를 사용합니다
		if (OnInputBufferConsumed.IsBound())
			OnInputBufferConsumed.Broadcast(StoredBufferKey);
		//버퍼를 닫습니다
		if (OnInputBufferClose.IsBound())
			OnInputBufferClose.Broadcast();

		ClearStoredBufferKeys(); //버퍼키를 클리어합니다
	}
	else //입력 버퍼를 사용 할 수 없는 상태라면
		ClearStoredBufferKeys(); //버퍼키들을 클리어 한다
}

void UCCombatComponent::ClearStoredBufferKeys()
{
	StoredBufferKey = EBufferKey::None; //저장된 버퍼키 초기화
	AttackBufferKey = EAttackTypes::None; //공격 버퍼키 초기화
	QuickSlotBufferKey = EQuickSlots::None; //퀵 슬롯 버퍼키 초기화
}

void UCCombatComponent::ClearInputBuffer()
{
	bIsBufferOpen = false;
	bCanConsumeInputBuffer = false;
	bSaveInput = false;
}

void UCCombatComponent::OnlyBufferCertainKeys(TMap<EBufferKey, bool> InBufferKey, bool IsResetState)
{
	bIsBufferOpen = false;
	bSaveInput = false;

	const bool L_IsBuffer = InBufferKey.Contains(StoredBufferKey) && *InBufferKey.Find(StoredBufferKey);

	//인버퍼키에 저장된 키가 존재하고 (그리고) 그 저장된 키의 bool 값이 True라면
	if (L_IsBuffer)
	{
		//입력 버퍼를 사용 할 수 상태 인지
		if (bCanConsumeInputBuffer)
		{
			//스테이트를 리셋해야한다면
			if (IsResetState)
				StateManagerComponent->SetState(ECharacterState::Idle);

			if (OnInputBufferConsumed.IsBound())
				OnInputBufferConsumed.Broadcast(StoredBufferKey);

			if (OnInputBufferClose.IsBound())
				OnInputBufferClose.Broadcast();

			bSaveInput = false;
			ClearStoredBufferKeys();
		}
		else //입력 버퍼를 사용 할 수 없다면
		{
			if (OnInputBufferClose.IsBound())
				OnInputBufferClose.Broadcast(); //입력버퍼를 닫습니다
			ClearStoredBufferKeys(); //저장된 키를 클리어합니다
		}
	}
	else //저장된 키가 존재하지 않거나 또는 저장된 키의 값이 false라면
	{
		if (OnInputBufferClose.IsBound())
			OnInputBufferClose.Broadcast(); //입력버퍼를 닫습니다
		ClearStoredBufferKeys(); //저장된 키를 클리어합니다
	}
}

void UCCombatComponent::StoreAttackBufferKey(EAttackTypes InAttackBufferKey)
{
	//저장된 버퍼 키 업데이트(모든 버퍼링된 작업에 대해 키를 업데이트해야 함)
	AttackBufferKey = InAttackBufferKey;
	StoredBufferKey = EBufferKey::Attack;

	//입력 버퍼가 열려 있으면 입력을 저장합니다.
	if (bIsBufferOpen)
		bSaveInput = true;
	else //입력버퍼가 열려있지 않다면 작업을 수행합니다
	{
		if (OnInputBufferConsumed.IsBound())
			OnInputBufferConsumed.Broadcast(StoredBufferKey);

		ClearStoredBufferKeys();
	}
}

void UCCombatComponent::StoreQuickSlotBufferKey(EQuickSlots InQuickSlotBufferKey)
{
	//저장된 버퍼 키 업데이트(모든 버퍼링된 작업에 대해 키를 업데이트해야 함)
	QuickSlotBufferKey = InQuickSlotBufferKey;
	StoredBufferKey = EBufferKey::SwitchActiveItem;

	//입력 버퍼가 열려 있으면 입력을 저장한다
	if (bIsBufferOpen)
		bSaveInput = true;
	else //입력 버퍼가 닫혀 있다면 버퍼키를 소비한다
	{
		if (OnInputBufferConsumed.IsBound())
			OnInputBufferConsumed.Broadcast(StoredBufferKey);
		ClearStoredBufferKeys();
	}
}

void UCCombatComponent::StoreHeldBufferKey(EBufferKey HeldBufferKey, bool InValue, bool IsConsume)
{
	//보유한 버퍼키의 값과 입력받은 값이 다를 경우에만
	if (GetHeldBufferKeyValue(HeldBufferKey) != InValue)
	{
		//참이라면
		if (InValue)
		{
			HeldBufferKeys.Add(HeldBufferKey, InValue); //보유한 키에 추가합니다
			LastStoredHeldBufferKey = HeldBufferKey; //마지막 버퍼키를 저장합니다

			//버퍼가 열려있지 않고 (그리고) 사용이라면
			if (!bIsBufferOpen && IsConsume)
			{
				if (OnInputBufferConsumed.IsBound())
					OnInputBufferConsumed.Broadcast(HeldBufferKey);
				ClearStoredBufferKeys();
			}
		}
		else //값이 거짓이라면
			HeldBufferKeys.Remove(HeldBufferKey); //보유한 키에서 지웁니다		
	}
}

bool UCCombatComponent::GetHeldBufferKeyValue(EBufferKey Key)
{
	if (HeldBufferKeys.Contains(Key))
		return *HeldBufferKeys.Find(Key);
	return false;
}

float UCCombatComponent::GetInputHeldTime(FKey Key)
{
	if (IsValid(GenericPlayerController))
		return GenericPlayerController->GetInputKeyTimeDown(Key);
	return 0.0f;
}

void UCCombatComponent::SetCharacterAttackType(EAttackTypes InAttackType)
{
	// EAttackTypes L_PrevAttackType = CurrentAttackType;
	CurrentAttackType = InAttackType;
}

void UCCombatComponent::ResetCombatState()
{
	//전투가 활성화 상태라면
	if (!StateManagerComponent->IsCombatDisabled())
		StateManagerComponent->SetState(ECharacterState::Idle);
}

void UCCombatComponent::SetFinisherMode(bool InFinisherMode)
{
	//입력받은 피니셔상태와 현재피니셔모드의 상태가 다르다면
	if (InFinisherMode != bFinisherMode)
	{
		bFinisherMode = InFinisherMode;
		if (OnFinisherModeChanged.IsBound())
			OnFinisherModeChanged.Broadcast(InFinisherMode);
	}
}

bool UCCombatComponent::CanPerformOptionalAbility(EOptionalCombatAbilities InOptionalAbility)
{
	if (OptionalCombatAbilities.Contains(InOptionalAbility))
		return *OptionalCombatAbilities.Find(InOptionalAbility);

	return false;
}

void UCCombatComponent::SetDodgeType(EDodgeType NewDodgeType)
{
	if (CurrentDodgeType != NewDodgeType)
		CurrentDodgeType = NewDodgeType;
}

EDodgeDirection UCCombatComponent::CalculateDirectionalDodge()
{
	//방향 회피 시스템에 대해 재생할 방향 및 애니메이션을 계산합니다.
	//데이터 테이블 폴더에 있는 데이터 테이블에서 8개의 모든 닷지 방향에 대한 애니메이션을 설정할 수 있습니다.

	//백터가 가르키는 방향에 해당하는 회전방향을 설정
	const FRotator LastVectorMakeRot = UKismetMathLibrary::Conv_VectorToRotator(OwnerCharacterRef->GetLastMovementInputVector());
	const FRotator OwnerRotation = GetOwner()->GetActorRotation(); //오너의 회전 값

	//정규화 시킨 Yaw 값을 가져옵니다
	const float L_DirectionalRollRotation =
		UKismetMathLibrary::NormalizedDeltaRotator(LastVectorMakeRot, OwnerRotation).Yaw;

	//구해온 Yaw값의 수치에 따라 방향을 설정합니다
	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, -30.f, 30.f))
		return EDodgeDirection::Forwards;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, 30.f, 60.f))
		return EDodgeDirection::FrontRight;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, 60.f, 120.f))
		return EDodgeDirection::Right;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, 120.f, 150.f))
		return EDodgeDirection::BackRight;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, -180.f, -150.f)
		|| UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, 150.f, 180.f))
		return EDodgeDirection::Backwards;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, -60.f, -30.f))
		return EDodgeDirection::FrontLeft;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, -120.f, -60.f))
		return EDodgeDirection::Left;

	if (UKismetMathLibrary::InRange_FloatFloat(L_DirectionalRollRotation, -150.f, -120.f))
		return EDodgeDirection::BackLeft;

	return EDodgeDirection::Forwards;
}

EDodgeDirection UCCombatComponent::SetDodgeDirection(bool OrientRotationToMovement)
{
	//움직임에 대한 회전 방향이 true인 경우 액터 회전을 설정하여 방향 회피를 설정합니다.

	if (OrientRotationToMovement)
		return EDodgeDirection::Forwards;

	// 이동 방향 회전이 false인 경우 8방향 닷지 애니메이션에 대한 닷지 방향 계산

	const EDodgeDirection L_Direction = CalculateDirectionalDodge(); //닷지의 방향을 결정합니다

	switch (L_Direction)
	{
	case EDodgeDirection::Forwards:
	case EDodgeDirection::BackLeft:
	case EDodgeDirection::BackRight:
	case EDodgeDirection::FrontLeft:
	case EDodgeDirection::FrontRight:
		return EDodgeDirection::Forwards;


	case EDodgeDirection::Backwards:
		return EDodgeDirection::Backwards;

	case EDodgeDirection::Right:
		return EDodgeDirection::Right;

	case EDodgeDirection::Left:
		return EDodgeDirection::Left;

	case EDodgeDirection::StepBack:
		return EDodgeDirection::StepBack;

	default: return EDodgeDirection::Forwards; //Default Break
	}
}

EGetupType UCCombatComponent::CalculateFacingDirection()
{
	//오너캐릭터 메쉬의 골반회전 값을 가져옵니다
	const FRotator PelvisRotator = GetMesh()->GetSocketRotation(FName(TEXT("pelvis")));
	const FVector PelvisRight = UKismetMathLibrary::GetRightVector(PelvisRotator);

	// 0 0 1 백터와 내적값을 구합니다
	const float DotResult = UKismetMathLibrary::Dot_VectorVector(PelvisRight, FVector(0.0f, 0.0f, 1.0f));

	//내적 값이 0.0f보다 
	if (DotResult >= 0.0f)
		return EGetupType::GetUpFacingDown; //위로 향해서

	return EGetupType::GetUpFacingDown; //아래를 향하며 일어난다 
}

void UCCombatComponent::StoreAttackInfo(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower,
                                        ECustomAttackTypes InCustomAttackTypes, bool InCauseKnockBack, float InKnockBackTime, float InKnockBackSpeed,
                                        bool InIsUnBlockAbleAttack, EAttackCategory InAttackCategory, ECombatType InCombatType, int32 InAttackIndex)
{
	if (IsValid(AttackInfo))
	{
		AttackInfo->StoreAttackInfo(InMeleeSwingDirection, InAttackPower, InCustomAttackTypes,
		                            InCauseKnockBack, InKnockBackTime, InKnockBackSpeed,
		                            InIsUnBlockAbleAttack, InAttackCategory, InCombatType, InAttackIndex);
	}
	else
	{
		UCAttackInfo* NewAttackInfo = Cast<UCAttackInfo>(UGameplayStatics::SpawnObject(UCAttackInfo::StaticClass(), GetOwner()));
		NewAttackInfo->StoreAttackInfo(InMeleeSwingDirection, InAttackPower, InCustomAttackTypes,
		                               InCauseKnockBack, InKnockBackTime, InKnockBackSpeed,
		                               InIsUnBlockAbleAttack, InAttackCategory, InCombatType, InAttackIndex);
		
		AttackInfo = NewAttackInfo;
	}
}

void UCCombatComponent::OnMontagesUpdated(FDataTableRowHandle InDataTableInfo)
{
	FCombatMontages* L_CombatMontage = InDataTableInfo.DataTable->FindRow<FCombatMontages>(InDataTableInfo.RowName, "");
	if (L_CombatMontage != nullptr)
	{
		OptionalCombatAbilities = L_CombatMontage->OptionalCombatAbilities;
		SetDodgeType(L_CombatMontage->DodgeDirection.DodgeType);
	}
}

void UCCombatComponent::StoreBasicAttackInfo(EGeneralCombatType AttackCategory, ECombatType CombatType, int32 AttackIndex)
{
	EAttackCategory BasicAttack = {};

	switch (AttackCategory)
	{
	case EGeneralCombatType::None:
		BasicAttack = EAttackCategory::Melee;
		break;

	case EGeneralCombatType::MeleeCombat:
		BasicAttack = EAttackCategory::Melee;
		break;

	case EGeneralCombatType::RangedCombat:
		BasicAttack = EAttackCategory::Ranged;
		break;

	case EGeneralCombatType::MagicCombat:
		BasicAttack = EAttackCategory::Magic;
		break;
	}

	if (IsValid(AttackInfo))
	{
		AttackInfo->StoreAttackInfo(EMeleeSwingDirection::Default, EAttackPower::Light, ECustomAttackTypes::None,
		                            false, 0.0f, 0.0f,
		                            false, BasicAttack, CombatType, AttackIndex);
	}
	else
	{
		UCAttackInfo* NewAttackInfo = Cast<UCAttackInfo>(UGameplayStatics::SpawnObject(UCAttackInfo::StaticClass(), GetOwner()));
		NewAttackInfo->StoreAttackInfo(EMeleeSwingDirection::Default, EAttackPower::Light, ECustomAttackTypes::None,
		                               false, 0.0f, 0.0f,
		                               false, BasicAttack, CombatType, AttackIndex);
		AttackInfo = NewAttackInfo;
	}
}

USkeletalMeshComponent* UCCombatComponent::GetMesh()
{
	return OwnerCharacterRef->GetMesh();
}

bool UCCombatComponent::IsFalling()
{
	return OwnerCharacterRef->GetCharacterMovement()->IsFalling();
}

UAnimInstance* UCCombatComponent::GetAnimInstance()
{
	return GetMesh()->GetAnimInstance();
}

UCharacterMovementComponent* UCCombatComponent::GetCharacterMovement()
{
	return OwnerCharacterRef->GetCharacterMovement();
}

bool UCCombatComponent::MontagePlaying(UAnimMontage* InMontage)
{
	return GetAnimInstance()->Montage_IsPlaying(InMontage);
}
