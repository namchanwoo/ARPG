#include "CBaseProjectile.h"

#include "Global.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/Interface_Combat.h"
#include "Particles/ParticleSystemComponent.h"
#include "Systems/CombatSystem/CustomCollisionComponent.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"


ACBaseProjectile::ACBaseProjectile()
{
	//초기 컴포넌트 생성 및 초기화 설정
	{
		WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
		WeaponRoot->SetupAttachment(RootComponent);

		ParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleTrailComponent"));
		ParticleTrail->PrimaryComponentTick.bStartWithTickEnabled = false;
		ParticleTrail->bAutoActivate = false;
		ParticleTrail->SetupAttachment(WeaponRoot);

		ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
		ArrowComponent->SetupAttachment(RootComponent);

		ProjectileCollisionComponent = CreateDefaultSubobject<UCustomCollisionComponent>(TEXT("ProjectileCollisionComponent"));
		ProjectileCollisionComponent->SetTraceRadius(0.5f);
		ProjectileCollisionComponent->SetCollisionProfileNamesToIgnore(TEXT("Pawn"));
	}

	//멤버 값 설정
	SpawnArrowState = EWeaponProjectileState::None;

	//Projectile Parameters
	{
		ProjectileInitialSpeed = 8000.0f;
		bRotationFollowsVelocity = true;
	}

	HitBoneName = TEXT("None");
	DeathPhysicsVelocity = 1800.f;
	HeadShotMultiplier = 2.5f;
}

void ACBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileCollisionComponent->Event_InitializeCollision(GetOwner());

	SetWeaponProjectileState(SpawnArrowState); //발사체상태를 설정합니다

	DefaultDeathPhysicsVelocity = DeathPhysicsVelocity; //물리속도를 설정합니다


	ProjectileCollisionComponent->OnHit.AddDynamic(this, &ACBaseProjectile::OnHit_ProjectileComponent);
	ProjectileCollisionComponent->OnCollisionEnabled.AddDynamic(this, &ACBaseProjectile::OnCollisionEnabled_ProjectileComponent);

	//함수들을 위한 기본준비
	ProjectileLogicLatentInfo.CallbackTarget = this;
	ProjectileLogicLatentInfo.ExecutionFunction = TEXT("ShotProjectileAfter");
	ProjectileLogicLatentInfo.Linkage = 1;
	ProjectileLogicLatentInfo.UUID = LatentID++;
}

void ACBaseProjectile::InitializeSpawnData(EWeaponProjectileState InSpawnArrowState, float InRegularDamage, float InPoiseDamage,
                                           UCAttackInfo* InAttackInfo, UCRPGItemAsset* InDataAsset)
{
	ProjectileState = InSpawnArrowState;
	RegularDamage = InRegularDamage;
	PoiseDamage = InPoiseDamage;
	AttackInfo = InAttackInfo;
	DataAsset = InDataAsset;
}

void ACBaseProjectile::SetWeaponProjectileState(EWeaponProjectileState NewArrowState)
{
	if (ProjectileState != NewArrowState)
	{
		OnStateEnd(ProjectileState); //그 전 상태를 끝낸다
		ProjectileState = NewArrowState; //새로운 상태로 설정
		OnStateBegin(ProjectileState); //새로운 상태를 시작한다
	}
}

void ACBaseProjectile::OnStateBegin(EWeaponProjectileState NewArrowState)
{
	switch (NewArrowState)
	{
	case EWeaponProjectileState::UnObtained:
		ItemStaticMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"), true);
		break;

	case EWeaponProjectileState::InWeapon:
		ItemStaticMesh->SetCollisionProfileName(TEXT("NoCollision"), true);
		break;

	case EWeaponProjectileState::InAir:
		ProjectileCollisionComponent->EnableCollision(ECollisionTraceTarget::None);
		SetProjectileVelocity();
		break;

	default: break; ////////////////////////////////////Default Break///////////////////////////
	}
}

