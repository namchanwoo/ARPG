#include "CRPGCharacter.h"

#include <ThirdParty/CryptoPP/5.6.5/include/randpool.h>

#include "CCombatPlayerController.h"
#include "Global.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Interface_AnimInstance.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Systems/CombatSystem/CCharacterStateManagerComponent.h"
#include "Systems/CombatSystem/CCombatComponent.h"
#include "Systems/CombatSystem/CEquipmentManagerComponent.h"
#include "Systems/CombatSystem/CMontageManagerComponent.h"
#include "Systems/CombatSystem/CStatsManagerComponent.h"
#include "Systems/CombatSystem/CustomCollisionComponent.h"
#include "Systems/ItemSystem/Actors/CBaseArcheryWeapon.h"


ACRPGCharacter::ACRPGCharacter()
{
	// 컨트롤러가 회전할 때 회전하지 않습니다
	bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<UObject> PM_Flesh_Src(
		TEXT("PhysicalMaterial'/Game/Game_/Systems/CombatSystem/PhysicalMaterials/PM_Flesh.PM_Flesh'"));

	//CharacterMovement Setting
	{
		GetCharacterMovement()->GravityScale = 1.7f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 300.0f;
		GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
		GetCharacterMovement()->JumpZVelocity = 600.0f;
		GetCharacterMovement()->AirControl = 0.7f;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->NavAgentProps.AgentRadius = 42.0f;
		GetCharacterMovement()->NavAgentProps.AgentHeight = 192.0f;
		GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	}

	//Capsule Component Setting 
	{
		GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		GetCapsuleComponent()->AreaClass = UNavArea_Obstacle::StaticClass();
		GetCapsuleComponent()->ComponentTags.Add(TAG_PAWNCAPSULE);
		if (PM_Flesh_Src.Succeeded())
			GetCapsuleComponent()->SetPhysMaterialOverride((UPhysicalMaterial*)PM_Flesh_Src.Object);
	}

	//Mesh Setting
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
		if (PM_Flesh_Src.Succeeded())
			GetMesh()->SetPhysMaterialOverride((UPhysicalMaterial*)PM_Flesh_Src.Object);
	}

	//컴포넌트 생성
	{
		//CameraBoom
		{
			CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
			CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 97.0f));
			CameraBoom->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
			CameraBoom->TargetArmLength = 370.0f;
			CameraBoom->SocketOffset = FVector(0.0f, 10.0f, 60.0f);
			CameraBoom->bUsePawnControlRotation = true; //Pawn의 회전을 사용합니다
			CameraBoom->bEnableCameraLag = true; //카메라 레그를 사용
			CameraBoom->CameraLagSpeed = 5.0f; //레그 스피드 설정
			CameraBoom->SetupAttachment(GetMesh());
		}

		//FollowCamera
		{
			FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
			FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		}

		//LockOn Widget
		{
			LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
			LockOnWidget->SetRelativeLocation(FVector(0.5f, 7.5f, 120.0f));
			LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
			LockOnWidget->SetDrawSize(FVector2D(14.0f, 14.0f));
			LockOnWidget->SetVisibility(false);
			LockOnWidget->SetupAttachment(GetMesh());

			static ConstructorHelpers::FClassFinder<UUserWidget> LockOnWidgetClass_Src(
				TEXT("WidgetBlueprint'/Game/Game_/Widgets/CombatUI/WB_LockOn.WB_LockOn_C'"));
			if (LockOnWidgetClass_Src.Succeeded())
				LockOnWidget->SetWidgetClass(LockOnWidgetClass_Src.Class);
		}

		//LockOnCollision
		{
			LockOnCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LockOnCollision"));
			LockOnCollision->SetSphereRadius(1500.0f);
			LockOnCollision->AreaClass = UNavArea_Obstacle::StaticClass();
			LockOnCollision->SetVisibility(false);
			LockOnCollision->SetCollisionProfileName(FName(TEXT("Custom")));
			LockOnCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
			LockOnCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			LockOnCollision->SetCollisionResponseToChannel(ECC_Interactable, ECR_Block);
			LockOnCollision->ComponentTags.Add(TAG_LOCKONSPHERE);
			LockOnCollision->SetupAttachment(GetRootComponent());
		}

		//BossStealthKill
		{
			BossStealthKill = CreateDefaultSubobject<UBoxComponent>(TEXT("BossStealthKill"));
			LockOnCollision->AreaClass = UNavArea_Obstacle::StaticClass();
			BossStealthKill->SetVisibility(false);
			BossStealthKill->SetupAttachment(GetRootComponent());
		}


		//StatsManagerComponent
		StatsManagerComponent = CreateDefaultSubobject<UCStatsManagerComponent>(TEXT("StatsManagerComponent"));
		//CharacterStateManagerComponent
		CharacterStateManager = CreateDefaultSubobject<UCCharacterStateManagerComponent>(TEXT("CharacterStateManagerComponent"));
		//CombatComponent
		CombatComponent = CreateDefaultSubobject<UCCombatComponent>(TEXT("CombatComponent"));
		//MontageManagerComponent
		MontageManagerComponent = CreateDefaultSubobject<UCMontageManagerComponent>(TEXT("MontageManagerComponent"));
		//CollisionComponent
		CollisionComponent = CreateDefaultSubobject<UCustomCollisionComponent>(TEXT("CollisionComponent"));
		CollisionComponent->SetTraceRadius(20.0f);

		//EquipmentComponent
		EquipmentComponent = CreateDefaultSubobject<UCEquipmentManagerComponent>(TEXT("EquipmentComponent"));
	}


	// Movement Members Setting
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Combat Members Setting
	bCheckFocusState = true;
}

void ACRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	//몽타주 인스턴스가 종료될 때 호출되는 델리게이트를 바인딩합니다
	GetAnimInstance()->OnAllMontageInstancesEnded.AddDynamic(this, &ACRPGCharacter::EventOnMontageInstancesEnd);

	//델리게이트 바인딩합니다
	{
		CHECK(MontageManagerComponent!=nullptr);
		MontageManagerComponent->OnMontagesUpdated.AddDynamic(this, &ACRPGCharacter::EventOnMontagesUpdated);

		CHECK(CombatComponent!=nullptr);
		CombatComponent->OnInputBufferConsumed.AddDynamic(this, &ACRPGCharacter::EventOnInputBufferConsumed);
		CombatComponent->OnInputBufferOpen.AddDynamic(this, &ACRPGCharacter::EventOnInputBufferOpen);
		CombatComponent->OnInputBufferClose.AddDynamic(this, &ACRPGCharacter::EventOnInputBufferClose);

		CHECK(CharacterStateManager!=nullptr);
		CharacterStateManager->OnMovementCancelSet.AddDynamic(this, &ACRPGCharacter::EventOnMovementCancelSet);
		CharacterStateManager->OnStateBegin.AddDynamic(this, &ACRPGCharacter::EventOnStateBegin);
		CharacterStateManager->OnStateEnd.AddDynamic(this, &ACRPGCharacter::EventOnStateEnd);
		CharacterStateManager->OnActionBegin.AddDynamic(this, &ACRPGCharacter::EventOnActionBegin);
		CharacterStateManager->OnActionEnd.AddDynamic(this, &ACRPGCharacter::EventOnActionEnd);
		CharacterStateManager->OnOwnerReset.AddDynamic(this, &ACRPGCharacter::EventOnOwnerReset);
		CharacterStateManager->OnBlockStateSet.AddDynamic(this, &ACRPGCharacter::EventOnBlockStateSet);

		CHECK(StatsManagerComponent!=nullptr);
		StatsManagerComponent->OnCurrentValueUpdated.AddUniqueDynamic(this, &ACRPGCharacter::EventOnCurrentStatValueUpdated);

		CHECK(EquipmentComponent!=nullptr);
		EquipmentComponent->OnCombatTypeChanged.AddUniqueDynamic(this, &ACRPGCharacter::EventOnOnCombatTypeChanged);
		EquipmentComponent->OnCombatTypeEnd.AddUniqueDynamic(this, &ACRPGCharacter::EventOnCombatTypeEnd);
	}


	//다른 함수들의 로직을 위한 준비
	{
		DodgeAfterLogicLatenInfo.CallbackTarget = this;
		DodgeAfterLogicLatenInfo.ExecutionFunction = TEXT("DodgeAfter");
		DodgeAfterLogicLatenInfo.Linkage = 1;
		DodgeAfterLogicLatenInfo.UUID = LatentAction++;
	}

	EquipmentComponent->SetCombatType(ECombatType::Unarmed);
	ResetCharacterStates();

	//플레이어 컨트롤러 설정
	CombatPlayerController = Cast<ACCombatPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StatsManagerComponent->InitializeStatsComponent();
	StatsManagerComponent->InitializeAttributes();

	EquipmentComponent->InitializeEquipmentComponent();
}

