// TruckLogisticsSimulator - Cargo System Implementation
// Copyright 2026. All rights reserved.

#include "Gameplay/Cargo/CargoSystem.h"
#include "Gameplay/Route/RouteManager.h"

UCargoSystem::UCargoSystem()
{
	PrimaryComponentTick.TickInterval = 0.016f;
	bTickInEditor = false;
}

void UCargoSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UCargoSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLoadingState(DeltaTime);
}

bool UCargoSystem::TryLoadCargo(const FCargoInstance& InCargo)
{
	// Check weight
	if (InCargo.Weight > MaxCargoWeight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot load cargo: exceeds max weight (%.1f / %.1f kg)"),
			InCargo.Weight, MaxCargoWeight);
		return false;
	}

	// Check if already have cargo
	if (CurrentCargo.bIsLoaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already have cargo loaded"));
		return false;
	}

	// Start loading
	CurrentCargo = InCargo;
	bIsLoading = true;
	LoadingTimer = LoadingTime;

	UE_LOG(LogTemp, Warning, TEXT("Started loading cargo: %.1f kg"), InCargo.Weight);
	return true;
}

bool UCargoSystem::TryUnloadCargo(float& OutMoneyEarned, float& OutXPEarned)
{
	OutMoneyEarned = 0.0f;
	OutXPEarned = 0.0f;

	if (!CurrentCargo.bIsLoaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("No cargo to unload"));
		return false;
	}

	// Get route manager for reward calculation
	ARouteManager* RouteManager = ARouteManager::GetInstance();
	if (!RouteManager || !RouteManager->HasActiveRoute())
	{
		UE_LOG(LogTemp, Warning, TEXT("No active route for unload"));
		return false;
	}

	// Calculate rewards
	FRouteProgress RouteProgress = RouteManager->GetCurrentRouteProgress();
	float FuelCost = RouteProgress.DistanceTraveled * 0.0025f;  // $2.50 per km
	OutMoneyEarned = RouteProgress.CurrentRoute.CalculateReward(CurrentCargo, FuelCost);

	// XP based on distance + cargo type
	OutXPEarned = static_cast<int32>(RouteProgress.DistanceTraveled / 1000.0f);  // 1 XP per km
	OutXPEarned += 50;  // Delivery bonus

	if (CurrentCargo.CargoType == ECargoType::Hazmat)
	{
		OutXPEarned = static_cast<int32>(OutXPEarned * HazmatTimeMultiplier);
		OutMoneyEarned *= HazmatTimeMultiplier;
	}

	// Apply damage penalty
	float DamagePenalty = (CurrentCargo.CurrentDamage / 100.0f) * OutMoneyEarned * 0.5f;
	OutMoneyEarned = FMath::Max(0.0f, OutMoneyEarned - DamagePenalty);

	UE_LOG(LogTemp, Warning, TEXT("Cargo unloaded! Money: %.2f, XP: %d (Damage: %.1f%%)"),
		OutMoneyEarned, static_cast<int32>(OutXPEarned), CurrentCargo.CurrentDamage);

	// Clear cargo
	CurrentCargo = FCargoInstance();
	bIsLoading = false;

	return true;
}

void UCargoSystem::ApplyCollisionDamage(float ImpactForce)
{
	if (!CurrentCargo.bIsLoaded)
		return;

	// Calculate damage based on impact force and cargo type
	float DamageAmount = ImpactForce * 0.1f;  // Arbitrary scaling

	if (CurrentCargo.CargoType == ECargoType::Fragile)
	{
		DamageAmount *= FragileDamageMultiplier;
	}
	else if (CurrentCargo.CargoType == ECargoType::Hazmat)
	{
		// Hazmat doesn't take damage, but we could trigger other effects
		return;
	}

	CurrentCargo.CurrentDamage = FMath::Clamp(
		CurrentCargo.CurrentDamage + DamageAmount,
		0.0f,
		100.0f
	);

	if (CurrentCargo.CurrentDamage > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargo damaged! Current damage: %.1f%%"), CurrentCargo.CurrentDamage);
	}
}

bool UCargoSystem::ValidateDeliveryLocation(FVector DeliveryPoint, float ProximityThreshold)
{
	if (!CurrentCargo.bIsLoaded)
	{
		return false;
	}

	ARouteManager* RouteManager = ARouteManager::GetInstance();
	if (!RouteManager || !RouteManager->HasActiveRoute())
	{
		return false;
	}

	return RouteManager->IsPlayerNearDeliveryPoint(DeliveryPoint, ProximityThreshold);
}

void UCargoSystem::UpdateLoadingState(float DeltaTime)
{
	if (!bIsLoading)
		return;

	LoadingTimer -= DeltaTime;

	if (LoadingTimer <= 0.0f)
	{
		// Finish loading
		CurrentCargo.bIsLoaded = true;
		bIsLoading = false;
		LoadingTimer = 0.0f;

		UE_LOG(LogTemp, Warning, TEXT("Cargo loaded successfully!"));
	}
}
