#include "CMontageManagerComponent.h"

#include "CCombatComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"


UCMontageManagerComponent::UCMontageManagerComponent()
{
	ConstructorHelpers::FObjectFinder<UDataTable> DT_CombatMontages_Src(TEXT("DataTable'/Game/Game_/Datas/DT_CombatMontages.DT_CombatMontages'"));
	if (DT_CombatMontages_Src.Succeeded())
		DataTable = DT_CombatMontages_Src.Object;
}

void UCMontageManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	CHECK(OwnerCharacter!=nullptr);
	CombatComponent = Cast<UCCombatComponent>(OwnerCharacter->GetComponentByClass(UCCombatComponent::StaticClass()));
	CHECK(CombatComponent!=nullptr);

	SetDefaultMontages(); //기본몽테이지 설정
}

FAnimMontageStruct UCMontageManagerComponent::GetGeneralActionMontage(ECharacterAction InCombatAction, int32 InMontageIndex, bool IsRandomIndex)
{
	FActionMontage L_ActionMontage;

	//기본 일반 액션 몽테이지가 있다면
	const bool L_SharedMontage = DefaultGeneralActionMontages.Contains(InCombatAction);

	if (L_SharedMontage)
		L_ActionMontage = *DefaultGeneralActionMontages.Find(InCombatAction);


	//일반 액션 몽테이지가 있다면
	const bool L_CombatTypeMontage = GeneralActionMontages.Contains(InCombatAction);
	if (L_CombatTypeMontage)
		L_ActionMontage = *GeneralActionMontages.Find(InCombatAction);


	//무기별 유형의 몽타주가 있다면
	const bool L_WeaponSpecific = WeaponActionMontages.Contains(InCombatAction);
	if (L_WeaponSpecific)
		L_ActionMontage = *WeaponActionMontages.Find(InCombatAction);


	//현재 전투컴포넌트가 퍼니셔모드이며 또한 퍼니셔몽테지가 있다면
	const bool L_FinisherModeAction =
		CombatComponent->IsFinisherMode() && FinisherModeActionMontages.Contains(InCombatAction);
	if (L_FinisherModeAction)
		L_ActionMontage = *FinisherModeActionMontages.Find(InCombatAction);


	//유형별 몽타주가 한 종류라도 있다면
	if (L_CombatTypeMontage || L_WeaponSpecific || L_FinisherModeAction || L_SharedMontage)
	{
		//일반 전투 액션 마지막 인덱스 설정
		GeneralCombatActionLastIndex = L_ActionMontage.ActionMontages.Num() - 1;

		//0~마지막인덱스까지 랜덤숫자를 설정
		const int32 L_RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, GeneralCombatActionLastIndex); //랜덤 인덱스 설정

		//랜덤이 참이라면 랜덤인덱스를 아니라면 몽테이지 인덱스를
		const int32 L_TargetIndex = UKismetMathLibrary::SelectInt(L_RandomIndex, InMontageIndex, IsRandomIndex);

		//현재 일반액션몽테이지 인덱스를 설정한다
		GeneralCombatActionMontageIndex = UKismetMathLibrary::SelectInt(GeneralCombatActionLastIndex, L_TargetIndex,
		                                                                L_TargetIndex > GeneralCombatActionLastIndex);

		//애니메이션몽테이지가 존재한다면
		if (IsValid(L_ActionMontage.ActionMontages[GeneralCombatActionMontageIndex].AnimMontage))
			return L_ActionMontage.ActionMontages[GeneralCombatActionMontageIndex];
	}

	return {};
}