bool ACRPGCharacter::CanJumpInternal_Implementation() const
{
	TArray<ECharacterState> CanStates;
	CanStates.Add(ECharacterState::Idle);
	CanStates.Add(ECharacterState::Aiming);
	CanStates.Add(ECharacterState::Blocking);

	return CanStates.Contains(CharacterStateManager->GetCurrentState()) ||
		CharacterStateManager->GetCancelFrames();
}

void ACRPGCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//원거리형 전투유형이 아니라면 애님몽테이지를 중지합니다
	if (EquipmentComponent->GetGeneralCombatType() != EGeneralCombatType::RangedCombat)
		StopAnimMontage();

	//근접공격을 초기화합니다
	ResetMeleeAttack();

	TArray<ECharacterState> L_IdleMaps;
	L_IdleMaps.Add(ECharacterState::Aiming);
	L_IdleMaps.Add(ECharacterState::Blocking);
	L_IdleMaps.Add(ECharacterState::FocusedState);

	//위 상태에 현재상태가 포함되어 있지 않다면
	if (!L_IdleMaps.Contains(GetCurrentCharacterState()))
		CharacterStateManager->SetState(ECharacterState::Idle);

	//추락 데미지를 받아야한다면
	if (bFallDamage)
	{
		StatsManagerComponent->ModifyCurrentStatValue(EStats::Health, -50.0f);
		I_FallDamage(false);
	}
}

void ACRPGCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	SpawnSoulTransform = GetMesh()->GetComponentTransform();
}

void ACRPGCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
                                                      const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);
	SpawnSoulTransform = FTransform(GetActorRotation(), PreviousLocation);
}

void ACRPGCharacter::EventOnOnCombatTypeChanged(ECombatType InCombatType)
{
	MontageManagerComponent->UpdateMontages(InCombatType);

	if (!CanPerformOptionalAbility(EOptionalCombatAbilities::FinisherMode))
		CombatComponent->SetFinisherMode(false);

	switch (InCombatType)
	{
	case ECombatType::None: break;
	case ECombatType::Unarmed: break;
	case ECombatType::LightSword: break;
	case ECombatType::GreatSword: break;
	case ECombatType::LightSwordAndShield: break;
	case ECombatType::Axe: break;
	case ECombatType::Bow: break;
	case ECombatType::Spear: break;
	case ECombatType::Katana: break;
	case ECombatType::DualSwords: break;
	case ECombatType::Twinblades:
		CombatComponent->SetFinisherMode(false);
		break;
	case ECombatType::UnarmedCombatAndShield: break;
	case ECombatType::AxeAndShield: break;
	case ECombatType::Crossbow: break;
	default: break;
	}
}

void ACRPGCharacter::EventOnCombatTypeEnd(ECombatType InCombatType)
{
}


void ACRPGCharacter::ResetChargeAttack()
{
	UKismetSystemLibrary::K2_ClearTimerHandle(this, LightChargeAttackHandle);
	AttackHeldTime = 0.0f;
	ChargeAttackCount = 0;
	if (bStrongChargeAttackWindow)
	{
		I_EndStrongChargeAttack();
		bStrongChargeAttackWindow = false;
	}
}

void ACRPGCharacter::LightChargeAttackTimerSetOn(const FKey& InAttackKey)
{
	AttackHeldTime = CombatPlayerController->GetInputKeyTimeDown(InAttackKey);
	if (AttackHeldTime >= ChargeAttackHoldTime)
	{
		UKismetSystemLibrary::K2_ClearTimerHandle(this, LightChargeAttackHandle);
		if (EquipmentComponent->IsCombatEnabled())
			CombatComponent->StoreAttackBufferKey(EAttackTypes::LightChargeAttack);
	}
}

void ACRPGCharacter::CheckLightChargeAttack(const FKey& InAttackKey)
{
	ResetChargeAttack();
	static FTimerDelegate LightChargeAttackTimerSetOn_Del;
	LightChargeAttackTimerSetOn_Del.BindUFunction(this, FName("LightChargeAttackTimerSetOn"), InAttackKey);
	GetWorld()->GetTimerManager().SetTimer(LightChargeAttackHandle, LightChargeAttackTimerSetOn_Del, 0.016, true);
}

bool ACRPGCharacter::CanPerformOptionalAbility(EOptionalCombatAbilities InOptionalActivity)
{
	return CombatComponent->CanPerformOptionalAbility(InOptionalActivity);
}

bool ACRPGCharacter::CanEnterFocusState()
{
	const bool L_CanState = IsCharacterStateEqual(ECharacterState::Idle) || IsCharacterStateEqual(ECharacterState::Blocking);
	return CanPerformOptionalAbility(EOptionalCombatAbilities::FocusState) && L_CanState;
}

bool ACRPGCharacter::IsActionEqual(ECharacterAction CompareAction)
{
	return CharacterStateManager->GetCurrentAction() == CompareAction;
}

bool ACRPGCharacter::IsActionNotEqual(ECharacterAction CompareAction)
{
	return CharacterStateManager->GetCurrentAction() != CompareAction;
}

bool ACRPGCharacter::CheckChainAttack(EAttackTypes InAttackType)
{
	if (CanPerformOptionalAbility(EOptionalCombatAbilities::ChainAttack)) //체인어택이 가능하다면
		if (InAttackType == EAttackTypes::StrongAttack) //공격 유형이 강한 공격라면
			if (IsAttackTypeEqual(EAttackTypes::LightAttack)) //공격 유형이 가벼운 공격이라면
				if (AttackCount == 2) //어택카운트가 2라면
				{
					CombatComponent->StoreAttackBufferKey(EAttackTypes::ChainAttack);
					return true;
				}
	return false;
}

bool ACRPGCharacter::SelectDesiredAttackType(EAttackTypes InAttackType)
{
	//캐릭터상태가 집중중인 상태라면
	if (IsCharacterStateEqual(ECharacterState::FocusedState))
	{
		//최소 마나가 가 5.0f 보다 크거나 같다면
		if (HasEnoughOfStat(EStats::Mana, 5.0f))
		{
			//공격 버퍼 키를 WeaponArtAttack를 저장하고 true로 반환
			CombatComponent->StoreAttackBufferKey(EAttackTypes::WeaponArtAttack);
			return true;
		}
	}
	else //캐릭터상태가 집중중인 상태가 아니라면
	{
		//특별한 액션키가 눌러져 있고 (그리고) 포커스상태이고 (그리고) 포커스 상태에 진입 가능할 떄
		if (bIsSpecialActionPressed && bCheckFocusState && CanEnterFocusState())
		{
			CombatComponent->StoreBufferKey(EBufferKey::ToggleFocusMode); //포커스 모드토글 키를 키버퍼에 저장한다
			if (IsCharacterStateEqual(ECharacterState::FocusedState)) //현재 캐릭터 상태가 포커스 상태라면
				if (HasEnoughOfStat(EStats::Mana, 5.0f)) //최소 마나가 가 5.0f 보다 크거나 같다면
				{
					CombatComponent->StoreAttackBufferKey(EAttackTypes::WeaponArtAttack); //공격 버퍼 키를 WeaponArtAttack를 저장하고 true로 반환
					return true;
				}
		}
	}


	//닷지공격이 가능하다면
	if (CanPerformOptionalAbility(EOptionalCombatAbilities::DodgeAttack))
	{
		//현재 캐릭터의 상태가 스텝회피 상태라면
		if (IsCharacterStateEqual(ECharacterState::StepDodging))
		{
			//버퍼가 오픈 상태라면
			if (CombatComponent->IsBufferOpen())
			{
				CombatComponent->StoreAttackBufferKey(EAttackTypes::StepDodgeAttack);
				return true;
			}
		}

		//현재 캐릭터의 상태가 회피 상태라면
		if (IsCharacterStateEqual(ECharacterState::Dodging))
		{
			//버퍼가 오픈 상태라면
			if (CombatComponent->IsBufferOpen())
			{
				CombatComponent->StoreAttackBufferKey(EAttackTypes::RollDodgeAttack);
				return true;
			}
		}
	}


	//현재 캐릭터의 액션상태가 웨폰차지공격상태라면 (그리고) 버퍼가 열려 있다면
	if (IsActionEqual(ECharacterAction::WeaponArtCharge) && CombatComponent->IsBufferOpen())
	{
		CombatComponent->StoreAttackBufferKey(EAttackTypes::WeaponArtAttack);
		return true;
	}


	//연속공격을 체크해서 연속공격이라면
	if (CheckChainAttack(InAttackType))
		return true;


	//새로운 공격 타입이 가벼운공격과 같고 (그리고) 현재 공격 타입이 강한공격이라면
	const bool L_ResetAttackCount1 = InAttackType == EAttackTypes::LightAttack && IsAttackTypeEqual(EAttackTypes::StrongAttack);

	//새로운 공격 타입이 강한공격과 같고 (그리고) 현재 공격 타입이 가벼운공격 이라면
	const bool L_ResetAttackCount2 = (InAttackType == EAttackTypes::StrongAttack) && (IsAttackTypeEqual(EAttackTypes::LightAttack));

	if (L_ResetAttackCount1 || L_ResetAttackCount2)
		AttackCount = 0;

	CombatComponent->StoreAttackBufferKey(InAttackType);
	return false;
}

