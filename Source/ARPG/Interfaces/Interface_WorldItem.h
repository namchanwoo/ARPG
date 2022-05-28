#pragma once

#include "CoreMinimal.h"
#include "Datas/ItemData.h"
#include "UObject/Interface.h"
#include "Interface_WorldItem.generated.h"


UINTERFACE(MinimalAPI)
class UInterface_WorldItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_API IInterface_WorldItem
{
	GENERATED_BODY()


public:
	
	/**화살을 발사합니다*/
	virtual void I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage) =0;

	/**발사체의 상태를 가져옵니다*/
	virtual EWeaponProjectileState I_GetProjectileState() =0;

	/**발사체를 발사합니다*/
	virtual void I_ShootProjectile(float InRegularDamage, float InPoiseDamage) =0;

	/**화살통을 초기화 합니다*/
	virtual void I_InitializeQuiver(int32 InArrows, UStaticMesh* InArrowMesh) =0;

	/**화살통을 업데이트 합니다*/
	virtual void I_UpdateQuiver(int32 InAmount) =0;
};
