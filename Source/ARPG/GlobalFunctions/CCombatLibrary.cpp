#include "CCombatLibrary.h"

#include "Kismet/KismetMathLibrary.h"


float CCombatLibrary::GetMultiplierForPercentage(int32 InPercentage)
{
	return InPercentage / 100.0f;
}