/////////////////////////////////Input///////////////////////
void ACRPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACRPGCharacter::Event_JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACRPGCharacter::Event_JumpReleased);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ACRPGCharacter::Event_RollPressed);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &ACRPGCharacter::Event_RollReleased);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ACRPGCharacter::Event_LightAttackPressed);
	PlayerInputComponent->BindAction("LightAttack", IE_Released, this, &ACRPGCharacter::Event_LightAttackReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACRPGCharacter::Event_SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACRPGCharacter::I_StopSprinting);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACRPGCharacter::Event_Crouch);
	PlayerInputComponent->BindAction("EquipItem", IE_Pressed, this, &ACRPGCharacter::Event_EquipItem);
	PlayerInputComponent->BindAction("ToggleWalk", IE_Pressed, this, &ACRPGCharacter::ToggleWalk);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACRPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACRPGCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ACRPGCharacter::EventOnCurrentStatValueUpdated(EStats InStatUpdated, float InCurrentValue, float InMaxValue)
{
	if (InStatUpdated == EStats::Health)
	{
		if (InCurrentValue <= 0.0f)
		{
			StatsManagerComponent->OnDeath();
			CharacterStateManager->SetDead(true);
			if (IsCharacterStateNotEqual(ECharacterState::Executed_Fatality))
				CharacterStateManager->SetState(ECharacterState::Dead);
		}
	}
}

bool ACRPGCharacter::HasEnoughOfStat(EStats InStat, float MinimumValue)
{
	return StatsManagerComponent->GetCurrentValueForStat(InStat) >= MinimumValue;
}

void ACRPGCharacter::ResetCharacterStates()
{
	CharacterStateManager->SetState(ECharacterState::Idle);
	CharacterStateManager->SetAction(ECharacterAction::None);
	CombatComponent->SetCharacterAttackType(EAttackTypes::None);
}

bool ACRPGCharacter::IsCharacterStateEqual(ECharacterState InState)
{
	return CharacterStateManager->GetCurrentState() == InState;
}

bool ACRPGCharacter::IsCharacterStateNotEqual(ECharacterState InState)
{
	return CharacterStateManager->GetCurrentState() != InState;
}

ECharacterState ACRPGCharacter::GetCurrentCharacterState()
{
	CHECK(CharacterStateManager!=nullptr, ECharacterState::Idle);
	return CharacterStateManager->GetCurrentState();
}

EGeneralCombatType ACRPGCharacter::GetCurrentGeneralCombat()
{
	CHECK(EquipmentComponent!=nullptr, EGeneralCombatType::None);
	return EquipmentComponent->GetGeneralCombatType();
}

void ACRPGCharacter::EventOnMontageInstancesEnd()
{
	CharacterStateManager->SetAction(ECharacterAction::None);
	CombatComponent->SetCharacterAttackType(EAttackTypes::None);
	ResetMeleeAttack();
	AttemptToEnterStates();

	//현재 전투유형이 원거리전투라면
	if (GetCurrentGeneralCombat() == EGeneralCombatType::RangedCombat)
	{
		if (!EquipmentComponent->GetArcheryWeaponEquipped()->IsArrowMeshVisible())
		{
			//나중에 궁수로직을 설계할 때 꼭 체크합니다
		}
	}
}

float ACRPGCharacter::PlayHighPriorityMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InStartMontageTime,
                                              EMontagePlayReturnType MontageReturn,
                                              bool IsStopAllMontages)
{
	if (IsValid(MontageToPlay))
	{
		return GetAnimInstance()->Montage_Play(MontageToPlay, InPlayRate, MontageReturn, InStartMontageTime, IsStopAllMontages);
	}
	else
	{
		CharacterStateManager->ResetState();
		return 0.0f;
	}
}

bool ACRPGCharacter::IsMontagePlaying(UAnimMontage* InMontage)
{
	return GetAnimInstance()->Montage_IsPlaying(InMontage);
}

bool ACRPGCharacter::IsAnyMontagePlaying()
{
	return GetAnimInstance()->IsAnyMontagePlaying();
}

void ACRPGCharacter::EventOnMontagesUpdated(FDataTableRowHandle InDataTableInfo)
{
	CombatComponent->OnMontagesUpdated(InDataTableInfo);
	//히트리액션컴포넌트 로직을 추가합니다
}


void ACRPGCharacter::MoveForward(float Value)
{
	//죽음상태가 아니라면
	if (!IsDead())
	{
		FVector L_Forward;
		FVector L_Right;
		GetControlDirection(L_Forward, L_Right);
		AddMovementInput(L_Forward, Value);
	}
}

void ACRPGCharacter::MoveRight(float Value)
{
	//죽음상태가 아니라면
	if (!IsDead())
	{
		FVector L_Forward;
		FVector L_Right;
		GetControlDirection(L_Forward, L_Right);
		AddMovementInput(L_Right, Value);
	}
}

void ACRPGCharacter::GetControlDirection(FVector& OutForwardVector, FVector& OutRightVector)
{
	const float ControlYaw = GetControlRotation().Yaw;
	const FRotator DirRotator = FRotator(0.0f, ControlYaw, 0.0f);
	OutForwardVector = UKismetMathLibrary::GetForwardVector(DirRotator);
	OutRightVector = UKismetMathLibrary::GetRightVector(DirRotator);
}

void ACRPGCharacter::ToggleWalk()
{
	//걷기토글이 아니라면
	if (!bToggleWalk)
	{
		bToggleWalk = true;
		if (IsGaitEqual(EGait::Run) || IsGaitEqual(EGait::Sprint))
			StatsManagerComponent->SetGait(EGait::Walk);
	}
	else //걷기토글 이라면
	{
		bToggleWalk = false;
		if (IsGaitEqual(EGait::Walk) && IsGaitNotEqual(EGait::Blocking))
			StatsManagerComponent->SetGait(EGait::Run);
	}
}

void ACRPGCharacter::Event_LightAttackPressed(FKey InAttackKey)
{
	if (OnGeneralCombatTypeEqual(EGeneralCombatType::MeleeCombat))
	{
		CombatComponent->StoreHeldBufferKey(EBufferKey::Attack, true, false);
		CombatComponent->StoreHeldBufferKey(EBufferKey::Aim, true, false);

		if (OnAttackTypeEqual(EAttackTypes::LightChargeAttack))
		{
			if (OnGeneralCombatTypeEqual(EGeneralCombatType::MeleeCombat))
				SelectDesiredAttackType(EAttackTypes::LightAttack);
			else
				CheckLightChargeAttack(InAttackKey);
		}
	}
}

void ACRPGCharacter::Event_LightAttackReleased()
{
	ResetChargeAttack();
	CombatComponent->StoreHeldBufferKey(EBufferKey::Attack, false, true);
	CombatComponent->StoreHeldBufferKey(EBufferKey::Aim, false, false);
	if (OnAttackTypeNotEqual(EAttackTypes::LightChargeAttack))
		if (OnGeneralCombatTypeEqual(EGeneralCombatType::MeleeCombat))
			SelectDesiredAttackType(EAttackTypes::LightAttack);
}

void ACRPGCharacter::Event_SprintPressed()
{
	//질주가 가능하다면
	if (CanSprint())
	{
		UnCrouch();

		//현재 캐릭터 상태가 스텝회피라면 몽테이지를 중지한다
		if (IsCharacterStateEqual(ECharacterState::StepDodging))
			StopAnimMontage();

		StatsManagerComponent->SetGait(EGait::Sprint);

		bToggleWalk = false;
		bIsSprinting = true;

		IInterface_AnimInstance* CastAnimInterface = Cast<IInterface_AnimInstance>(GetAnimInstance());
		CHECK(CastAnimInterface!=nullptr);
		CastAnimInterface->I_IsSprinting(bIsSprinting);

		//타겟팅컴포넌트 로직추가

		StatsManagerComponent->ModifyCurrentStatValue(EStats::Stamina, -1.0f, true, true);
		UKismetSystemLibrary::K2_SetTimer(StatsManagerComponent,TEXT("SprintStaminaCost"), 0.1f, true);
		BlockReleased();
	}
	else //질주가 가능하지 않다면
	{
		I_StopSprinting();
	}
}