FAnimMontageStruct UCMontageManagerComponent::GetAttackMontage(EAttackTypes InAttackType, int32 InAttackIndex, bool RandomIndex)
{
	FActionMontage L_AttackMontage;

	// 기본 공격 몽타주가 있다면
	const bool L_CommonMontage = DefaultAttackMontages.Contains(InAttackType);
	if (L_CommonMontage)
		L_AttackMontage = *DefaultAttackMontages.Find(InAttackType);


	//전투타입의 몽타주가 있다면
	const bool L_CombatTypeMontageFound = AttackMontages.Contains(InAttackType);
	if (L_CombatTypeMontageFound)
		L_AttackMontage = *AttackMontages.Find(InAttackType);


	//어택타입이 웨폰기술 어택이거나   (또는)			(전투모드가 피니셔모드가 아니고 (그리고) 기본 공격몽타주가 없다면)
	const bool L_WeaponAttackType = InAttackType == EAttackTypes::WeaponArtAttack || (!CombatComponent->IsFinisherMode() && !L_CommonMontage);
	//웨폰공격 몽테이지가 존재한다면
	const bool L_WeaponMontageFind = WeaponAttackMontages.Contains(InAttackType);
	const bool L_WeaponSpecific = L_WeaponMontageFind && L_WeaponAttackType; //웨폰기술 몽테이지가 있다면 (그리고) 어택의 조건이 가능하다면 
	if (L_WeaponSpecific)
		L_AttackMontage = *WeaponAttackMontages.Find(InAttackType);


	//피니셔 유형의 몽타주가 있고 (그리고) 컴뱃컴포넌트가 현재 피니셔모드라면
	const bool L_FinisherModeAction = CombatComponent->IsFinisherMode() && FinisherModeAttackMontages.Contains(InAttackType);
	if (L_FinisherModeAction)
		L_AttackMontage = *FinisherModeAttackMontages.Find(InAttackType);


	//유형별 몽타주가 한 종류라도 있다면
	if (L_CombatTypeMontageFound || L_WeaponSpecific || L_FinisherModeAction || L_CommonMontage)
	{
		AttackMontageLastIndex = L_AttackMontage.ActionMontages.Num() - 1; //공격 몽테이지의 마지막 인덱스를 설정

		const int32 L_RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, AttackMontageLastIndex); //랜덤 인덱스를 설정

		const int32 L_TargetIndex = UKismetMathLibrary::SelectInt(L_RandomIndex, InAttackIndex, RandomIndex); //타겟인덱스를 설정

		//현재 공격 인덱스로 설정
		CurrentAttackMontageIndex = UKismetMathLibrary::SelectInt(AttackMontageLastIndex, L_TargetIndex, L_TargetIndex > AttackMontageLastIndex);

		//마지막공격 몽테이지를 설정
		LastAttackMontage = L_AttackMontage.ActionMontages[CurrentAttackMontageIndex];

		//애니메이션몽테이가 유효하다면
		if (IsValid(LastAttackMontage.AnimMontage))
			return LastAttackMontage;
	}

	return {};
}

FAnimMontageStruct UCMontageManagerComponent::GetActionMontageFromItem(ECharacterAction InCombatAction, int32 MontageIndex, bool RandomIndex,
                                                                       const FItemSlot& InSlotItem)
{
	FItemMontage L_ItemActionMontage = CInventoryEquipmentLibrary::GetItemMontages(InSlotItem);

	if (L_ItemActionMontage.ActionMontages.Contains(InCombatAction))
	{
		const FActionMontage* L_ActionMontage = L_ItemActionMontage.ActionMontages.Find(InCombatAction); //아이템의 액션 몽테이지를 가져온다

		const int32 L_LastIndex = L_ActionMontage->ActionMontages.Num() - 1; //마지막 인덱스 설정
		const int32 L_RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, L_LastIndex); //랜덤 인덱스 설정

		//랜덤인덱스가 참이라면 RandomIndex를 아니라면 MontageIndex를 설정
		int32 TargetIndex = UKismetMathLibrary::SelectInt(L_RandomIndex, MontageIndex, RandomIndex);

		//타겟 인덱스가 라스트 인덱스보다 크다면 라스트인덱스로 설정 아니라면 타겟 인덱스로 설정
		TargetIndex = UKismetMathLibrary::SelectInt(L_LastIndex, TargetIndex, TargetIndex > L_LastIndex);

		//애님 몽테이지가 유효하다면 반환한다
		if (IsValid(L_ActionMontage->ActionMontages[TargetIndex].AnimMontage))
			return L_ActionMontage->ActionMontages[TargetIndex];
	}

	return {};
}

