#pragma once

#include "GlobalFunctions/CRPGLibrary.h"
#include "GlobalFunctions/CCombatLibrary.h"
#include "GlobalFunctions/CInventoryEquipmentLibrary.h"
#include "GlobalFunctions/CHelpers.h"
#include "GlobalFunctions/Gate.h"
#include "GlobalFunctions/DoOnce.h"
#include "GlobalFunctions/CLog.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#define ECC_Interactable ECC_GameTraceChannel1

#define TAG_ENEMY FName(TEXT("Enemy"))
#define TAG_ENEMYAI FName(TEXT("EnemyAI"))
#define TAG_MASTERAI FName(TEXT("MasterAI"))
#define TAG_PLAYERCHARACTER FName(TEXT("PlayerCharacter"))
#define TAG_PAWNCAPSULE FName(TEXT("Pawn Capsule"))
#define TAG_LOCKONSPHERE FName(TEXT("LockOnSphere"))
#define TAG_AICOMPONENT FName(TEXT("AI Component"))
#define TAG_IGNORE FName(TEXT("Ignore"))


#define COLOR_MAGENTA FLinearColor(0.25f,0.0f,0.25f,1.0f)
#define COLOR_PURPLE FLinearColor(1.0f,0.0f,1.0f,1.0f)
#define COLOR_YELLOW FLinearColor(1.0f,0.5f,0.0f,1.0f)
#define COLOR_BROWN FLinearColor(0.25f,0.125f,0.0f,1.0f)
#define COLOR_SKY FLinearColor(0.0f,0.5f,1.0f,1.0f)
#define COLOR_ORANGE FLinearColor(0.9f, 0.4f, 0.0f, 1.0f)
#define COLOR_PINK FLinearColor(0.9f, 0.3f, 1.0f, 1.0f)


//물리 피직스(서피스) 타입
#define PhysicalSurface_Metal SurfaceType1
#define PhysicalSurface_Cobblestone SurfaceType2
#define PhysicalSurface_Water SurfaceType3
#define PhysicalSurface_Flesh SurfaceType4
#define PhysicalSurface_Wood SurfaceType5
#define PhysicalSurface_Concrete SurfaceType6
#define PhysicalSurface_Dirt SurfaceType7
#define PhysicalSurface_Rock SurfaceType8



#define NOTFIND_STATVALUE 0.0f