void ACBaseProjectile::OnStateEnd(EWeaponProjectileState PrevArrowState)
{
	if (PrevArrowState == EWeaponProjectileState::InAir)
	{
		//프로젝타일 무브번트 컴포넌트가 존재한다면 소멸시킨다.
		if (IsValid(ProjectileComponent))
			ProjectileComponent->DestroyComponent();

		ProjectileCollisionComponent->DisableCollision(); //콜리전 컴포넌트의 콜리전을 비활성화
		ParticleTrail->EndTrails(); //파티클트레일컴포넌트의 트레일 이미터를 종료
		SetLifeSpan(5.0f); //5초뒤에 소멸한다
	}
}

void ACBaseProjectile::AdjustDeathPhysicsVelocity()
{
	if (LastHit.BoneName == TEXT("head"))
		DeathPhysicsVelocity = DeathPhysicsVelocity * HeadShotMultiplier;
	else
		DeathPhysicsVelocity = DefaultDeathPhysicsVelocity;
}

float ACBaseProjectile::LerpDamageStat(float InStat, float InChargeAmount)
{
	return UKismetMathLibrary::Lerp(InStat * 0.5f, InStat, InChargeAmount);
}

void ACBaseProjectile::StoreAttackInfo(UCAttackInfo* InAttackInfo)
{
	AttackInfo = InAttackInfo;
}

void ACBaseProjectile::SetProjectileVelocity()
{
	//발사 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, ShootProjectileSound, GetActorLocation());

	//스켈레탈 메쉬의 콜리전 설정
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemSkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);

	//프로젝타일 무브먼트 컴포넌트를 생성합니다
	ProjectileComponent = NewObject<UProjectileMovementComponent>(this, UProjectileMovementComponent::StaticClass(),
	                                                              FName(TEXT("ProjectileComponent")));
	CHECK(ProjectileComponent != nullptr);
	ProjectileComponent->RegisterComponent();
	ProjectileComponent->InitialSpeed = (GetActorForwardVector() * ProjectileInitialSpeed).Size();
	ProjectileComponent->bRotationFollowsVelocity = bRotationFollowsVelocity;
	ProjectileComponent->Velocity = GetActorForwardVector() * ProjectileInitialSpeed;

	UKismetSystemLibrary::Delay(this, 0.3f, ProjectileLogicLatentInfo);
	ParticleTrail->Activate(false);
}

void ACBaseProjectile::ShotProjectileAfter()
{
	if (IsValid(ProjectileComponent))
		ProjectileComponent->ProjectileGravityScale = 0.3f;
}

void ACBaseProjectile::FireProjectile(float InRegularDamage, float InPoiseDamage)
{
	FTransform L_SpawnTransform;
	L_SpawnTransform.SetLocation(GetActorLocation());
	L_SpawnTransform.SetRotation(FQuat(GetDirection()));
	L_SpawnTransform.SetScale3D(FVector::OneVector);

	ACBaseProjectile* L_SpawnProjectile =
		GetWorld()->SpawnActorDeferred<ACBaseProjectile>(this->GetClass(), L_SpawnTransform, GetOwner(), GetOwner()->GetInstigator());
	L_SpawnProjectile->InitializeSpawnData(EWeaponProjectileState::InAir, InRegularDamage, InPoiseDamage, AttackInfo, DataAsset);
	UGameplayStatics::FinishSpawningActor(L_SpawnProjectile, L_SpawnTransform);

	ProjectileCollisionComponent->DisableCollision();
	Destroy();
}

void ACBaseProjectile::OnHit_ProjectileComponent(FHitResult HitResult, ECollisionChannel HitCollisionChannel)
{
	LastHit = HitResult; //마지막 히트설정

	//폰이라면 
	if (HitCollisionChannel == ECC_Pawn)
	{
		//데미지를 준다
		TryToApplyDamage(LastHit.GetActor());
		SetWeaponProjectileState(EWeaponProjectileState::UnObtained);
	}
	else //폰이 아니라면
	{
		SetWeaponProjectileState(EWeaponProjectileState::UnObtained);

		IInterface_Combat* Combat_Interface = Cast<IInterface_Combat>(GetOwner());
		CHECK(Combat_Interface != nullptr);
		Combat_Interface->I_PlayImpactEffect(DataAsset->GetImpactEffect(EImpactDecision::NotBlocked), LastHit.ImpactPoint,
		                                     UGameplayStatics::GetSurfaceType(LastHit), false);
	}
}

