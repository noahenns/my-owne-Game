// TruckLogisticsSimulator - Core Game Types
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"

/**
 * Cargo type enumeration
 * Determines behavior, rewards, and constraints
 */
UENUM(BlueprintType)
enum class ECargoType : uint8
{
	Bulk = 0		UMETA(DisplayName = "Bulk Cargo"),
	Fragile = 1		UMETA(DisplayName = "Fragile Cargo"),
	Hazmat = 2		UMETA(DisplayName = "Hazmat (Time-Sensitive)")
};

/**
 * Cargo instance data
 * Represents a single cargo shipment
 */
USTRUCT(BlueprintType)
struct FCargoInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cargo")
	ECargoType CargoType = ECargoType::Bulk;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cargo")
	float Weight = 5.0f;  // kg

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cargo")
	float CurrentDamage = 0.0f;  // 0-100%

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cargo")
	bool bIsLoaded = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cargo")
	FDateTime DeliveryDeadline;  // For hazmat routes

	// Get cargo multiplier for rewards
	float GetRewardMultiplier() const
	{
		switch (CargoType)
		{
			case ECargoType::Fragile: return 1.2f;   // +20%
			case ECargoType::Hazmat: return 1.5f;    // +50%
			case ECargoType::Bulk:
			default: return 1.0f;
		}
	}
};

/**
 * Delivery route definition
 * Describes a single contract route
 */
USTRUCT(BlueprintType)
struct FDeliveryRoute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	FString RouteName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	FVector DestinationLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	float DistanceKilometers = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	float BaseReward = 500.0f;  // Currency

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	ECargoType RequiredCargoType = ECargoType::Bulk;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Route")
	TArray<FVector> Waypoints;

	// Calculate total reward for a delivery
	float CalculateReward(const FCargoInstance& Cargo, float FuelCost) const
	{
		const float DistanceBonus = DistanceKilometers * 10.0f;  // $10 per km
		const float CargoMultiplier = Cargo.GetRewardMultiplier();
		const float GrossPay = (BaseReward + DistanceBonus) * CargoMultiplier;
		const float NetPay = FMath::Max(0.0f, GrossPay - FuelCost);
		return NetPay;
	}
};

/**
 * Player progression state
 * Persisted across sessions
 */
USTRUCT(BlueprintType)
struct FPlayerProgression
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	float TotalMoney = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	int32 TotalXP = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	int32 CompletedDeliveries = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	bool bUnlocked3AxleTrailer = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	bool bUnlockedPremiumRoutes = false;

	int32 GetCurrentLevel() const
	{
		return 1 + (TotalXP / 100);  // Level up every 100 XP
	}

	bool CanUnlock3AxleTrailer() const
	{
		return GetCurrentLevel() >= 5 && !bUnlocked3AxleTrailer;
	}

	bool CanUnlockPremiumRoutes() const
	{
		return GetCurrentLevel() >= 10 && !bUnlockedPremiumRoutes;
	}
};

/**
 * Route progress tracking
 * Current state of active delivery
 */
USTRUCT(BlueprintType)
struct FRouteProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	FDeliveryRoute CurrentRoute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	FCargoInstance CurrentCargo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	float DistanceTraveled = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	int32 CurrentWaypointIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	bool bCargoLoaded = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
	bool bRouteActive = false;

	float GetCompletionPercentage() const
	{
		if (CurrentRoute.DistanceKilometers <= 0.0f)
			return 0.0f;
		return FMath::Clamp(DistanceTraveled / (CurrentRoute.DistanceKilometers * 1000.0f), 0.0f, 1.0f);
	}

	float GetRemainingDistance() const
	{
		return FMath::Max(0.0f, (CurrentRoute.DistanceKilometers * 1000.0f) - DistanceTraveled);
	}
};