void ACRPGCharacter::Event_JumpPressed()
{
	UnCrouch();
	CombatComponent->StoreBufferKey(EBufferKey::Jump);
}

void ACRPGCharacter::Event_JumpReleased()
{
	StopJumping();
	I_StopSprinting();
}

void ACRPGCharacter::Event_Crouch()
{
	if (!GetCharacterMovement()->IsCrouching())
		Crouch();
	else
		UnCrouch();
}

void ACRPGCharacter::Event_RollPressed(FKey InKey)
{
	RollKey = InKey;

	//캐릭터가 넉다운되어 있다면
	if (IsCharacterStateEqual(ECharacterState::KnockDown))
	{
		CombatComponent->StoreBufferKey(EBufferKey::GetUpRoll);
		ResetDodgeHeldTimer();
	}
	else //캐릭터가 넉다운 되어있지 않다면
	{
		switch (OnDodgeType())
		{
		case EDodgeType::RollDodge:
			if (HasMovementInput())
			{
				UKismetSystemLibrary::K2_SetTimer(this,TEXT("DodgeHeldTimer"), 0.0167f, true);
				UKismetSystemLibrary::RetriggerableDelay(this, 0.45f, DodgeAfterLogicLatenInfo);
			}
			else
			{
				CombatComponent->StoreBufferKey(EBufferKey::RollDodge);
				ResetDodgeHeldTimer();
			}
			break;

		case EDodgeType::StepDodge:
			CombatComponent->StoreBufferKey(EBufferKey::StepDodge);
			if (HasMovementInput())
			{
				UKismetSystemLibrary::K2_SetTimer(this,TEXT("DodgeHeldTimer"), 0.0167f, true);
				UKismetSystemLibrary::RetriggerableDelay(this, 0.45f, DodgeAfterLogicLatenInfo);
			}
			else
			{
				ResetDodgeHeldTimer();
			}
			break;

		case EDodgeType::Both:

			DetectDoubleClick(
				[this]()
				{
					CombatComponent->StoreBufferKey(EBufferKey::RollDodge);
					ResetDodgeHeldTimer();
				},
				[this]()
				{
					CombatComponent->StoreBufferKey(EBufferKey::StepDodge);
					if (HasMovementInput())
					{
						UKismetSystemLibrary::K2_SetTimer(this,TEXT("DodgeHeldTimer"), 0.0167f, true);
						UKismetSystemLibrary::RetriggerableDelay(this, 0.45f, DodgeAfterLogicLatenInfo);
					}
					else
					{
						ResetDodgeHeldTimer();
					}
				});
			break;
		}
	}
}

void ACRPGCharacter::Event_RollReleased()
{
	UKismetSystemLibrary::K2_ClearTimer(this,TEXT("DodgeHeldTimer"));

	//현재 질주중이라면
	if (bIsSprinting)
	{
		I_StopSprinting();
		ResetDodgeHeldTimer();
	}
	else //현재 질주중이 아니고
	{
		//현재 회피 타입이 구르기 (그리고) 이전의 입력값이 있고 (그리고) 회피를 누른시간이 0.45f 보다 작다면
		if (OnDodgeType() == EDodgeType::RollDodge && HasMovementInput() && DodgeHeldTime <= 0.45f)
		{
			CombatComponent->StoreBufferKey(EBufferKey::RollDodge);
			ResetDodgeHeldTimer();
		}
		else
		{
			ResetDodgeHeldTimer();
		}
	}
}

void ACRPGCharacter::Event_EquipItem()
{
	CombatComponent->StoreBufferKey(EBufferKey::ToggleCombatMode);
}

void ACRPGCharacter::DodgeAfter()
{
	//회피키를 누른시간이 0.4초보다 같거나 크다면
	if (DodgeHeldTime >= 0.4f)
		Event_SprintPressed();
	else //회피키를 누른시간이 0.4 초보다 작다면
	{
		I_StopSprinting();
		ResetDodgeHeldTimer();
	}
}

void ACRPGCharacter::AttemptMovementCancel()
{
	//이전 움직임입력값이 있고 (그리고) 몽테이지가 재생중이라면
	if (HasMovementInput() && IsAnyMontagePlaying())
	{
		//이동시도 입력을 취소할수 있다면 몽테이지를 중지합니다
		if (CanCancelIntoMovementInput())
		{
			StopAnimMontage();
			StopAnimMontage();
		}
	}
}

bool ACRPGCharacter::PerformDodge(EDodgeType InDodgeType, EDodgeDirection InDodgeDirection, int32 InMontageIndex)
{
	//회피 유형에 따른 회피몽테이지를 가져옵니다
	FAnimMontageStruct L_DodgeAnimMontage;
	if (InDodgeType == EDodgeType::RollDodge)
		L_DodgeAnimMontage = MontageManagerComponent->GetRollDodgeMontage(InDodgeDirection, InMontageIndex);
	else if (InDodgeType == EDodgeType::StepDodge)
		L_DodgeAnimMontage = MontageManagerComponent->GetStepDodgeMontage(InDodgeDirection, InMontageIndex);


	//몽테이지가 유효하다면
	if (IsValid(L_DodgeAnimMontage.AnimMontage))
	{
		if (InDodgeType == EDodgeType::RollDodge)
		{
			CharacterStateManager->SetState(ECharacterState::Dodging);
			CharacterStateManager->SetAction(ECharacterAction::RollDodge);
		}
		else if (InDodgeType == EDodgeType::StepDodge)
		{
			CharacterStateManager->SetAction(ECharacterAction::StepDodge);
			CharacterStateManager->SetState(ECharacterState::StepDodging);
		}

		float L_DodgeAnimDuration = PlayHighPriorityMontage(L_DodgeAnimMontage.AnimMontage, L_DodgeAnimMontage.PlayRate,
		                                                    L_DodgeAnimMontage.StartMontageTime,
		                                                    EMontagePlayReturnType::Duration, true);


		CharacterStateManager->ResetOwner(L_DodgeAnimDuration * L_DodgeAnimMontage.ResetTimeMultiplier);

		//현재 구르기 애니메이션 몽테이지가 재생중이라면
		if (IsMontagePlaying(L_DodgeAnimMontage.AnimMontage))
		{
			const float L_DodgeCost = StatsManagerComponent->GetCurrentValueForStat(EStats::DodgeStatCost) * -1.0f;
			StatsManagerComponent->ModifyCurrentStatValue(EStats::Stamina, L_DodgeCost, true, true);
			return true;
		}

		//현재 구르기 애니메이션 몽테이지가 재생중이 아니라면
		return false;
	}

	//몽테이지가 유효하지 않다면
	ResetCharacterStates();
	return false;
}

void ACRPGCharacter::DodgeHeldTimer()
{
	const auto L_PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	DodgeHeldTime = L_PlayerController->GetInputKeyTimeDown(RollKey);
}

void ACRPGCharacter::ResetDodgeHeldTimer()
{
	DodgeHeldTime = 0.0f;
	UKismetSystemLibrary::K2_ClearTimer(this, TEXT("DodgeHeldTimer"));
}

EDodgeType ACRPGCharacter::OnDodgeType()
{
	return CombatComponent->GetDodgeType();
}

bool ACRPGCharacter::OnGeneralCombatTypeEqual(EGeneralCombatType CompareCharacterState)
{
	return EquipmentComponent->GetGeneralCombatType() == CompareCharacterState;
}

bool ACRPGCharacter::OnAttackTypeEqual(EAttackTypes CompareAttackTypes)
{
	return CombatComponent->GetCurrentAttackType() == CompareAttackTypes;
}

bool ACRPGCharacter::OnAttackTypeNotEqual(EAttackTypes CompareAttackTypes)
{
	return CombatComponent->GetCurrentAttackType() != CompareAttackTypes;
}

void ACRPGCharacter::EventOnStateBegin(ECharacterState InCharacterState)
{
	UpdateBlocking(InCharacterState); //블록킹상태를 업데이트 합니다

	switch (InCharacterState)
	{
	case ECharacterState::Idle:
		bIsDefenseBroken = false;
		bFinisherModeAttack = false;
		break;

	case ECharacterState::Attacking: break;
	case ECharacterState::Blocking: break;
	case ECharacterState::FocusedState: break;
	case ECharacterState::HitStunned: break;
	case ECharacterState::Stunned: break;
	case ECharacterState::Interacting:
		CharacterStateManager->DisableMovement(true);
		break;

	case ECharacterState::Dodging: break;
	case ECharacterState::StepDodging: break;
	case ECharacterState::KnockDown: break;
	case ECharacterState::Executed_Fatality: break;
	case ECharacterState::DefenseBroken: break;
	case ECharacterState::BlockStunned: break;
	case ECharacterState::GettingUp: break;
	case ECharacterState::UsingTool:
		StatsManagerComponent->SetGait(EGait::Walk);
		break;

	case ECharacterState::Aiming: break;
	case ECharacterState::Dead: break;
	case ECharacterState::Disabled:
		CharacterStateManager->DisableMovement(true);
		break;

	case ECharacterState::GeneralAction: break;
	default: break;
	}
}

