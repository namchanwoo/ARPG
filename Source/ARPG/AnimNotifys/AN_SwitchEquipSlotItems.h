
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Datas/ItemData.h"
#include "AN_SwitchEquipSlotItems.generated.h"




UCLASS()
class ARPG_API UAN_SwitchEquipSlotItems : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
	EEquipSlots EquipSlots;
};
