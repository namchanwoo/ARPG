#include "CBaseWeapon.h"

#include "Global.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Particles/ParticleSystemComponent.h"
#include "Systems/CombatSystem/CustomCollisionComponent.h"


ACBaseWeapon::ACBaseWeapon()
{
	//Weapon Component 생성 및 초기화
	{
		//웨폰루트 만들기
		WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
		WeaponRoot->SetupAttachment(RootComponent);

		//초점 콜리전 만들기
		FocusCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FocusCollision"));
		FocusCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 74.0f));
		FocusCollision->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
		FocusCollision->AreaClass = UNavArea_Obstacle::StaticClass();
		FocusCollision->SetCollisionProfileName(TEXT("Custom"));
		FocusCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FocusCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		FocusCollision->SetCollisionResponseToChannel(ECC_Interactable, ECR_Block);
		FocusCollision->SetupAttachment(WeaponRoot);

		//무기 방향 만들기
		MeleeWeaponGuideArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MeleeWeaponGuideArrow"));
		MeleeWeaponGuideArrow->SetRelativeLocation(FVector(-0.5f, 1.0f, -13.f));
		MeleeWeaponGuideArrow->SetRelativeRotation(FRotator(90.0f, 90.0f, 540.f));
		MeleeWeaponGuideArrow->SetRelativeScale3D(FVector(0.8f, 0.0f, -0.03f));
		MeleeWeaponGuideArrow->ArrowSize = 20.0f;
		MeleeWeaponGuideArrow->SetupAttachment(WeaponRoot);

		//트레일 파티클 컴포넌트 만들기
		ParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleTrail"));
		ParticleTrail->PrimaryComponentTick.bStartWithTickEnabled = false;
		ParticleTrail->SetupAttachment(RootComponent);


		//Create CollisionComponent
		WeaponCollisionComponent = CreateDefaultSubobject<UCustomCollisionComponent>(TEXT("WeaponCollisionComponent"));
		SecondWeaponCollisionComponent = CreateDefaultSubobject<UCustomCollisionComponent>(TEXT("SecondWeaponCollisionComponent"));
	}

	WeaponStartSocket = FName(TEXT("WeaponStart"));
	WeaponEndSocket = FName(TEXT("WeaponEnd"));
}

void ACBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponCollisionComponent->Event_InitializeCollision(GetOwner());
	SecondWeaponCollisionComponent->Event_InitializeCollision(GetOwner());

	const auto L_CastCharacter = Cast<ACharacter>(GetOwner());
	if (L_CastCharacter != nullptr)
		OwnerCharacter = L_CastCharacter;

	OwnerCollisionComponent = Cast<UCustomCollisionComponent>(OwnerCharacter->GetComponentByClass(UCustomCollisionComponent::StaticClass()));

	WeaponCollisionComponent->OnCollisionEnabled.AddDynamic(this, &ACBaseWeapon::OnCollisionEnabled_WeaponCollision);
	SecondWeaponCollisionComponent->OnCollisionEnabled.AddDynamic(this, &ACBaseWeapon::ACBaseWeapon::OnCollisionEnabled_SecondWeaponCollision);
	WeaponCollisionComponent->OnHit.AddDynamic(this, &ACBaseWeapon::ACBaseWeapon::OnHit_WeaponCollision);
	SecondWeaponCollisionComponent->OnHit.AddDynamic(this, &ACBaseWeapon::ACBaseWeapon::ACBaseWeapon::OnHit_SecondWeaponCollision);

	// //핸드 타입이 양손을 쓴다면
	// if (DataAsset->WeaponHandType == EItemHandType::EIHT_DualHand)
	// {
	// 	//스테틱 메쉬를 가져옵니다
	// 	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetItemMeshComponent());
	// 	//스테틱 메쉬 캐스트가 되었다면
	// 	if (StaticMeshComponent != nullptr)
	// 	{
	// 		//스테틱메쉬컴포넌트를 만듭니다
	// 		SecondWeaponStaticMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName(TEXT("SecondWeaponStaticMesh")));
	// 		if (SecondWeaponStaticMesh != nullptr)
	// 		{
	// 			SecondWeaponStaticMesh->RegisterComponent();
	// 			SecondWeaponStaticMesh->SetRelativeTransform(GetItemMeshComponent()->GetRelativeTransform());
	// 			SecondWeaponStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 			SecondWeaponStaticMesh->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
	// 		}
	// 	}
	// 	else //스테틱 메쉬 캐스팅에 실패 했다면 스켈레탈이라는겁니다 하하
	// 	{
	// 		SecondWeaponSkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetItemMeshComponent());
	// 		if (SecondWeaponSkeletalMeshComponent != nullptr)
	// 		{
	// 			SecondWeaponSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 			SecondWeaponSkeletalMeshComponent->SetSkeletalMesh(Cast<USkeletalMeshComponent>(GetItemMeshComponent())->SkeletalMesh);
	// 		}
	// 	}
	// }
}