void ACRPGCharacter::EventOnStateEnd(ECharacterState InCharacterState)
{
	switch (InCharacterState)
	{
	case ECharacterState::Idle: break;
	case ECharacterState::Attacking: break;
	case ECharacterState::Blocking: break;
	case ECharacterState::FocusedState: break;
	case ECharacterState::HitStunned: break;
	case ECharacterState::Stunned: break;
	case ECharacterState::Interacting:
		CharacterStateManager->DisableMovement(false);
		break;

	case ECharacterState::Dodging: break;
	case ECharacterState::StepDodging: break;
	case ECharacterState::KnockDown: break;
	case ECharacterState::Executed_Fatality: break;
	case ECharacterState::DefenseBroken: break;
	case ECharacterState::BlockStunned: break;
	case ECharacterState::GettingUp: break;
	case ECharacterState::UsingTool:
		if (IsGaitEqual(EGait::Walk))
			StatsManagerComponent->SetGait(EGait::Run);
		break;

	case ECharacterState::Aiming: break;
	case ECharacterState::Dead: break;

	case ECharacterState::Disabled:
		CharacterStateManager->DisableMovement(false);
		break;

	case ECharacterState::GeneralAction: break;
	default: break;
	}
}

void ACRPGCharacter::EventOnMovementCancelSet(bool InCanMovementCancel)
{
	if (InCanMovementCancel)
		UKismetSystemLibrary::K2_SetTimer(this,TEXT("AttemptMovementCancel"), 0.05f, true);
	else
		UKismetSystemLibrary::K2_ClearTimer(this,TEXT("AttemptMovementCancel"));
}

void ACRPGCharacter::EventOnActionBegin(ECharacterAction InCharacterAction)
{
	switch (InCharacterAction)
	{
	case ECharacterAction::None: break;
	case ECharacterAction::Attack: break;
	case ECharacterAction::RollDodge: break;
	case ECharacterAction::StepDodge: break;
	case ECharacterAction::Interacting: break;
	case ECharacterAction::Parry: break;
	case ECharacterAction::Parried: break;
	case ECharacterAction::UseTool: break;
	case ECharacterAction::HitReaction: break;
	case ECharacterAction::ToggleFocusState: break;
	case ECharacterAction::EnterCombat: break;
	case ECharacterAction::ExitCombat: break;
	case ECharacterAction::EnterFinisherMode: break;
	case ECharacterAction::ExitFinisherMode: break;
	case ECharacterAction::SitDown_Bonfire: break;
	case ECharacterAction::StandUp_Bonfire: break;
	case ECharacterAction::WeaponArtCharge: break;
	case ECharacterAction::StartZooming: break;
	case ECharacterAction::StopZooming: break;
	case ECharacterAction::ReloadBow: break;
	case ECharacterAction::Aim: break;
	case ECharacterAction::RangedAttack: break;
	case ECharacterAction::GetupFacingUp: break;
	case ECharacterAction::GetupFacingDown: break;
	case ECharacterAction::Counter: break;
	case ECharacterAction::GeneralAction: break;
	case ECharacterAction::Equip: break;
	case ECharacterAction::UnEquip: break;
	case ECharacterAction::SwitchShield: break;
	case ECharacterAction::SwitchArrow: break;
	case ECharacterAction::SwitchTool: break;
	default: break ;
	}
}

void ACRPGCharacter::EventOnActionEnd(ECharacterAction InCharacterAction)
{
	switch (InCharacterAction)
	{
	case ECharacterAction::None: break;
	case ECharacterAction::Attack: break;
	case ECharacterAction::RollDodge: break;
	case ECharacterAction::StepDodge: break;
	case ECharacterAction::Interacting: break;
	case ECharacterAction::Parry: break;
	case ECharacterAction::Parried: break;
	case ECharacterAction::UseTool: break;
	case ECharacterAction::HitReaction: break;
	case ECharacterAction::ToggleFocusState: break;
	case ECharacterAction::EnterCombat: break;
	case ECharacterAction::ExitCombat: break;
	case ECharacterAction::EnterFinisherMode: break;
	case ECharacterAction::ExitFinisherMode: break;
	case ECharacterAction::SitDown_Bonfire: break;
	case ECharacterAction::StandUp_Bonfire: break;
	case ECharacterAction::WeaponArtCharge: break;
	case ECharacterAction::StartZooming: break;
	case ECharacterAction::StopZooming: break;
	case ECharacterAction::ReloadBow: break;
	case ECharacterAction::Aim: break;
	case ECharacterAction::RangedAttack: break;
	case ECharacterAction::GetupFacingUp: break;
	case ECharacterAction::GetupFacingDown: break;
	case ECharacterAction::Counter: break;
	case ECharacterAction::GeneralAction: break;
	case ECharacterAction::Equip: break;
	case ECharacterAction::UnEquip: break;
	case ECharacterAction::SwitchShield: break;
	case ECharacterAction::SwitchArrow: break;
	case ECharacterAction::SwitchTool: break;
	default: break ;
	}
}

void ACRPGCharacter::EventOnOwnerReset()
{
	ResetCharacterStates();
	ResetDodgeHeldTimer();
	if (CharacterStateManager->GetCancelFrames())
		AttemptToEnterStates();
}

void ACRPGCharacter::EventOnBlockStateSet(bool InIsBlocking)
{
	if (InIsBlocking)
	{
		StatsManagerComponent->SetGait(EGait::Blocking);
		StatsManagerComponent->ModifyRegenPercentForStat(EStats::Stamina, 10);
	}
	else
	{
		StatsManagerComponent->ModifyRegenPercentForStat(EStats::Stamina, 100);
		BlockHeldTime = 0.0f;
		if (IsGaitEqual(EGait::Blocking))
			StatsManagerComponent->SetGait(EGait::Run);
	}
}

void ACRPGCharacter::GetUpRoll()
{
	//구르기 회피가 가능하다면
	if (CanRollDodge())
	{
		EDodgeDirection L_DodgeDirection = CombatComponent->SetDodgeDirection(IsOrientRotationToMovement());
		PerformDodge(EDodgeType::RollDodge, L_DodgeDirection, 0);
	}
}

void ACRPGCharacter::RollDodge()
{
	if (EquipmentComponent->IsCombatEnabled())
		ResetMeleeAttack();

	UnCrouch(); //웅크리기를 해재합니다

	//이전 이동 입력값이 있다면
	if (HasMovementInput())
	{
		GetUpRoll();
	}
	else //이전 이동 입력값이 없다면
	{
		//스탭백이 가능하다면
		if (CanStepBack())
			PerformDodge(EDodgeType::RollDodge, EDodgeDirection::StepBack);
	}
}

void ACRPGCharacter::StepDodge()
{
	if (EquipmentComponent->IsCombatEnabled())
		ResetMeleeAttack();

	UnCrouch();

	if (HasMovementInput())
	{
		if (CanStepDodge())
		{
			auto L_DodgeDirection = CombatComponent->SetDodgeDirection(IsOrientRotationToMovement());
			PerformDodge(EDodgeType::StepDodge, L_DodgeDirection);
		}
	}
	else
	{
		if (CanStepBack())
			PerformDodge(EDodgeType::StepDodge, EDodgeDirection::StepBack, 0);
	}
}

bool ACRPGCharacter::IsGaitEqual(EGait CompareGait)
{
	return StatsManagerComponent->GetGait() == CompareGait;
}

bool ACRPGCharacter::IsGaitNotEqual(EGait CompareGait)
{
	return StatsManagerComponent->GetGait() != CompareGait;
}

bool ACRPGCharacter::CanSprint()
{
	return GetVelocity().Size() != 0.0f && //현재 속력이 0.0이 아니며 (그리고)
		HasEnoughOfStat(EStats::Stamina, 5.0f) && //현재 스테미나가 최소5보다 크너가 같고(그리고)
		IsCharacterStateNotEqual(ECharacterState::Aiming); //현재 조준상태가 아니라면
}

