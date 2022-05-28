#include "CBaseArcherProjectile.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/Interface_Combat.h"
#include "Particles/ParticleSystemComponent.h"
#include "Systems/CombatSystem/CustomCollisionComponent.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"

ACBaseArcherProjectile::ACBaseArcherProjectile()
{
	BowChargeAmount = 1.0f;
	ArrowMinimumInitialSpeed = 1500.f;
	ArrowPenetration = 50.f;
	SurfaceTypesToAttach.Add(PhysicalSurface_Flesh);
	SurfaceTypesToAttach.Add(PhysicalSurface_Wood);
}

void ACBaseArcherProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ACBaseArcherProjectile::SetProjectileVelocity()
{
	//발사 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, ShootProjectileSound, GetActorLocation());

	//스켈레탈 메쉬의 콜리전 설정
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemSkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	//프로젝타일 무브먼트 컴포넌트를 생성합니다
	ProjectileComponent = NewObject<UProjectileMovementComponent>(this, UProjectileMovementComponent::StaticClass(),
	                                                              FName(TEXT("ProjectileComponent")));
	CHECK(ProjectileComponent != nullptr);
	const float L_BowChargeSpeed = UKismetMathLibrary::Lerp(ArrowMinimumInitialSpeed, ProjectileInitialSpeed, BowChargeAmount);
	ProjectileComponent->InitialSpeed = (GetActorForwardVector() * L_BowChargeSpeed).Size();
	ProjectileComponent->Velocity = GetActorForwardVector() * L_BowChargeSpeed;
	ProjectileComponent->bRotationFollowsVelocity = bRotationFollowsVelocity;
	ProjectileComponent->RegisterComponent();


	UKismetSystemLibrary::Delay(this, 0.3f, ProjectileLogicLatentInfo);
	ParticleTrail->Activate(false);
}

void ACBaseArcherProjectile::ShootArrowProjectile()
{
	FTransform L_SpawnTransform;
	L_SpawnTransform.SetLocation(GetActorLocation());
	L_SpawnTransform.SetRotation(FQuat(GetDirection()));
	L_SpawnTransform.SetScale3D(FVector::OneVector);

	ACBaseArcherProjectile* ArcherProjectile =
		GetWorld()->SpawnActorDeferred<ACBaseArcherProjectile>(this->GetClass(), L_SpawnTransform, GetOwner(), GetOwner()->GetInstigator());

	ArcherProjectile->BowChargeAmount = BowChargeAmount;
	ArcherProjectile->SpawnArrowState = EWeaponProjectileState::InAir;
	ArcherProjectile->RegularDamage = LerpDamageStat(RegularDamage, BowChargeAmount);
	ArcherProjectile->PoiseDamage = LerpDamageStat(PoiseDamage, BowChargeAmount);
	ArcherProjectile->AttackInfo = AttackInfo;
	ArcherProjectile->DataAsset = DataAsset;
	UGameplayStatics::FinishSpawningActor(ArcherProjectile, L_SpawnTransform);

	ProjectileCollisionComponent->DisableCollision();
	Destroy();
}

void ACBaseArcherProjectile::MaterialImpactEffect(EPhysicalSurface InPhysicalSurface)
{
	SurfaceType = InPhysicalSurface;
	ImpactPoint = LastHit.ImpactPoint;
}

