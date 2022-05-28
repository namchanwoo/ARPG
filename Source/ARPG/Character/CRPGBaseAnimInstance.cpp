#include "Character/CRPGBaseAnimInstance.h"

#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/Interface_GeneralCharacter.h"
#include "Interfaces/Interface_Rotation.h"
#include "Systems/CombatSystem/CCharacterStateManagerComponent.h"
#include "Systems/CombatSystem/CCombatComponent.h"
#include "Systems/CombatSystem/CEquipmentManagerComponent.h"

UCRPGBaseAnimInstance::UCRPGBaseAnimInstance()
{
	RotationInterpSpeed = 720.0f;
	AimingRotationInterpSpeed = 7.0f;
	LeanIntensityScaling = 30.0f;
	BowDrawSpeed = 1.0f;
	LerpAlpha = 0.2f;
	TurnInPlacePlayRate = 1.0f;
	LegBlendWeightMultiplier = 5.0f;
}

void UCRPGBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	CharacterRef = Cast<ACharacter>(TryGetPawnOwner());
	CHECK(CharacterRef!=nullptr);

	EquipmentComponent = Cast<UCEquipmentManagerComponent>
		(CharacterRef->GetComponentByClass(UCEquipmentManagerComponent::StaticClass()));

	// TargetingComponent = Cast<UTargetingComponent>
	// 	(CharacterRef->GetComponentByClass(UTargetingComponent::StaticClass()));

	CombatComponent = Cast<UCCombatComponent>
		(CharacterRef->GetComponentByClass(UCCombatComponent::StaticClass()));

	CharacterStateManager = Cast<UCCharacterStateManagerComponent>
		(CharacterRef->GetComponentByClass(UCCharacterStateManagerComponent::StaticClass()));

	// HitReactionComponent = Cast<UHitReactionComponent>
	// 	(CharacterRef->GetComponentByClass(UHitReactionComponent::StaticClass()));


	//컴포넌트들 델리게이트를 바인딩
	{
		CHECK(EquipmentComponent!=nullptr);
		EquipmentComponent->OnCombatTypeChanged.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnCombatTypeChanged);
		EquipmentComponent->OnGeneralCombatTypeChanged.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnGeneralCombatTypeChanged);
		EquipmentComponent->OnCombatChanged.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnCombatChanged);

		CHECK(CombatComponent!=nullptr);
		CombatComponent->OnFinisherModeChanged.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnFinisherModeChanged);

		CHECK(CharacterStateManager!=nullptr);
		CharacterStateManager->OnStateBegin.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnStateBegin);
		CharacterStateManager->OnActionBegin.AddDynamic(this, &UCRPGBaseAnimInstance::EventOnActionBegin);
	}


	FTimerHandle WaitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		bCharacterInitialized = true;
	}), 0.1f, false); // 반복하려면 false를 true로 변경
}

void UCRPGBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(CharacterRef))
	{
		//캐릭터 스테이터스를 동기화
		{
			//공중 설정
			bIsInAir = CharacterRef->GetMovementComponent()->IsFalling();
			//크런치설정
			bIsCrouching = CharacterRef->GetMovementComponent()->IsCrouching();
			//스피드설정
			Speed = TryGetPawnOwner()->GetVelocity().Size();
			//방향 값설정
			Direction = CalculateDirection(CharacterRef->GetVelocity(), CharacterRef->GetActorRotation());
			//가속 설정
			CurrentAcceleration = CharacterRef->GetCharacterMovement()->GetCurrentAcceleration().Size();
			//가속 여부 설정
			bIsAccelerating = CurrentAcceleration > 0.0f;

			//걷기 가속값 설정
			CurrentAccelerationWalk = CharacterRef->GetCharacterMovement()->MaxAcceleration * 0.65f;


			if (bIsAccelerating)
				Direction = DirectionStop;

			//애니메이션 재생 여부 설정
			bIsMontagePlaying = IsAnyMontagePlaying();
		}


		//기울기 계산
		CalculateLeanAngles(DeltaSeconds);

		//부드럽게 캐릭터 회전
		SmoothCharacterRotation();

		//Yaw And Pitch Math
		{
			const FRotator L_OwnerRotation = TryGetPawnOwner()->GetActorRotation();

			Roll = L_OwnerRotation.Roll;
			Pitch = L_OwnerRotation.Pitch;
			YawLastTick = Yaw;
			Yaw = L_OwnerRotation.Yaw;
			YawChangeOverFrame = YawLastTick - Yaw;


			const FRotator L_DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(TryGetPawnOwner()->GetBaseAimRotation(),
			                                                                           TryGetPawnOwner()->GetActorRotation());

			AimRotationPitch = UKismetMathLibrary::ClampAngle(L_DeltaRotator.Pitch, -90.0f, 90.0f);
			AimRotationYaw = L_DeltaRotator.Yaw;
		}


		//조준중일때 회전
		AimingRotation();

		//IK관련된 프로퍼티 설정
		IkHitReactions();

		//공중에 있다면
		if (bIsInAir)
			InAirCheck();
		else //공중에 있지 않다면
			CalculateRootYawOffset();
	}
}

