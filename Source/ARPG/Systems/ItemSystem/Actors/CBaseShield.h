
#pragma once

#include "CoreMinimal.h"
#include "CBaseWeapon.h"
#include "CBaseShield.generated.h"



UCLASS()
class ARPG_API ACBaseShield : public ACBaseWeapon
{
	GENERATED_BODY()

public:
	ACBaseShield();
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void OnItemEquipped() override;

	virtual void OnItemUnEquipped() override;

};