void ACBaseArcherProjectile::OnHit_ProjectileComponent(FHitResult HitResult, ECollisionChannel HitCollisionChannel)
{
	LastHit = HitResult;

	//히트 채널이 폰인 경우
	if (HitCollisionChannel == ECC_Pawn)
	{
		TryToApplyDamage(LastHit.GetActor());
		SetWeaponProjectileState(EWeaponProjectileState::UnObtained);

		//히트캐릭터가 존재하는 경우
		if (IsValid(HitCharacter))
		{
			bool L_Hit = false;
			switch (LastCombatHidData.ImpactDecision)
			{
			case EImpactDecision::None: L_Hit = false;
				break;
			case EImpactDecision::NotBlocked: L_Hit = true;
				break;
			case EImpactDecision::Blocked: L_Hit = false;
				break;
			case EImpactDecision::PerfectBlocked: L_Hit = false;
				break;
			case EImpactDecision::Parried: L_Hit = true;
				break;
			case EImpactDecision::DefenseBroken: L_Hit = false;
				break;
			}

			if (L_Hit) //스왑인 경우
			{
				FVector NewLocation = LastHit.ImpactPoint - (GetActorForwardVector() * ArrowPenetration);
				if (SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics))
				{
					AttachToComponent(HitCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), LastHit.BoneName);
					SetWeaponProjectileState(EWeaponProjectileState::StuckInObject);
				}
			}
			else //스왑이 아닌 경우
			{
				ItemStaticMesh->SetSimulatePhysics(true);
				ItemStaticMesh->SetCollisionProfileName(FName(TEXT("ragdoll")));
				const FVector NewVel = GetActorForwardVector() * UKismetMathLibrary::Lerp(-100.0f, -400.0f, BowChargeAmount);
				ItemStaticMesh->SetAllPhysicsLinearVelocity(NewVel, true);
			}
			return;
		}
		else //히트캐릭터가 유효하지 않다면
		{
			//히트물질에 포함되어 있다면
			if (SurfaceTypesToAttach.Contains(UGameplayStatics::GetSurfaceType(LastHit)))
			{
				FVector NewLocation = LastHit.ImpactPoint - (GetActorForwardVector() * ArrowPenetration);
				if (SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics))
				{
					FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
					AttachToActor(LastHit.GetActor(), AttachmentTransformRules);
					SetWeaponProjectileState(EWeaponProjectileState::StuckInObject);
				}
			}
			else //히트 물질에 포함되어 있지 않다면
			{
				ItemStaticMesh->SetSimulatePhysics(true);
				ItemStaticMesh->SetCollisionProfileName(FName(TEXT("ragdoll")));
				FVector NewVel = GetActorForwardVector() * UKismetMathLibrary::Lerp(-100.0f, -400.0f, BowChargeAmount);
				ItemStaticMesh->SetAllPhysicsLinearVelocity(NewVel, true);
			}
		}
	}
	else //폰이 아닌 경우
	{
		IInterface_Combat* Combat_Interface = Cast<IInterface_Combat>(GetOwner());
		if (Combat_Interface != nullptr)
		{
			Combat_Interface->I_PlayImpactEffect(DataAsset->GetImpactEffect(EImpactDecision::NotBlocked), LastHit.ImpactPoint,
			                                     UGameplayStatics::GetSurfaceType(LastHit), false);
			SetWeaponProjectileState(EWeaponProjectileState::UnObtained);
		}

		//히트물질에 포함되어 있다면
		if (SurfaceTypesToAttach.Contains(UGameplayStatics::GetSurfaceType(LastHit)))
		{
			FVector L_NewLocation = LastHit.ImpactPoint - (GetActorForwardVector() * ArrowPenetration);
			if (SetActorLocation(L_NewLocation, false, nullptr, ETeleportType::TeleportPhysics))
			{
				FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
				AttachToActor(LastHit.GetActor(), AttachmentTransformRules);
				SetWeaponProjectileState(EWeaponProjectileState::StuckInObject);
			}
		}
		else //히트 물질에 포함되어 있지 않다면
		{
			ItemStaticMesh->SetSimulatePhysics(true);
			ItemStaticMesh->SetCollisionProfileName(FName(TEXT("ragdoll")));
			FVector NewVel = GetActorForwardVector() * UKismetMathLibrary::Lerp(-100.0f, -400.0f, BowChargeAmount);
			ItemStaticMesh->SetAllPhysicsLinearVelocity(NewVel, true);
		}
	}
}

void ACBaseArcherProjectile::I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage)
{
	BowChargeAmount = InChargeAmount;
	RegularDamage = InRegularDamage;
	PoiseDamage = InPoiseDamage;
	ShootArrowProjectile();
}