void UCRPGBaseAnimInstance::EventOnCombatTypeChanged(ECombatType InCombatType)
{
	CombatType = InCombatType;

	bBlockTransition = false;
	bFocusTransition = (CombatType == ECombatType::LightSword);
	bAimTransition = (CombatType == ECombatType::Bow);
}

void UCRPGBaseAnimInstance::EventOnGeneralCombatTypeChanged(EGeneralCombatType InGeneralCombatType)
{
	GeneralCombatType = InGeneralCombatType;
}

void UCRPGBaseAnimInstance::EventOnCombatChanged(bool bInCombatEnabled)
{
	bIsCombatEnabled = bInCombatEnabled;
}

void UCRPGBaseAnimInstance::EventOnFinisherModeChanged(bool InPunisherMode)
{
	bIsFinisherEnabled = InPunisherMode;
}

void UCRPGBaseAnimInstance::EventOnStateBegin(ECharacterState InCharacterState)
{
	CharacterState = InCharacterState;
}

void UCRPGBaseAnimInstance::EventOnActionBegin(ECharacterAction InCharacterAction)
{
	CharacterAction = InCharacterAction;
}

void UCRPGBaseAnimInstance::CalculateLeanAngles(float InDeltaSeconds)
{
	//오너캐릭터의 회전 값
	const FRotator L_OwnerRotator = TryGetPawnOwner()->GetActorRotation();
	//마지막 회전 값과 오너캐릭터의 회전 값을 정규화하고 그 Yaw값을 설정
	const float L_Yaw = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, L_OwnerRotator).Yaw;

	const float L_ScalingYaw = L_Yaw / InDeltaSeconds / LeanIntensityScaling;

	const float L_TargetYaw = UKismetMathLibrary::SelectFloat(L_ScalingYaw, L_Yaw, Speed >= 400.0f);

	const float L_AngleDegrees = UKismetMathLibrary::FInterpTo(YawDelta, L_TargetYaw, InDeltaSeconds, 6.0f);

	//주어진각도에서 임의의 각도로 클램핑한값을 설정합니다 
	YawDelta = UKismetMathLibrary::ClampAngle(L_AngleDegrees, -8.0f, 8.0f);

	//마지막 이전 틱의 회전값을 설정
	RotationLastTick = L_OwnerRotator;

	//몽테이지가 재생중이거나 (또는) 공중에 있고 (또는) 조준상태라면
	if (bIsMontagePlaying || bIsInAir || CharacterState == ECharacterState::Aiming)
		YawDelta = 0.0f;
}

void UCRPGBaseAnimInstance::SmoothCharacterRotation()
{
	//캐릭터가 회전해야 한다면
	if (bRotateCharacter)
	{
		IInterface_Rotation* Rotation_Interface = Cast<IInterface_Rotation>(TryGetPawnOwner());
		CHECK(Rotation_Interface!=nullptr);
		const FRotator L_CharacterRotation = Rotation_Interface->I_GetCharacterRotation();


		const FRotator L_PawnRotation = TryGetPawnOwner()->GetActorRotation();
		const FRotator L_NewRotation = UKismetMathLibrary::RInterpTo_Constant(L_PawnRotation, L_CharacterRotation,
		                                                                      UGameplayStatics::GetWorldDeltaSeconds(this), RotationInterpSpeed);

		TryGetPawnOwner()->SetActorRotation(L_NewRotation);
	}
}

void UCRPGBaseAnimInstance::AimingRotation()
{
	//현재 캐릭터 상태가 조준중이고  (그리고) 몽테이지가 재생중이 아니라면
	if (CharacterState == ECharacterState::Aiming && !bIsMontagePlaying)
	{
		FRotator L_OwnerRotation = TryGetPawnOwner()->GetActorRotation();
		FRotator L_ControlRotation = TryGetPawnOwner()->GetControlRotation();
		FRotator L_TargetRotation = FRotator(L_OwnerRotation.Pitch, L_ControlRotation.Yaw, L_OwnerRotation.Roll);

		const FRotator L_NewRotation = UKismetMathLibrary::RInterpTo(L_OwnerRotation, L_TargetRotation,
		                                                             UGameplayStatics::GetWorldDeltaSeconds(this), AimingRotationInterpSpeed);
		TryGetPawnOwner()->SetActorRotation(L_NewRotation);
	}
}