FAnimMontageStruct UCMontageManagerComponent::GetRollDodgeMontage(EDodgeDirection InDodgeDirection, int32 InMontageIndex)
{
	FActionMontage L_MontageStruct;

	//일반적인 구르기 회피가 있다면
	const bool L_CommonDodgeMontages = GetAllDefaultRollDodgeMontages().Contains(InDodgeDirection);
	if (L_CommonDodgeMontages)
		L_MontageStruct = *GetAllDefaultRollDodgeMontages().Find(InDodgeDirection);

	//전투 구르기 회피가 있다면
	const bool L_CombatTypeSpecific = GetAllCombatTypeRollDodgeMontages().Contains(InDodgeDirection);
	if (L_CombatTypeSpecific)
		L_MontageStruct = *GetAllCombatTypeRollDodgeMontages().Find(InDodgeDirection);


	//무기별 유형의 닷지가 있다면
	const bool L_WeaponSpecific = WeaponRollDodgeMontages.Contains(InDodgeDirection);
	if (L_WeaponSpecific)
		L_MontageStruct = *WeaponRollDodgeMontages.Find(InDodgeDirection);


	//하나의 유형이라도 있다면
	if (L_CommonDodgeMontages || L_CombatTypeSpecific || L_WeaponSpecific)
	{
		const int32 L_LastIndex = L_MontageStruct.ActionMontages.Num() - 1;
		const int32 L_TargetIndex = UKismetMathLibrary::SelectInt(L_LastIndex, InMontageIndex, InMontageIndex > L_LastIndex);

		//몽테이지가 유효하다면
		if (IsValid(L_MontageStruct.ActionMontages[L_TargetIndex].AnimMontage))
			return L_MontageStruct.ActionMontages[L_TargetIndex];
	}

	return {};
}

FAnimMontageStruct UCMontageManagerComponent::GetStepDodgeMontage(EDodgeDirection InDodgeDirection, int32 InMontageIndex)
{
	FActionMontage L_MontageStruct;

	//일반적인 유형의 스텝 회피가 있다면
	const bool L_CommonDodgeMontages = GetAllDefaultStepDodgeMontages().Contains(InDodgeDirection);
	if (L_CommonDodgeMontages)
		L_MontageStruct = *GetAllDefaultStepDodgeMontages().Find(InDodgeDirection);

	//전투 유형의 스텝 회피가 있다면
	const bool L_CombatTypeSpecific = GetAllCombatTypeStepDodgeMontages().Contains(InDodgeDirection);
	if (L_CombatTypeSpecific)
		L_MontageStruct = *GetAllCombatTypeStepDodgeMontages().Find(InDodgeDirection);


	//무기별 유형의 스텝 회피가 있다면
	const bool L_WeaponSpecific = WeaponStepDodgeMontage.Contains(InDodgeDirection);
	if (L_WeaponSpecific)
		L_MontageStruct = *WeaponStepDodgeMontage.Find(InDodgeDirection);


	//하나의 유형이라도 있다면
	if (L_CommonDodgeMontages || L_CombatTypeSpecific || L_WeaponSpecific)
	{
		const int32 LastIndex = L_MontageStruct.ActionMontages.Num() - 1; //마지막 인덱스를 설정
		StepDodgeIndex = UKismetMathLibrary::SelectInt(LastIndex, InMontageIndex, InMontageIndex > LastIndex); //스텝회피 인덱스를 설정

		//몽테이지가 유효하다면
		if (IsValid(L_MontageStruct.ActionMontages[StepDodgeIndex].AnimMontage))
			return L_MontageStruct.ActionMontages[StepDodgeIndex];
	}

	return {};
}

