// TruckLogisticsSimulator - Route Manager
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/GameTypes.h"
#include "RouteManager.generated.h"

/**
 * Manages all delivery routes and active route state
 * Singleton actor spawned in level
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ARouteManager : public AActor
{
	GENERATED_BODY()

public:
	ARouteManager();

	virtual void BeginPlay() override;

	// ROUTE QUERY
	UFUNCTION(BlueprintCallable, Category = "Routes")
	FDeliveryRoute GetRouteByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Routes")
	int32 GetRouteCount() const { return AvailableRoutes.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Routes")
	bool HasActiveRoute() const { return CurrentRouteProgress.bRouteActive; }

	UFUNCTION(BlueprintCallable, Category = "Routes")
	FRouteProgress GetCurrentRouteProgress() const { return CurrentRouteProgress; }

	UFUNCTION(BlueprintCallable, Category = "Routes")
	FVector GetNextWaypoint() const;

	UFUNCTION(BlueprintCallable, Category = "Routes")
	float GetDistanceToNextWaypoint(FVector FromLocation) const;

	// ROUTE MANAGEMENT
	UFUNCTION(BlueprintCallable, Category = "Routes")
	bool AcceptRoute(int32 RouteIndex);

	UFUNCTION(BlueprintCallable, Category = "Routes")
	void UpdateRouteProgress(float DistanceTraveled, FVector CurrentLocation);

	UFUNCTION(BlueprintCallable, Category = "Routes")
	bool IsPlayerNearDeliveryPoint(FVector PlayerLocation, float ProximityRadius = 1000.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Routes")
	void CompleteRoute();

	UFUNCTION(BlueprintCallable, Category = "Routes")
	void CancelRoute();

	// SINGLETON PATTERN
	static ARouteManager* GetInstance(AWorld* World = nullptr);

protected:
	virtual void BeginDestroy() override;

	// Available routes (hardcoded for MVP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Routes")
	TArray<FDeliveryRoute> AvailableRoutes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Routes")
	FRouteProgress CurrentRouteProgress;

	// Initialize default routes for MVP
	void InitializeDefaultRoutes();

	// Singleton instance
	static ARouteManager* _Instance;
};