void UCRPGBaseAnimInstance::IkHitReactions()
{
	//히트리액션 컴포넌트로직을 추가해야합니다
	// LerpAlpha = HitReactionComponent->GetReactionIn();
	//
	// WS_LClav = HitReactionComponent->LeftArmWorldSpace;
	// WS_RClav = HitReactionComponent->RightArmWorldSpace;
	// WS_Spine03 = HitReactionComponent->SpineWorldSpace;
	// WS_Neck = HitReactionComponent->NeckWorldSpace;
	// WS_HandR = HitReactionComponent->RightHandWorldSpace;
	// WS_HandL = HitReactionComponent->LeftHandWorldSpace;
	//
	//
	// LClavAlpha = UKismetMathLibrary::Lerp(LClavAlpha, HitReactionComponent->LeftArmAlpha, LerpAlpha);
	// RClavAlpha = UKismetMathLibrary::Lerp(RClavAlpha, HitReactionComponent->RightArmAlpha, LerpAlpha);
	// Spine_Alpha = UKismetMathLibrary::Lerp(Spine_Alpha, HitReactionComponent->SpineAlpha, LerpAlpha);
	// NeckAlpha = UKismetMathLibrary::Lerp(NeckAlpha, HitReactionComponent->NeckAlpha, LerpAlpha);
	// HandRAlpha = UKismetMathLibrary::Lerp(HandRAlpha, HitReactionComponent->RightHandAlpha, LerpAlpha);
	// HandLAlpha = UKismetMathLibrary::Lerp(HandLAlpha, HitReactionComponent->LeftHandAlpha, LerpAlpha);
	// Leg_R = HitReactionComponent->RightLegAlpha;
	// Leg_L = HitReactionComponent->LeftLegAlpha;
}

void UCRPGBaseAnimInstance::ResetAirCheck()
{
	AirCheck_DoOnce.Reset();
}

void UCRPGBaseAnimInstance::InAirCheck()
{
	if (JumpLineTrace())
	{
		//땅과의 거리가 600.0f보다 멀다면
		if (DistanceFromGround > 600.0f)
		{
			//공중확인이 가능하다면
			if (AirCheck_DoOnce.Execute())
			{
				IInterface_GeneralCharacter* GeneralCharacter_Interface = Cast<IInterface_GeneralCharacter>(TryGetPawnOwner());
				CHECK(GeneralCharacter_Interface!=nullptr);
				GeneralCharacter_Interface->I_FallDamage(true);
				bLargeLanding = true;
			}
		}
		else //땅과의 거리가 600.0f보다 작다면
		{
			ResetAirCheck();
		}
	}
}

void UCRPGBaseAnimInstance::CalculateRootYawOffset()
{
	const bool L_PlaceState =
		CharacterState == ECharacterState::Aiming ||
		CharacterState == ECharacterState::Blocking ||
		CharacterState == ECharacterState::Idle || bShouldTurnInPlace;

	//회전해야한다면 (가속중이 아니고 (그리고) 스피드가 0.0f 이하이고 (그리고) 제자리상태 중 하나라도 참이면 (그리고) 캐릭터가 초기화되었다면)
	if (!bIsAccelerating && Speed <= 0.0f && L_PlaceState && bCharacterInitialized)
	{
		const float L_YawClampAngle = UKismetMathLibrary::ClampAngle(YawChangeOverFrame + RootYawOffset, -90.0f, 90.0f);
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(L_YawClampAngle);

		const float L_CurveValue_IsTurning = GetCurveValue(FName(TEXT("IsTurning")));

		//커브 값이 1.0f와 같다면
		if (UKismetMathLibrary::NearlyEqual_FloatFloat(L_CurveValue_IsTurning, 1.0f, 0.001f))
		{
			bIsTurning = true;
			if (RootTurning_DoOnce.Execute())
				DistanceCurveValue = GetCurveValue(FName(TEXT("DistanceCurve")));

			DistanceCurveValueLastFrame = DistanceCurveValue;
			DistanceCurveValue = GetCurveValue(FName(TEXT("DistanceCurve")));

			float L_DistanceCurveChangeOverFrame = DistanceCurveValueLastFrame - DistanceCurveValue;
			//방향 배율을 설정
			const float L_DirectionMagnification = UKismetMathLibrary::SelectFloat(-1.0f, 1.0f, RootYawOffset > 0.0f);
			// 커브값 * 방향배율 값
			const float L_DirectionTick = L_DistanceCurveChangeOverFrame * L_DirectionMagnification;

			RootYawOffset = UKismetMathLibrary::ClampAngle(RootYawOffset - L_DirectionTick, -90.0f, 90.0f);

			//회전 속도 설정
			TurnInPlacePlayRate = UKismetMathLibrary::MapRangeClamped(RootYawOffset, 0.0f, 90.0f, 1.0f, 2.0f);
		}
		else //커브값이 1.0f 과 같지 않다면
		{
			bIsTurning = false;
			RootTurning_DoOnce.Reset();
		}
	}
	else //회전하지 않는다면
	{
		if (RootYawOffset != 0.0f)
			RootYawOffset = UKismetMathLibrary::FInterpTo(RootYawOffset, 0.0f, UGameplayStatics::GetWorldDeltaSeconds(this), 10.0f);
	}
}