bool ACRPGCharacter::CanRollDodge()
{
	//스테미나가 충분합니다
	bool L_EnoughStamina = HasEnoughOfStat(EStats::Stamina, 5.0f);

	//해당하는 상태라면 구르기가 가능합니다
	TArray<ECharacterState> L_CanStates;
	L_CanStates.Add(ECharacterState::Idle);
	L_CanStates.Add(ECharacterState::Aiming);
	L_CanStates.Add(ECharacterState::Blocking);
	L_CanStates.Add(ECharacterState::FocusedState);
	bool L_CanState = L_CanStates.Contains(CharacterStateManager->GetCurrentState());

	//현재 공격유형이 대쉬공격이 아니라면
	bool L_NotDashAttack = IsAttackTypeNotEqual(EAttackTypes::DashAttack);

	bool L_Condition1 = L_EnoughStamina && L_CanState && L_NotDashAttack;

	//현재 상태가 회피가 아니고 (그리고) 프레임을 캔슬할 수 있다면
	bool L_Condition2 = CharacterStateManager->GetCancelFrames() &&
		CharacterStateManager->IsStateNotEqual(ECharacterState::Dodging);

	bool L_ConditionResult = L_Condition1 || L_Condition2;

	//조건이 유효하고 추락중이 아니라면
	return L_ConditionResult && !IsFalling();
}

bool ACRPGCharacter::CanStepDodge()
{
	//스테미나가 충분합니다
	bool L_EnoughStamina = HasEnoughOfStat(EStats::Stamina, 5.0f);

	//해당하는 상태라면 구르기가 가능합니다
	TArray<ECharacterState> L_CanStates;
	L_CanStates.Add(ECharacterState::Idle);
	L_CanStates.Add(ECharacterState::Aiming);
	L_CanStates.Add(ECharacterState::Blocking);
	L_CanStates.Add(ECharacterState::FocusedState);
	bool L_CanState = L_CanStates.Contains(CharacterStateManager->GetCurrentState());

	//현재 공격유형이 대쉬공격이 아니라면
	bool L_NotDashAttack = IsAttackTypeNotEqual(EAttackTypes::DashAttack);

	//회피상태가 아니라면
	bool L_NotDodging = CharacterStateManager->IsStateNotEqual(ECharacterState::Dodging);

	bool L_Condition1 = L_EnoughStamina && L_CanState && L_NotDashAttack && L_NotDodging;

	//현재 상태가 스텝회피가 아니고 (그리고) 프레임을 캔슬할 수 있다면
	bool L_Condition2 = CharacterStateManager->GetCancelFrames() &&
		CharacterStateManager->IsStateNotEqual(ECharacterState::StepDodging);


	bool L_ConditionResult = L_Condition1 || L_Condition2;

	//조건이 유효하고 추락중이 아니라면
	return L_ConditionResult && !IsFalling();
}

bool ACRPGCharacter::CanStepBack()
{
	//스테미나가 충분합니다
	bool L_EnoughStamina = HasEnoughOfStat(EStats::Stamina, 5.0f);

	//해당하는 상태라면 구르기가 가능합니다
	TArray<ECharacterState> L_CanStates;
	L_CanStates.Add(ECharacterState::Idle);
	L_CanStates.Add(ECharacterState::Aiming);
	L_CanStates.Add(ECharacterState::Blocking);
	L_CanStates.Add(ECharacterState::FocusedState);
	bool L_CanState = L_CanStates.Contains(CharacterStateManager->GetCurrentState());

	//현재 공격유형이 대쉬공격이 아니라면
	bool L_NotDashAttack = IsAttackTypeNotEqual(EAttackTypes::DashAttack);

	bool L_Condition1 = L_EnoughStamina && L_CanState && L_NotDashAttack;

	//현재 상태가 회피가 아니고 (그리고) 프레임을 캔슬할 수 있다면
	bool L_Condition2 = CharacterStateManager->GetCancelFrames() &&
		CharacterStateManager->IsStateNotEqual(ECharacterState::StepDodging) &&
		CharacterStateManager->IsStateNotEqual(ECharacterState::Dodging);

	bool L_ConditionResult = L_Condition1 || L_Condition2;

	//조건이 유효하고 추락중이 아니라면
	return L_ConditionResult && !IsFalling();
}

bool ACRPGCharacter::IsFalling()
{
	return GetCharacterMovement()->IsFalling();
}

bool ACRPGCharacter::IsOrientRotationToMovement()
{
	return GetCharacterMovement()->bOrientRotationToMovement;
}

bool ACRPGCharacter::CanCancelIntoMovementInput()
{
	TArray<ECharacterState> L_CanStateMap;
	L_CanStateMap.Add(ECharacterState::Idle);
	L_CanStateMap.Add(ECharacterState::FocusedState);
	L_CanStateMap.Add(ECharacterState::Blocking);

	//가능한 상태라면
	bool L_CanState = L_CanStateMap.Contains(GetCurrentCharacterState());

	//버퍼가 닫힌상태라면
	bool L_IsBufferClose = !CombatComponent->IsBufferOpen();

	//저장된 버퍼가 아무것도 아니거나 (또는) 방어 (또는) 최소스테미나가 5이상이 아니라면
	bool L_CanBuffer = CombatComponent->GetStoredBufferKey() == EBufferKey::None ||
		CombatComponent->GetStoredBufferKey() == EBufferKey::Block ||
		!HasEnoughOfStat(EStats::Stamina, 5.0f);

	//이동취소를 할수 있는상태라면
	bool L_CanMovementCancel = CharacterStateManager->CanMovementCancel();

	//마지막 이동입력값이 0.1f 보다 크다면
	bool L_MovementInput = GetLastMovementInputVector().Size() > 0.1f;

	return L_CanState && L_IsBufferClose && L_CanBuffer && L_CanMovementCancel && L_MovementInput && IsAnyMontagePlaying();
}

bool ACRPGCharacter::CanAttack()
{
	const bool L_EnoughCost = HasEnoughOfStat(EStats::Stamina, 5.0f);
	const bool L_NotPaused = !UGameplayStatics::IsGamePaused(this);

	//액션이 가능한 상태라면
	TArray<ECharacterState> L_CanStateMap;
	L_CanStateMap.Add(ECharacterState::Idle);
	L_CanStateMap.Add(ECharacterState::FocusedState);
	L_CanStateMap.Add(ECharacterState::Blocking);
	const bool L_CanState = L_CanStateMap.Contains(GetCurrentCharacterState());


	//액션이 가능하지 않은 상태라면
	TArray<ECharacterState> L_NotCanStateMap;
	L_NotCanStateMap.Add(ECharacterState::Attacking);
	L_NotCanStateMap.Add(ECharacterState::StepDodging);

	const bool L_CanNotState = CharacterStateManager->GetCancelFrames() && !L_NotCanStateMap.Contains(GetCurrentCharacterState());
	const bool L_CanAction = L_CanState || L_CanNotState;

	return L_EnoughCost && L_NotPaused && L_CanAction;
}

bool ACRPGCharacter::CanPerformParry()
{
	//선택적능력 패리가 가능하고 (그리고) 낙하상태가 아니고 (그리고) 현재 상태가 Idle (그리고)
	//스태미나가 5.0이상 있고 (그리고) 게임이 일시정지가 아니라면 (그리고) 피니셔모드가 아니라면
	return CanPerformOptionalAbility(EOptionalCombatAbilities::Parry) && !IsFalling() &&
		IsCharacterStateEqual(ECharacterState::Idle) && HasEnoughOfStat(EStats::Stamina, 5.0f) &&
		!CombatComponent->IsFinisherMode() && !UGameplayStatics::IsGamePaused(this);
}

bool ACRPGCharacter::IsWeaponArtAttack()
{
	//집중상태에 진입할수 없고 (그리고) 특별한행동키를 눌렀고 (그리고) 마나가 최소5보다 크고 (그리고) 패리를 수행할 수 없다면
	return !CanEnterFocusState() && bIsSpecialActionPressed && !CanPerformParry() == false && HasEnoughOfStat(EStats::Mana, 5.0f);
}

bool ACRPGCharacter::IsDashAttack()
{
	const float L_MovementSpeed = StatsManagerComponent->GetBaseValueForStat(EStats::MovementSpeed) + 100.0f;
	return GetCharacterMovement()->MaxWalkSpeed >= L_MovementSpeed && GetVelocity().Size() >= L_MovementSpeed && bIsSprinting;
}

bool ACRPGCharacter::IsChargeAttack()
{
	return AttackHeldTime >= ChargeAttackHoldTime && CombatComponent->GetStoredAttackBufferKey() == EAttackTypes::LightChargeAttack;
}

int32 ACRPGCharacter::SelectChargeAttackIndex()
{
	ChargeAttackCount = AttackCount;
	return ChargeAttackCount;
}

