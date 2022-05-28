#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Datas/StatData.h"
#include "GlobalFunctions/DoOnce.h"
#include "UW_StatBar.generated.h"


class UCStatsManagerComponent;
class UProgressBar;
class UBorder;
class USizeBox;
class UOverlay;


UCLASS()
class ARPG_API UUW_StatBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UUW_StatBar(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;

public:
	void InitializeStatBar(UCStatsManagerComponent* OwnerStatComponent);

	UFUNCTION()
	void OnCurrentValueUpdated(EStats StatUpdated, float CurrentValue, float MaxValue);

	UFUNCTION()
	void OnMaxStatValueUpdated(EStats StatUpdated, float Value);

	void UpdateStatBarPercent(float InPercent);

	UFUNCTION()
	void UpdateLerpBar();

	void ResetDoOnce();

#pragma region                                   Designer Members
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	UOverlay* Overlay_StatBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	USizeBox* SizeBox_LerpStatBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	UBorder* Border_LerpStatBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	UProgressBar* ProgressBar_LerpStatBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	USizeBox* SizeBox_StatBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Designer", meta = (BindWidget))
	UProgressBar* ProgressBar_StatBar;

#pragma endregion                                Designer Members


#pragma region                                   Properties Members
public:
	/**스텟의 종류*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	EStats StatBarType;

	/**스텟바 스타일*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	FProgressBarStyle StatBarStyle;

	/**러프 스텟바 스타일*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	FProgressBarStyle LerpBarStyle;

	/**스텟 퍼센트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	float StatPercent;

	/**러프스테이트바를 업데이트 해야 하는 지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	bool bUpdateLerpStatBar;

	/**스텟바의 채우기 컬러 및 투명도*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	FLinearColor StatBarFillColorAndOpacity;

	/**러프 스텟바의 채우기 컬러 및 투명도*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	FLinearColor LerpStatBarFillColorAndOpacity;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	float LerpStaBarPercentAlpha;

	/**StatBar의 사이즈 배율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	float StatBarSizeMultiplier;

	/**Width를 더 크게 늘려야 하는 지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	bool bMaxStatUpdateWidth;

	/**늘려야 할 Width */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties", meta = (BindWidget))
	float CustomWidthOverride;
#pragma endregion                                Properties Members


	FDoOnce DO_CVUpdated;
};
