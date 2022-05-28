
#pragma once

#include "CoreMinimal.h"
#include "DoOnce.generated.h"


USTRUCT(BlueprintType)
struct FDoOnce
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bDonOnce;

	FORCEINLINE FDoOnce();
	explicit FORCEINLINE FDoOnce(bool bStartClosed);


	FORCEINLINE void Reset() { bDonOnce = true; }

	FORCEINLINE bool Execute()
	{
		if (bDonOnce)
		{
			bDonOnce = false;
			return true;
		}
		return false;
	}
};


FORCEINLINE FDoOnce::FDoOnce() : bDonOnce(true)
{
}

FORCEINLINE FDoOnce::FDoOnce(const bool bStartClosed) : bDonOnce(!bStartClosed)
{
}