bool ACRPGCharacter::CanEnterCombatMode()
{
	TArray<ECharacterState> L_CanStateMap;
	L_CanStateMap.Add(ECharacterState::Idle);
	L_CanStateMap.Add(ECharacterState::Blocking);
	L_CanStateMap.Add(ECharacterState::Aiming);
	return L_CanStateMap.Contains(GetCurrentCharacterState());
}

void ACRPGCharacter::EnableCombat()
{
	if (CanEnterCombatMode())
		if (!PerformAction(ECharacterAction::EnterCombat, ECharacterState::GeneralAction, 0.0f, 0, false, true))
			EquipmentComponent->ToggleCombat();
}

void ACRPGCharacter::CancelAiming()
{
	//나중에 구현합니다
}

void ACRPGCharacter::DisableCombat()
{
	if (CanEnterCombatMode())
	{
		if (IsCharacterStateEqual(ECharacterState::Aiming))
			CancelAiming();
		else
		{
			if (!PerformAction(ECharacterAction::ExitCombat, ECharacterState::GeneralAction, 0.0f, 0, false, true))
				EquipmentComponent->ToggleCombat();
		}
	}
}

void ACRPGCharacter::ToggleCombat()
{
	ResetMeleeAttack();

	if (!EquipmentComponent->IsCombatEnabled())
		EnableCombat();
	else
		DisableCombat();
}

void ACRPGCharacter::ResetAttackHandle()
{
	ResetMeleeAttack();
	UKismetSystemLibrary::K2_ClearTimerHandle(this, ResetComboHandle);
}

void ACRPGCharacter::OnAttackTypeBegin(EAttackTypes InAttackTypes)
{
	switch (InAttackTypes)
	{
	case EAttackTypes::None: break;
	case EAttackTypes::LightAttack: break;
	case EAttackTypes::StrongAttack: break;
	case EAttackTypes::DashAttack: break;
	case EAttackTypes::LightChargeAttack: break;
	case EAttackTypes::WeaponArtAttack: break;
	case EAttackTypes::AirAttack: break;
	case EAttackTypes::FatalityExecution: break;
	case EAttackTypes::Assassination: break;
	case EAttackTypes::EnterFinisherModeAttack: break;
	case EAttackTypes::ExitFinisherModeAttack: break;
	case EAttackTypes::ShootArcheryProjectile: break;
	case EAttackTypes::ChainAttack: break;
	case EAttackTypes::Riposte: break;
	case EAttackTypes::StepDodgeAttack: break;
	case EAttackTypes::RollDodgeAttack: break;
	case EAttackTypes::MAX: break;
	default: ;
	}
}

bool ACRPGCharacter::PerformAttack(EAttackTypes InAttackType, int32 InAttackIndex, bool IsRandomIndex)
{
	if (EquipmentComponent->IsCombatEnabled())
	{
		UnCrouch();

		CombatComponent->SetCharacterAttackType(InAttackType);
		CharacterStateManager->SetAction(ECharacterAction::Attack);
		CharacterStateManager->SetState(ECharacterState::Attacking);
		CombatComponent->StoreBasicAttackInfo(EquipmentComponent->GetGeneralCombatType(), EquipmentComponent->GetCurrentCombatType(), InAttackIndex);

		UKismetSystemLibrary::K2_ClearTimer(this,TEXT("ResetAttackHandle"));

		FAnimMontageStruct L_AttackMontage = MontageManagerComponent->GetAttackMontage(InAttackType, InAttackIndex, IsRandomIndex);

		//어택 애니메이션 몽테이지가 유효하다면
		if (IsValid(L_AttackMontage.AnimMontage))
		{
			// InformAIOfAnAttack() 나중에 로직을 추가합니다

			float L_AttackMontageDuration = PlayHighPriorityMontage(L_AttackMontage.AnimMontage, L_AttackMontage.PlayRate,
			                                                        L_AttackMontage.StartMontageTime, EMontagePlayReturnType::Duration, true);
			CharacterStateManager->ResetOwner(L_AttackMontageDuration * L_AttackMontage.ResetTimeMultiplier);

			AttackCount++; //어태카운트를 증가합니다

			//어택카운트가 몽테이지의 마지막인덱스보다 크다면
			if (AttackCount > MontageManagerComponent->GetAttackMontageLastIndex())
			{
				AttackCount = 0;
				bCheckFocusState = false;
			}

			//콤보핸들을 설정합니다
			ResetComboHandle = UKismetSystemLibrary::K2_SetTimer(this,TEXT("ResetAttackHandle"), L_AttackMontageDuration * 0.8f, false);

			//공격 몽테이지를 실행하고 잇다면
			if (IsMontagePlaying(L_AttackMontage.AnimMontage))
			{
				OnAttackTypeBegin(InAttackType);

				//공격유형이 원거리가공격이 아니라면
				if (IsActionNotEqual(ECharacterAction::RangedAttack))
				{
					//새로운 공격유형이 웨폰공격 기술이라면
					if (InAttackType == EAttackTypes::WeaponArtAttack)
					{
						float L_WeaponArtCost = StatsManagerComponent->GetCurrentValueForStat(EStats::ManaCost) * -1.0f;
						StatsManagerComponent->ModifyCurrentStatValue(EStats::Mana, L_WeaponArtCost, true, true);
					}


					float L_AttackCost = StatsManagerComponent->AttackStatCost(InAttackType) * -1.0f;
					StatsManagerComponent->ModifyCurrentStatValue(EStats::Stamina, L_AttackCost, true, true);

					return true;
				}

				//공격유형이 원거리라면
				return true;
			}

			//공격몽테이지를 실행하고 있지 않다면
			return false;
		}

		//어택 애니메이션 몽테이지가 유효하지 않다면
		ResetCharacterStates();
		return false;
	}

	//전투상태가 아니라면
	ToggleCombat();
	return false;
}

bool ACRPGCharacter::PerformAction(ECharacterAction InCombatAction, ECharacterState InActionCombatState, float InMontageStartPosition,
                                   int32 InMontageIndex, bool IsRandomIndex, bool IsAutoResset)
{
	CharacterStateManager->SetAction(InCombatAction); //액션 상태를 설정
	CharacterStateManager->SetState(InActionCombatState); //캐릭터 상태를 설정

	//몽테이지 컴포넌트에서 액션애니메이션을 가져옵니다
	const FAnimMontageStruct L_ActionMontage = MontageManagerComponent->GetGeneralActionMontage(InCombatAction, InMontageIndex, IsRandomIndex);

	//액션몽테이지가 유효하다면
	if (IsValid(L_ActionMontage.AnimMontage))
	{
		//애니메이션 시작 시간을 설정합니다
		float L_AtStartPosition = UKismetMathLibrary::SelectFloat(InMontageStartPosition, L_ActionMontage.StartMontageTime, InMontageStartPosition > 0.0f);

		//몽테이지를 실행합니다 그 후 몽테이지의 지속시간을 가져옵니다
		const float L_MontageDuration = PlayHighPriorityMontage(L_ActionMontage.AnimMontage, L_ActionMontage.PlayRate, L_AtStartPosition,
		                                                        EMontagePlayReturnType::Duration, true);
		//자동 리셋이라면
		if (IsAutoResset)
			CharacterStateManager->ResetOwner(L_MontageDuration * L_ActionMontage.ResetTimeMultiplier);

		//액션 몽테이지가 실행중이라면
		if (IsMontagePlaying(L_ActionMontage.AnimMontage))
		{
			const ECharacterAction L_CurrentAction = CharacterStateManager->GetCurrentAction();
			float L_ActionCost = StatsManagerComponent->GetCombatActionStatCost(L_CurrentAction) * -1.0f;
			StatsManagerComponent->ModifyCurrentStatValue(EStats::Stamina, L_ActionCost);
			return true;
		}

		//액션 몽테이지가 실행중이아니라면
		return false;
	}

	//액션몽테이지가 유효하지 않다면
	ResetCharacterStates();
	return false;
}

void ACRPGCharacter::AttackEvent()
{
	//공격이 가능하지 않다면 종료합니다
	if (CanAttack()) return;

	//공중에 있거나 추락중이라면 공중공격을 실행합니다
	if (IsFalling())
		PerformAttack(EAttackTypes::AirAttack, AttackCount, false);

	if (IsWeaponArtAttack())
		PerformAttack(EAttackTypes::WeaponArtAttack, AttackCount, false);

	if (IsDashAttack())
		PerformAttack(EAttackTypes::DashAttack, AttackCount, false);

	if (IsChargeAttack())
	{
		ResetChargeAttack();
		PerformAttack(CombatComponent->GetStoredAttackBufferKey(), SelectChargeAttackIndex(), false);
	}

	PerformAttack(CombatComponent->GetStoredAttackBufferKey(), AttackCount, false);
}