void ACBaseProjectile::OnCollisionEnabled_ProjectileComponent(ECollisionTraceTarget TraceTarget)
{
	ProjectileCollisionComponent->SetCollisionMesh(GetItemMeshComponent(), GetItemMeshComponent()->GetAllSocketNames());
}

void ACBaseProjectile::MaterialImpactEffect(EPhysicalSurface InPhysicalSurface)
{
	SurfaceType = InPhysicalSurface;
	ImpactPoint = LastHit.ImpactPoint;
}

void ACBaseProjectile::TryToApplyDamage(AActor* InHitActor)
{
	ACharacter* L_CastCharacter = Cast<ACharacter>(InHitActor); 
	if (L_CastCharacter != nullptr) return; //캐릭터로 캐스팅 할 수 없다면 함수를 종료합니다


	HitCharacter = L_CastCharacter;
	HitBoneName = LastHit.BoneName; //뼈 이름을 저장

	//카메라 흔들림을 시작합니다
	CHECK(CameraShake_Class != nullptr);
	UGameplayStatics::GetPlayerController(this, 0)->ClientStartCameraShake(CameraShake_Class);

	//죽을때 물리속도를 설정
	AdjustDeathPhysicsVelocity();
	//히트컴포넌트 로직을 추가합니다
	// 	UHitReactionComponent* L_EnemyHitReactionComponent = Cast<UHitReactionComponent>(InHitActor->GetComponentByClass(UHitReactionComponent::StaticClass()));
	// 	CHECK(L_EnemyHitReactionComponent != nullptr);
	//
	// 	IDamage_Interface* IDamage = Cast<IDamage_Interface>(GetOwner());
	// 	CHECK(IDamage!=nullptr);
	// 	LastCombatHidData = IDamage->GetCombatHitData(LastHit);
	//
	// 	L_EnemyHitReactionComponent->StoreCombatHitData(LastCombatHidData);
	//
	// 	FDamageEvent ProjectileDamageEvent;
	// 	ProjectileDamageEvent.DamageTypeClass = UProjectileDamageType::StaticClass();
	// 	InHitActor->TakeDamage(GetDamageWithMultipliers(), ProjectileDamageEvent, GetOwner()->GetInstigatorController(), this);
	//
	// 	FDamageEvent PoiseDamageEvent;
	// 	PoiseDamageEvent.DamageTypeClass = UPoiseDamageType::StaticClass();
	// 	InHitActor->TakeDamage(PoiseDamage, PoiseDamageEvent, GetOwner()->GetInstigatorController(), this);
	//
	// 	FDamageEvent AdditionalDamageEvent;
	// 	PoiseDamageEvent.DamageTypeClass = AdditionalDamageType;
	// 	InHitActor->TakeDamage(AdditionalDamageTypeAmount, AdditionalDamageEvent, GetOwner()->GetInstigatorController(), this);
	//
	// 	if (IsValid(L_EnemyHitReactionComponent))
	// 	{
	// 		L_EnemyHitReactionComponent->StoreEnemyAttackInfo(AttackInfo);
	//
	// 		auto L_ImpactEffect = DataAsset->GetImpactEffect(EImpactDecision::NotBlocked);
	// 		auto L_SurfaceType = UGameplayStatics::GetSurfaceType(LastHit);
	// 		L_EnemyHitReactionComponent->OnApplyImpactEffect.Broadcast(L_ImpactEffect, L_SurfaceType, LastHit.ImpactPoint);
	// 		L_EnemyHitReactionComponent->OnApplyHitEffect.Broadcast();
	// 		L_EnemyHitReactionComponent->ApplyCameraSake(L_ImpactEffect);
	// 	}
}