FAnimMontageStruct UCMontageManagerComponent::GetCounterAttackMontage(EHitFacingDirection InDirection)
{
	if (CounterAttacks.Contains(InDirection))
		if (IsValid(CounterAttacks.Find(InDirection)->AnimMontage))
			return *CounterAttacks.Find(InDirection);
	return {};
}

void UCMontageManagerComponent::SetDefaultMontages()
{
	CHECK(DataTable != nullptr);

	const FCombatMontages* L_DefaultCombatMontage = DataTable->FindRow<FCombatMontages>(FName(TEXT("Default")), "");
	if (L_DefaultCombatMontage != nullptr)
	{
		DefaultGeneralActionMontages = L_DefaultCombatMontage->GeneralActions;
		DefaultAttackMontages = L_DefaultCombatMontage->Attacks;
		DefaultRollDodgeMontages = L_DefaultCombatMontage->DodgeDirection.RollDodgeMontages;
		DefaultStepDodgeMontages = L_DefaultCombatMontage->DodgeDirection.StepDodgeMontages;
	}
}

void UCMontageManagerComponent::ResetAttackMontageIndex()
{
	AttackMontageLastIndex = 0;
	CurrentAttackMontageIndex = 0;
}

void UCMontageManagerComponent::UpdateMontages(ECombatType InCombatType, FName InCustomRowName, bool UseCustomRowName)
{
	//컴벳타입이 None 이라면 모두 클리어한다
	if (InCombatType == ECombatType::None)
	{
		GeneralActionMontages.Empty();
		AttackMontages.Empty();
		FinisherModeActionMontages.Empty();
		CombatTypeRollDodgeMontages.Empty();
		CombatTypeStepDodgeMontages.Empty();
		CounterAttacks.Empty();
	}
	else //컴벳타입이 None 이 아니라면
	{
		CHECK(DataTable!=nullptr);

		//커스텀네임의 사용에 따라서 로우네임을 설정합니다
		FName L_RowName;
		UseCustomRowName ? L_RowName = InCustomRowName : L_RowName = *CRPGLibrary::GetCombatTypeAsString(InCombatType);

		//데이터테이블의 값을 찾는다
		const FCombatMontages* L_RowNameCombatMontages = DataTable->FindRow<FCombatMontages>(L_RowName, "");

		//데이터 테이블에서 값을 찾았다면
		if (L_RowNameCombatMontages != nullptr)
		{
			GeneralActionMontages = L_RowNameCombatMontages->GeneralActions;
			AttackMontages = L_RowNameCombatMontages->Attacks;
			CombatTypeRollDodgeMontages = L_RowNameCombatMontages->DodgeDirection.RollDodgeMontages;
			CombatTypeStepDodgeMontages = L_RowNameCombatMontages->DodgeDirection.StepDodgeMontages;
			FinisherModeActionMontages = L_RowNameCombatMontages->FinisherModeMontages.GeneralActions;
			FinisherModeAttackMontages = L_RowNameCombatMontages->FinisherModeMontages.Attacks;
			CounterAttacks = L_RowNameCombatMontages->CounterAttacks;

			if (OnMontagesUpdated.IsBound())
			{
				FDataTableRowHandle BroadHandle;
				BroadHandle.DataTable = DataTable;
				BroadHandle.RowName = L_RowName;
				OnMontagesUpdated.Broadcast(BroadHandle);
			}
		}
		else //데이터테이블에서 값을 찾지 못했다면
		{
			FString L_PrintString = L_RowName.ToString();
			L_PrintString.Append("RowName Not Find");
			UKismetSystemLibrary::PrintString(this, L_PrintString, true, true, FLinearColor::Red, 5.f);
		}
	}
}

void UCMontageManagerComponent::UpdateWeaponMontages(const FItemMontage& WeaponMontages)
{
	WeaponActionMontages = WeaponMontages.ActionMontages;
	WeaponAttackMontages = WeaponMontages.AttackMontages;
	WeaponRollDodgeMontages = WeaponMontages.RollDodgeMontages;
	WeaponStepDodgeMontage = WeaponMontages.StepDodgeMontages;
}
