#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interface_WorldItem.h"
#include "CBaseItem.generated.h"

class UCEquipmentManagerComponent;

UCLASS(Blueprintable)
class ARPG_API ACBaseItem : public AActor, public IInterface_WorldItem
{
	GENERATED_BODY()

public:
	ACBaseItem();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	/**액터가 삭제될 때 호출됩니다*/
	UFUNCTION()
	void Event_OnDestroyed(AActor* InDestroyedActor);

public:
	/**액터를 어태치 합니다*/
	UFUNCTION(BlueprintCallable, Category="Master Item")
	virtual bool AttachActor(FName InSocketName, bool IsUseExtraMesh);

	/**월드 아이템의 메쉬를 데이테에셋의 메쉬로 업데이트합니다*/
	void UpdateWorldItemMesh();

	/**아이템메쉬의 물리 시뮬레이션을 설정합니다*/
	void SetItemMeshSimulatePhysics();

public:
	/**월드 아이템의 히든상태를 설정합니다*/
	virtual void OnWorldItemHidden(bool InHidden);

	/**아이템을 사용합니다(자식에서 정의가능)*/
	virtual void OnUse();

	/**아이템을 장착합니다*/
	virtual void OnItemEquipped();

	/**아이템을 장착 해재합니다*/
	virtual void OnItemUnEquipped();

	/**아이템 장착상태를 설정합니다*/
	void SetItemEquipped(bool InCondition);

public:
	/**아이템의 애니메이션 인스턴스를 갸져옵니다*/
	UAnimInstance* GetItemAnimInstance();

	/**아이템을 소유한 캐릭터를 가져옵니다*/
	ACharacter* GetOwnerCharacter();

	/**아이템의 메쉬타입에 따라서 메쉬컴포넌트를 가져옵니다*/
	UPrimitiveComponent* GetItemMeshComponent();

	/**현재 아이템이 장착되어있는 상태인가? 장착되어 있다면 true를 반환합니다*/
	bool IsItemEquipped();


#pragma  region                     IInterface_WorldItem
public:
	/**화살을 발사합니다*/
	virtual void I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage)override;

	/**발사체의 상태를 가져옵니다*/
	virtual EWeaponProjectileState I_GetProjectileState()override;

	/**발사체를 발사합니다*/
	virtual void I_ShootProjectile(float InRegularDamage, float InPoiseDamage)override;

	/**화살통을 초기화 합니다*/
	virtual void I_InitializeQuiver(int32 InArrows, UStaticMesh* InArrowMesh)override;

	/**화살통을 업데이트 합니다*/
	virtual void I_UpdateQuiver(int32 InAmount)override;

#pragma  endregion                  IInterface_WorldItem


	/***************************************************************************************************************Field Members************************/
	/***************************************************************************************************************Field Members************************/
	/***************************************************************************************************************Field Members************************/


protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseItem | Default")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseItem | Default")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseItem | Default")
	USkeletalMeshComponent* ItemSkeletalMesh;

	/**아이템의 장착상태*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="BaseItem | Default")
	bool bIsItemEquipped;
public:
	/**장비컴포넌트를 설정*/
	UPROPERTY(BlueprintReadOnly, Category = "BaseItem | Components",meta=(ExposeOnSpawn="true",AllowPrivateAccess="true"))
	UCEquipmentManagerComponent* EquipmentComponent;
	
	/**아이템데이터에셋*/
	UPROPERTY(BlueprintReadOnly, Category = "BaseItem | Initialize",meta=(ExposeOnSpawn="true",AllowPrivateAccess="true"))
	UCRPGItemAsset* DataAsset;

	/**아이템의 메쉬 타입*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="BaseItem | Mesh Type")
	EMeshType MeshType;
};