FRotator ACBaseProjectile::GetDirection()
{
	FVector2D ViewPortSize;
	GEngine->GameViewport->GetViewportSize(ViewPortSize);
	ViewPortSize = ViewPortSize / 2.f; //스크린 중앙위치를 3D백터로 변환합니다

	//스크린포지션을 월드포지션으로 변환합니다
	FVector L_Position;
	FVector L_Direction;
	auto L_PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	L_PlayerController->DeprojectScreenPositionToWorld(ViewPortSize.X, ViewPortSize.Y, L_Position, L_Direction);

	FVector L_StartTrace = L_Position;
	FVector L_EndTrace = L_Position + (L_Direction * 3000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); //WorldStatic :  EObjectTypeQuery::ObjectTypeQuery1
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); //Pawn : EObjectTypeQuery::ObjectTypeQuery3

	TArray<AActor*> L_LineIgnoreActors;
	L_LineIgnoreActors.Add(this);
	L_LineIgnoreActors.Add(GetOwner());
	FHitResult L_LineHit;

	bool L_IsLineHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, L_StartTrace, L_EndTrace,
	                                                                   ObjectTypes, false, L_LineIgnoreActors,
	                                                                   EDrawDebugTrace::None, L_LineHit, true);

	//히트가 되었다면
	if (L_IsLineHit)
	{
		ACharacter* L_CastCharacter = Cast<ACharacter>(L_LineHit.GetActor());
		//캐스트에 성공했다면
		if (L_CastCharacter != nullptr)
		{
			FVector L_BetweenHit = L_Direction * L_CastCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
			HitLocation = L_LineHit.Location + L_BetweenHit;

			bool L_IsForBreak = false;

			//반복문을 돌립니다
			for (int32 i = 1; i <= 10; i++)
			{
				if (L_IsForBreak) break;

				FVector L_SphereStart = LastHit.Location;
				float L_IntervalDistance = i * 5.0f;
				FVector L_SphereEnd = L_SphereStart + (L_Direction * L_IntervalDistance);

				TArray<TEnumAsByte<EObjectTypeQuery>> L_SphereObjectTypes;
				L_SphereObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
				TArray<AActor*> L_SphereIgnoreActors;
				TArray<FHitResult> L_SphereHits;

				bool L_SphereIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(this, L_SphereStart, L_SphereEnd,
				                                                                      2.0f, L_SphereObjectTypes, false,
				                                                                      L_SphereIgnoreActors, EDrawDebugTrace::None, L_SphereHits,
				                                                                      true);

				//히트했다면
				if (L_SphereIsHit)
				{
					for (auto f_SphereHit : L_SphereHits)
					{
						//히트한 컴포넌트의 프로파일 네임이 폰이 아니라면
						if (f_SphereHit.GetComponent()->GetCollisionProfileName() != FName(TEXT("Pawn")))
						{
							HitLocation = f_SphereHit.Location;
							L_IsForBreak = true;
							break;
						}
					}
				}
			} //반복문 끝내는 구간
			FVector L_DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitLocation);
			return UKismetMathLibrary::Conv_VectorToRotator(L_DirectionVector);
		}
		else //캐스트에 실패했다면
		{
			HitLocation = L_LineHit.Location;
			FVector L_DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitLocation);
			return UKismetMathLibrary::Conv_VectorToRotator(L_DirectionVector);
		}
	}
	else //히트 되지 않았다면
	{
		HitLocation = L_LineHit.TraceEnd;
		FVector L_DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitLocation);
		return UKismetMathLibrary::Conv_VectorToRotator(L_DirectionVector);
	}
}

float ACBaseProjectile::GetDamageWithMultipliers()
{
	const bool L_HitHead = LastHit.BoneName == FName(TEXT("head"));
	return UKismetMathLibrary::SelectFloat(RegularDamage * HeadShotMultiplier, RegularDamage, L_HitHead);
}

void ACBaseProjectile::I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage)
{
}

void ACBaseProjectile::I_ShootProjectile(float InRegularDamage, float InPoiseDamage)
{
	FireProjectile(InRegularDamage, InPoiseDamage);
}

void ACBaseProjectile::I_InitializeQuiver(int32 Arrows, UStaticMesh* ArrowMesh)
{
}

void ACBaseProjectile::I_UpdateQuiver(int32 InAmount)
{
}

EWeaponProjectileState ACBaseProjectile::I_GetProjectileState()
{
	return ProjectileState;
}