void ACBaseWeapon::OnHit(FHitResult InHitResult, ECollisionChannel InCollisionChannel)
{
	if (OwnerCollisionComponent->OnHit.IsBound())
		OwnerCollisionComponent->OnHit.Broadcast(InHitResult, InCollisionChannel);
}

void ACBaseWeapon::OnCollisionEnabled_WeaponCollision(ECollisionTraceTarget TraceTarget)
{
	WeaponCollisionComponent->SetCollisionMesh(GetItemMeshComponent(), GetItemMeshComponent()->GetAllSocketNames());
}

void ACBaseWeapon::OnCollisionEnabled_SecondWeaponCollision(ECollisionTraceTarget TraceTarget)
{
	WeaponCollisionComponent->SetCollisionMesh(GetItemMeshComponent(), GetItemMeshComponent()->GetAllSocketNames());
}

void ACBaseWeapon::OnHit_WeaponCollision(FHitResult InHitResult, ECollisionChannel InCollisionChannel)
{
	LastHit = InHitResult;
	if (InCollisionChannel == ECC_Pawn)
		OnHit(LastHit, ECC_Pawn);
	else
		OnHit(LastHit, InCollisionChannel);
}

void ACBaseWeapon::OnHit_SecondWeaponCollision(FHitResult InHitResult, ECollisionChannel InCollisionChannel)
{
	LastHit = InHitResult;
	if (InCollisionChannel == ECC_Pawn)
		OnHit(LastHit, ECC_Pawn);
	else
		OnHit(LastHit, InCollisionChannel);
}

void ACBaseWeapon::StartTrail(UParticleSystem* InWeaponTrail, bool UseNotifyTrail)
{
	const auto L_TrailTemplate = UseNotifyTrail ? InWeaponTrail : ParticleTrail->Template;
	CHECK(L_TrailTemplate!=nullptr);

	WeaponTrail = L_TrailTemplate;

	//아이템의 메쉬유형에 따라 스폰할 씬 컴포넌트를 설정
	USceneComponent* L_SpawnScene =
		Cast<USceneComponent>(UKismetMathLibrary::SelectObject(ItemStaticMesh, ItemSkeletalMesh, MeshType == EMeshType::StaticMesh));

	ParticleSystem = UGameplayStatics::SpawnEmitterAttached(WeaponTrail, L_SpawnScene, WeaponStartSocket, ItemStaticMesh->GetRelativeLocation());

	if (IsValid(ParticleSystem))
	{
		ParticleSystem->SetTemplate(WeaponTrail);
		ParticleSystem->BeginTrails(WeaponStartSocket, WeaponEndSocket, ETrailWidthMode_FromFirst, 1.0f);
	}
}

void ACBaseWeapon::EndTrail()
{
	if (IsValid(ParticleSystem))
		ParticleSystem->EndTrails();
}

UPrimitiveComponent* ACBaseWeapon::GetSecondWeaponMesh()
{
	if (IsValid(SecondWeaponStaticMesh))
		return SecondWeaponStaticMesh;
	if (IsValid(SecondWeaponCollisionComponent))
		return SecondWeaponSkeletalMeshComponent;
	return nullptr;
}