float UCRPGBaseAnimInstance::CheckFallDistance(const FVector_NetQuantize& InLocation)
{
	return (TryGetPawnOwner()->GetActorLocation() - InLocation).Size();
}

bool UCRPGBaseAnimInstance::JumpLineTrace()
{
	const FVector L_CharacterVelocity = CharacterRef->GetVelocity();

	//낙하 속도를 설정
	FallingSpeed = L_CharacterVelocity.Z;

	//낙하 속도가 -200.0f보다 작다면
	if (FallingSpeed < -200.0f)
	{
		//트레이스 스타트  = 캡슐의 월드 위치를 설정
		const FVector L_StartTrace = CharacterRef->GetCapsuleComponent()->GetComponentTransform().GetLocation();

		const float L_ClampZ = UKismetMathLibrary::FClamp(L_CharacterVelocity.Z, -4000.0f, -200.0f);
		const FVector L_NormalVector = UKismetMathLibrary::Vector_NormalUnsafe(FVector(L_CharacterVelocity.X, L_CharacterVelocity.Y, L_ClampZ));
		const float L_RangeFloat = UKismetMathLibrary::MapRangeClamped(L_CharacterVelocity.Z, 0.0f, -4000.0f, 50.0f, 2000.0f);
		const FVector L_EndTrace = L_StartTrace + (L_NormalVector * L_RangeFloat);

		const TArray<AActor*> L_ActorsToIgnore;
		FHitResult L_HitResult;

		UKismetSystemLibrary::LineTraceSingle(this, L_StartTrace, L_EndTrace, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		                                      false, L_ActorsToIgnore, EDrawDebugTrace::None, L_HitResult, true);

		DistanceFromGround = CheckFallDistance(L_HitResult.Location);

		//히트한 것이 있고 (그리고) 걸을수 있는 바닥위에 있으면
		return L_HitResult.bBlockingHit && CharacterRef->GetCharacterMovement()->IsWalkable(L_HitResult);
	}
	else //낙하 스피드가 -2000f보다 크다면
	{
		return false;
	}
}

void UCRPGBaseAnimInstance::I_SetBowDrawSpeed(float InBowDrawSpeed)
{
	BowDrawSpeed = InBowDrawSpeed;
}

void UCRPGBaseAnimInstance::I_IsTargetLocked(bool IsTargetLocked)
{
	bTargetLocked = IsTargetLocked;
}

void UCRPGBaseAnimInstance::I_IsSprinting(bool IsSprinting)
{
	bIsSprinting = IsSprinting;
}

void UCRPGBaseAnimInstance::I_SetLegIKBlendWeightMultiplier(float InLegBlendWeightMultiplier)
{
	LegBlendWeightMultiplier = InLegBlendWeightMultiplier;
}

void UCRPGBaseAnimInstance::I_SetPlayerHandStringSocketName(FName HandStringSocketName)
{
}

void UCRPGBaseAnimInstance::I_SetShouldTurnInPlace(bool InShouldTurnInPlace)
{
	bShouldTurnInPlace = InShouldTurnInPlace;
}

FRotator UCRPGBaseAnimInstance::I_GetCharacterRotation()
{
	return {};
}

void UCRPGBaseAnimInstance::I_StartRotating(float InRotationInterpSpeed)
{
	RotationInterpSpeed = InRotationInterpSpeed;
	bRotateCharacter = true;
}

void UCRPGBaseAnimInstance::I_StopRotating()
{
	bRotateCharacter = false;
}
