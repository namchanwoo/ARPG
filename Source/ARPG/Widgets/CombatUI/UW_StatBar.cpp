#include "UW_StatBar.h"


#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Systems/CombatSystem/CStatsManagerComponent.h"


UUW_StatBar::UUW_StatBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUpdateLerpStatBar = true;
	StatBarSizeMultiplier = 1.0f;
	bMaxStatUpdateWidth = true;
	CustomWidthOverride = 300.f;
}

void UUW_StatBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	//스텟 바의 스타일을 설정
	ProgressBar_StatBar->WidgetStyle = StatBarStyle;
	//프로그래스바의 채우기색과 투명도를 설정
	ProgressBar_StatBar->SetFillColorAndOpacity(StatBarFillColorAndOpacity);

	//리프 스텟바의 스타일을 설정
	ProgressBar_LerpStatBar->WidgetStyle = LerpBarStyle;
	//리프 스텟바의 채우기색과 투명도를 설정
	ProgressBar_LerpStatBar->SetFillColorAndOpacity(LerpStatBarFillColorAndOpacity);

	//Width 를 업데이트 해야 한다면
	if (!bMaxStatUpdateWidth)
	{
		SizeBox_StatBar->SetWidthOverride(CustomWidthOverride);
		SizeBox_LerpStatBar->SetWidthOverride(CustomWidthOverride);
	}
}

void UUW_StatBar::InitializeStatBar(UCStatsManagerComponent* OwnerStatComponent)
{
	OwnerStatComponent->OnCurrentValueUpdated.AddDynamic(this, &UUW_StatBar::OnCurrentValueUpdated);
	OwnerStatComponent->OnMaxStatValueUpdated.AddDynamic(this, &UUW_StatBar::OnMaxStatValueUpdated);

	ProgressBar_StatBar->SetPercent(0.0f);
	OwnerStatComponent->UpdateStat(StatBarType);

	LerpStaBarPercentAlpha = 0.1f;
}

void UUW_StatBar::OnCurrentValueUpdated(EStats StatUpdated, float CurrentValue, float MaxValue)
{
	//스텟바의 스텟타입과 스텟매니저의 스텟타입이 같다면
	if (StatUpdated == StatBarType)
	{
		UpdateStatBarPercent(CurrentValue / MaxValue); //스텟바위젯의 Percent를 업데이트한다

		FTimerHandle WaitHandle;
		const float WaitTime = 1.0f; //시간을 설정하고
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (DO_CVUpdated.Execute())
			{
				//LerpStatBar을 업데이트 해야 한다면
				if (bUpdateLerpStatBar)
				{
					UKismetSystemLibrary::K2_ClearTimer(this,TEXT("UpdateLerpBar"));
					UKismetSystemLibrary::K2_SetTimer(this,TEXT("UpdateLerpBar"), 0.1f, true);
				}
			}
		}), WaitTime, false);
	}
}

void UUW_StatBar::OnMaxStatValueUpdated(EStats StatUpdated, float Value)
{
	//업데이트 할 스텟타입과 위젯의 스텟타입이 같고 너비를 늘려야 한다면
	if (StatUpdated == StatBarType && bMaxStatUpdateWidth)
	{
		SizeBox_StatBar->SetWidthOverride(Value * StatBarSizeMultiplier);
		SizeBox_LerpStatBar->SetWidthOverride(Value * StatBarSizeMultiplier);
	}
}

void UUW_StatBar::UpdateStatBarPercent(float InPercent)
{
	StatPercent = InPercent;
	ProgressBar_StatBar->SetPercent(StatPercent);

	//현재 스텟바가 러브스텟바 퍼센트보다 크고 러브스텟바를 업데이트 해야한다면 업데이트한다
	if (StatPercent > ProgressBar_LerpStatBar->Percent && bUpdateLerpStatBar)
		ProgressBar_LerpStatBar->SetPercent(StatPercent);
}

void UUW_StatBar::UpdateLerpBar()
{
	//LerpStatBar의 Percent가 StatPercent와 같다면
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(ProgressBar_LerpStatBar->Percent, StatPercent, 0.01f))
	{
		//같으니까 UpdateStatLerpBar를 중단한다
		UKismetSystemLibrary::K2_ClearTimer(this,TEXT("UpdateLerpBar"));
		ProgressBar_LerpStatBar->SetPercent(StatPercent);
		ResetDoOnce();
	}
	else //LerpStatBar의 Percent가 StatPercent와 같지 않다면
	{
		//LerpStatbar의 Percent를 거의 LerpStatBarPecent로 맞춰준다
		const float L_LerpStatBarPercent = FMath::Lerp(ProgressBar_LerpStatBar->Percent, StatPercent, LerpStaBarPercentAlpha);
		ProgressBar_LerpStatBar->SetPercent(L_LerpStatBarPercent);
	}
}

void UUW_StatBar::ResetDoOnce()
{
	DO_CVUpdated.Reset();
}