void ACRPGCharacter::EventOnInputBufferConsumed(EBufferKey InputBufferKey)
{
	if (!IsDead())
	{
		switch (InputBufferKey)
		{
		case EBufferKey::None: break;
		case EBufferKey::Attack:
			AttackEvent();
			break;
		case EBufferKey::StepDodge:
			StepDodge();
			break;

		case EBufferKey::RollDodge:
			RollDodge();
			break;

		case EBufferKey::Block: break;
		case EBufferKey::Jump:
			if (CanJump())
			{
				StopAnimMontage();
				Jump();
			}
			break;
		case EBufferKey::Parry: break;
		case EBufferKey::UseTool: break;
		case EBufferKey::Interact: break;
		case EBufferKey::Crouch: break;
		case EBufferKey::ToggleFinisherMode: break;
		case EBufferKey::ChargeAttack: break;
		case EBufferKey::ToggleCombatMode:
			ToggleCombat();
			break;
		case EBufferKey::GetUpRoll: break;
		case EBufferKey::SwitchActiveItem: break;
		case EBufferKey::Zoom: break;
		case EBufferKey::Aim: break;
		default: break;
		}
	}
}

void ACRPGCharacter::EventOnInputBufferOpen()
{
}

void ACRPGCharacter::EventOnInputBufferClose()
{
	if (!IsDead())
		AttemptToEnterStates();
}

bool ACRPGCharacter::AttemptToEnterStates()
{
	if (EquipmentComponent->GetGeneralCombatType() == EGeneralCombatType::MeleeCombat)
	{
		//블록키가 존재하고 블록키의 값이 활성화라면
		if (CombatComponent->GetHeldBufferKeyValue(EBufferKey::Block))
		{
			CombatComponent->StoreBufferKey(EBufferKey::Block);
			return true;
		}
		else if (CombatComponent->GetHeldBufferKeyValue(EBufferKey::ToggleFocusMode)) //집중 모드 토글이 활성화라면
		{
			CombatComponent->StoreBufferKey(EBufferKey::ToggleFocusMode);
			return true;
		}
	}
	else if (EquipmentComponent->GetGeneralCombatType() == EGeneralCombatType::RangedCombat)
	{
		if (CombatComponent->GetHeldBufferKeyValue(EBufferKey::Aim))
		{
			CombatComponent->StoreBufferKey(EBufferKey::Aim);
			return true;
		}
		else if (CombatComponent->GetHeldBufferKeyValue(EBufferKey::Zoom))
		{
			CombatComponent->StoreBufferKey(EBufferKey::Zoom);
			return true;
		}
	}
	else if (EquipmentComponent->GetGeneralCombatType() == EGeneralCombatType::MagicCombat)
	{
	}

	return false;
}

void ACRPGCharacter::BlockReleased()
{
	//현재 블록 상태라면
	if (IsCharacterStateEqual(ECharacterState::Blocking))
	{
		CharacterStateManager->SetAction(ECharacterAction::None);
		CharacterStateManager->SetState(ECharacterState::Idle);

		//StopBlocking함수가 실행중이아니라면 0.15초뒤에 실행합니다
		if (!UKismetSystemLibrary::K2_IsTimerActive(this,TEXT("StopBlocking")))
			UKismetSystemLibrary::K2_SetTimer(this,TEXT("StopBlocking"), 0.15f, false);
	}
}

void ACRPGCharacter::StopBlocking()
{
	CharacterStateManager->SetBlockingState(false);
}

void ACRPGCharacter::ResetMeleeAttack()
{
	AttackCount = 0;
	ChargeAttackCount = 0;
	StrongChargeAttackAmount = 0.0f;
	MontageManagerComponent->ResetAttackMontageIndex();
	CombatComponent->SetCharacterAttackType(EAttackTypes::None);
}

void ACRPGCharacter::UpdateBlocking(ECharacterState InCharacterState)
{
	//블록킹 상태가 아니거나 (그리고) 블록기절이 아니고 (그리고) Idle아니라면 
	if (InCharacterState != ECharacterState::Blocking &&
		InCharacterState != ECharacterState::BlockStunned &&
		InCharacterState != ECharacterState::Idle)
	{
		CharacterStateManager->SetBlockingState(false);
	}
}

bool ACRPGCharacter::IsAttackTypeEqual(EAttackTypes CompareAttackType)
{
	return CombatComponent->GetCurrentAttackType() == CompareAttackType;
}

bool ACRPGCharacter::IsAttackTypeNotEqual(EAttackTypes CompareAttackType)
{
	return CombatComponent->GetCurrentAttackType() != CompareAttackType;
}

UAnimInstance* ACRPGCharacter::GetAnimInstance()
{
	return GetMesh()->GetAnimInstance();
}

bool ACRPGCharacter::IsDead()
{
	return CharacterStateManager->IsDead();
}

bool ACRPGCharacter::HasMovementInput()
{
	return UKismetMathLibrary::NotEqual_VectorVector(GetCharacterMovement()->GetLastInputVector(), FVector::ZeroVector);
}

void ACRPGCharacter::DetectDoubleClick(TFunctionRef<void()> DoubleClick, TFunctionRef<void()> SingleClick)
{
	static bool IsDoubleClick = false;
	static FGate ClickGate = FGate(true);

	if (ClickGate.IsOpen())
		IsDoubleClick = true;

	ClickGate.Open();

	FTimerHandle WaitTimerHandle;
	float WaitTime = 0.25f;

	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		ClickGate.Close();
	}), WaitTime, false); // 반복하려면 false를 true로 변경

	if (IsDoubleClick)
	{
		IsDoubleClick = false;
		DoubleClick();
	}
	else
		SingleClick();
}

bool ACRPGCharacter::I_TurnInPlace(bool InTurnPlace)
{
	return false;
}

void ACRPGCharacter::I_StopSprinting()
{
	bIsSprinting = false;

	IInterface_AnimInstance* CastAnimInterface = Cast<IInterface_AnimInstance>(GetAnimInstance());
	CHECK(CastAnimInterface!=nullptr);
	CastAnimInterface->I_IsSprinting(bIsSprinting);

	//타겟팅컴포넌트의 로직을 추가합니다

	UKismetSystemLibrary::K2_PauseTimer(StatsManagerComponent,TEXT("SprintStaminaCost"));
	StatsManagerComponent->SetCanModifyMovementSpeed(true);
	if (IsGaitEqual(EGait::Sprint))
		StatsManagerComponent->SetGait(EGait::Run);
}

void ACRPGCharacter::I_SendCharacterCameraAndSpringArm(UCameraComponent** OutCameraComponent, USpringArmComponent** OutSpringArmComponent)
{
}

void ACRPGCharacter::I_FootStep(EPhysicalSurface InSurfaceType, FVector StepLocation)
{
}

void ACRPGCharacter::I_SlowMotion(float InSlowMotionStrength, float TimeInSlowMotion)
{
}

void ACRPGCharacter::I_RagDollCharacterMesh()
{
}

void ACRPGCharacter::I_RestAtBonfire()
{
}

bool ACRPGCharacter::I_IsCharacterSprinting()
{
	return false;
}

void ACRPGCharacter::I_FallDamage(bool InFallDamage)
{
	bFallDamage = InFallDamage;
}

void ACRPGCharacter::I_ToggleCameraMovement(bool InDisableCameraMovement)
{
	bDisableCameraMovement = InDisableCameraMovement;
}

void ACRPGCharacter::I_PlayActionMontage(ECharacterAction InCombatAction, int32 MontageIndex)
{
}

void ACRPGCharacter::I_ShouldDrawBowString(bool InShouldDrawBowString)
{
	bShouldDrawBowString = InShouldDrawBowString;
}

void ACRPGCharacter::I_PerformAbility(ECharacterAction InAction, ECharacterState InState, float MontageStartPosition, int32 MontageIndex,
                                      bool RandomIndex, bool AutoReset)
{
}

void ACRPGCharacter::I_TogglePauseMenu()
{
}

void ACRPGCharacter::I_ToggleBonfireMenu()
{
}

void ACRPGCharacter::I_ToggleUserInterface(EUserInterface InUserInterface)
{
}

FKey ACRPGCharacter::I_GetScrollKey()
{
	return {};
}

void ACRPGCharacter::I_EndStrongChargeAttack()
{
}

bool ACRPGCharacter::I_CombatEnabled()
{
	return false;
}

UStaticMesh* ACRPGCharacter::I_GetArrowMesh()
{
	return nullptr;
}

void ACRPGCharacter::I_PlayImpactEffect(TSubclassOf<UCBaseImpactEffect> InImpactEffect, FVector InImpactPoint, EPhysicalSurface InSurfaceType,
                                        bool UseScreenShake)
{
}
