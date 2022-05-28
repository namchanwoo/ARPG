#include "CBaseItem.h"

#include "Global.h"
#include "Character/CRPGCharacter.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"


ACBaseItem::ACBaseItem()
{
	//Create Component
	{
		DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
		SetRootComponent(DefaultSceneRoot);

		ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
		ItemStaticMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
		ItemStaticMesh->SetupAttachment(DefaultSceneRoot);


		ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemSkeletalMesh");
		ItemSkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
		ItemSkeletalMesh->SetupAttachment(DefaultSceneRoot);
	}
}

void ACBaseItem::BeginPlay()
{
	Super::BeginPlay();

	UpdateWorldItemMesh(); //월드 아이템 메쉬를 업데이트합니다

	OnDestroyed.AddDynamic(this, &ACBaseItem::Event_OnDestroyed); //액터가 삭제될 때 호출되는  Event_OnDestroyed 함수를 연결합니다

	//1.메쉬의 타입을 설정해줍니다
	{
		//스켈레탈 메쉬컴포넌트의 스켈레탈 메쉬가 유효하다면
		if (IsValid(ItemSkeletalMesh->SkeletalMesh))
		{
			//데이터 에셋이 유효하다면
			if (IsValid(DataAsset))
			{
				//메쉬 타입을 스켈레탈 메쉬로 설정 해주고 데이테에셋의 애니메이션인스턴스가 존재한다면 애님인스턴스를 설정합니다
				MeshType = EMeshType::SkeletalMesh;
				if (IsValid(DataAsset->AnimationBlueprint))
					ItemSkeletalMesh->SetAnimInstanceClass(DataAsset->AnimationBlueprint);
			}
		}
		else //스켈레탈 메쉬 컴포넌트의 스켈레탈 메쉬가 유효하지 않다면
		{
			//아이템 스텍틱 메쉬의 컴포넌트가 유효하다면
			if (IsValid(ItemStaticMesh->GetStaticMesh()))
				MeshType = EMeshType::StaticMesh;
			else //아이템 스텍틱 메쉬의 컴포넌트가 유효하지 않다면
				MeshType = EMeshType::None;
		}
	}

	//2.메쉬가 유효하다면 상대좌표로 트랜스폼을 설정합니다
	{
		//데이터 에셋이 유효하다면
		if (IsValid(DataAsset))
		{
			USceneComponent* L_SelectMesh = nullptr;
			switch (MeshType)
			{
			case EMeshType::None: break;

			case EMeshType::StaticMesh:
				L_SelectMesh = ItemStaticMesh;
				break;

			case EMeshType::SkeletalMesh:
				L_SelectMesh = ItemSkeletalMesh;
				break;
			}

			//메쉬가 유효하다면
			if (IsValid(L_SelectMesh))
				L_SelectMesh->SetRelativeTransform(DataAsset->RelativeTransform, false, nullptr, ETeleportType::None);
		}
	}
}

void ACBaseItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateWorldItemMesh();
}

void ACBaseItem::Event_OnDestroyed(AActor* InDestroyedActor)
{
	OnItemUnEquipped();
}

bool ACBaseItem::AttachActor(FName InSocketName, bool IsUseExtraMesh)
{
	const auto L_CastCharacter = Cast<ACRPGCharacter>(GetOwner());
	const auto L_AttachParent = L_CastCharacter->GetMesh();
	CHECK(L_CastCharacter!=nullptr||L_AttachParent!=nullptr, false);
	AttachToComponent(L_AttachParent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), InSocketName);
	return true;
}

void ACBaseItem::UpdateWorldItemMesh()
{
	//데이터 에셋이 유효하다면
	if (IsValid(DataAsset))
	{
		//데이터에셋의 아이템메쉬가 유효하다면
		if (IsValid(DataAsset->ItemMesh))
			ItemStaticMesh->SetStaticMesh(DataAsset->ItemMesh);
		else //데이터에셋의 아이템메쉬가 유효하지 않다면
		{
			//데이터에셋의 아이템 스켈레탈 메쉬가 유효하다면
			if (IsValid(DataAsset->ItemSkeletalMesh))
				ItemSkeletalMesh->SetSkeletalMesh(DataAsset->ItemSkeletalMesh);
		}
	}
}

void ACBaseItem::SetItemMeshSimulatePhysics()
{
	//스테틱메쉬의 아이템메쉬가 유효하다면
	if (IsValid(ItemStaticMesh->GetStaticMesh()))
	{
		ItemStaticMesh->SetCollisionProfileName(FName(TEXT("Ragdoll")));
		ItemStaticMesh->SetSimulatePhysics(true);
	}
	else //스테틱메쉬의 아이템메쉬가 유효하지 않다면
	{
		//스켈레탈메쉬 컴포넌트의 스켈레탈메쉬가 유효하다면
		if (IsValid(ItemSkeletalMesh->SkeletalMesh))
		{
			ItemSkeletalMesh->SetCollisionProfileName(FName(TEXT("Ragdoll")));
			ItemSkeletalMesh->SetSimulatePhysics(true);
		}
	}
}

void ACBaseItem::OnWorldItemHidden(bool InHidden)
{
}

void ACBaseItem::OnUse()
{
}

void ACBaseItem::OnItemEquipped()
{
	SetItemEquipped(true);
}

void ACBaseItem::OnItemUnEquipped()
{
	SetItemEquipped(false);
}

void ACBaseItem::SetItemEquipped(bool InCondition)
{
	bIsItemEquipped = InCondition;
}

UAnimInstance* ACBaseItem::GetItemAnimInstance()
{
	if (IsValid(ItemSkeletalMesh->GetAnimInstance()))
		return ItemSkeletalMesh->GetAnimInstance();

	return nullptr;
}

ACharacter* ACBaseItem::GetOwnerCharacter()
{
	return Cast<ACharacter>(GetOwner());
}

UPrimitiveComponent* ACBaseItem::GetItemMeshComponent()
{
	switch (MeshType)
	{
	case EMeshType::StaticMesh:
		return ItemStaticMesh;

	case EMeshType::SkeletalMesh:
		return ItemSkeletalMesh;
		
	default: return nullptr;
	}
}

bool ACBaseItem::IsItemEquipped()
{
	return bIsItemEquipped;
}

void ACBaseItem::I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage)
{
}

EWeaponProjectileState ACBaseItem::I_GetProjectileState()
{
	return {};
}

void ACBaseItem::I_ShootProjectile(float InRegularDamage, float InPoiseDamage)
{
}

void ACBaseItem::I_InitializeQuiver(int32 InArrows, UStaticMesh* InArrowMesh)
{
}

void ACBaseItem::I_UpdateQuiver(int32 InAmount)
{
}

