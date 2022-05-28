

#include "CBaseShield.h"

ACBaseShield::ACBaseShield()
{
}

void ACBaseShield::BeginPlay()
{
	Super::BeginPlay();
}

void ACBaseShield::OnItemEquipped()
{
	SetItemEquipped(true);
}

void ACBaseShield::OnItemUnEquipped()
{
	SetItemEquipped(false);
}
