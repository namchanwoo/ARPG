#pragma once

#include "CoreMinimal.h"
#include "CBaseProjectile.h"
#include "CBaseArcherProjectile.generated.h"


UCLASS()
class ARPG_API ACBaseArcherProjectile : public ACBaseProjectile
{
	GENERATED_BODY()
public:
	ACBaseArcherProjectile();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetProjectileVelocity() override;

	//화살 발사체를 발사합니다
	void ShootArrowProjectile();
	
	virtual void MaterialImpactEffect(EPhysicalSurface InPhysicalSurface) override;
	
	virtual void OnHit_ProjectileComponent(FHitResult HitResult, ECollisionChannel HitCollisionChannel) override;



#pragma region                           IInterface_WorldItem
public:
	virtual void I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage) override;
#pragma endregion                        IInterface_WorldItem
	


	
public:
	/**화살 최소 초기속도*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Archer Projectile | Initialize")
	float ArrowMinimumInitialSpeed;

	/**화살 관통(침투)력*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Archer Projectile | Initialize")
	float ArrowPenetration;

	/**어태치 할 물리표면*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Archer Projectile | Initialize")
	TArray<TEnumAsByte<EPhysicalSurface>> SurfaceTypesToAttach;

	/**활 차지(양)*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Archer Projectile | Projectile Parameters")
	float BowChargeAmount;
	
};
