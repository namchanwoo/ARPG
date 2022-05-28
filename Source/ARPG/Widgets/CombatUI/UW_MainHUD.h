#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Datas/StatData.h"
#include "UW_MainHUD.generated.h"

class UCRPGITemAsset;
class UCStatsManagerComponent;
class UOverlay;
class UUW_StatBar;

UCLASS()
class ARPG_API UUW_MainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	/**스텟바를 초기화 합니다*/
	UFUNCTION(BlueprintCallable)
	void Initialize_StatBars(UCStatsManagerComponent* OwnerStatsComponent);
	
	/**스텟 이벤트를 등록합니다*/
	void AssignStatsEvents(UCStatsManagerComponent* InStatsManagerComponent);

public:
	UFUNCTION()
	void OnStatusEffectApplied(EStatusEffects InStatusEffects);
	
	UFUNCTION()
	void OnStatusEffectRemoved(EStatusEffects InStatusEffects);
	
	UFUNCTION()
	void SetStatusEffectBarEnabled(EStatusEffects InStatusEffects, bool IsEnable);
	

public:
	UUW_StatBar* GetStatBar(EStats InStat);

	UUW_StatBar* GetStatusBar(EStatusEffects InStatusEffect);


#pragma region                               Designer
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Designer", meta = (BindWidget))
	UUW_StatBar* HealthStatBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Designer", meta = (BindWidget))
	UUW_StatBar* ManaStatBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Designer", meta = (BindWidget))
	UUW_StatBar* StaminaStatBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Designer", meta = (BindWidget))
	UUW_StatBar* PoisonResist;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Designer", meta = (BindWidget))
	UUW_StatBar* BurningResist;
#pragma endregion                            Designer


#pragma region                               Properties Members
public:

#pragma endregion                            Properties Members
};
