// TruckLogisticsSimulator - Route Manager Implementation
// Copyright 2026. All rights reserved.

#include "Gameplay/Route/RouteManager.h"
#include "Kismet/GameplayStatics.h"

ARouteManager* ARouteManager::_Instance = nullptr;

ARouteManager::ARouteManager()
{
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = false;
}

void ARouteManager::BeginPlay()
{
	Super::BeginPlay();

	// Singleton setup
	if (_Instance != nullptr && _Instance != this)
	{
		Destroy();
		return;
	}
	_Instance = this;

	// Initialize routes
	InitializeDefaultRoutes();

	UE_LOG(LogTemp, Warning, TEXT("RouteManager initialized with %d routes"), AvailableRoutes.Num());
}

void ARouteManager::BeginDestroy()
{
	if (_Instance == this)
	{
		_Instance = nullptr;
	}
	Super::BeginDestroy();
}

ARouteManager* ARouteManager::GetInstance(AWorld* World)
{
	if (_Instance == nullptr && World != nullptr)
	{
		_Instance = World->SpawnActor<ARouteManager>();
	}
	return _Instance;
}

void ARouteManager::InitializeDefaultRoutes()
{
	AvailableRoutes.Empty();

	// ROUTE 1: Urban Depot to Port
	{
		FDeliveryRoute Route;
		Route.RouteName = TEXT("Urban Depot → Port");
		Route.StartLocation = FVector(5000, 5000, 0);
		Route.DestinationLocation = FVector(15000, 5000, 0);
		Route.DistanceKilometers = 10.0f;
		Route.BaseReward = 450.0f;
		Route.RequiredCargoType = ECargoType::Bulk;
		
		// Add waypoints along the route
		for (int i = 0; i < 5; ++i)
		{
			float Alpha = static_cast<float>(i) / 4.0f;
			FVector Waypoint = FMath::Lerp(Route.StartLocation, Route.DestinationLocation, Alpha);
			Route.Waypoints.Add(Waypoint);
		}

		AvailableRoutes.Add(Route);
	}

	// ROUTE 2: Warehouse to Construction Site
	{
		FDeliveryRoute Route;
		Route.RouteName = TEXT("Warehouse → Construction");
		Route.StartLocation = FVector(5000, 15000, 0);
		Route.DestinationLocation = FVector(20000, 20000, 0);
		Route.DistanceKilometers = 18.0f;
		Route.BaseReward = 650.0f;
		Route.RequiredCargoType = ECargoType::Fragile;
		
		// Add waypoints
		TArray<FVector> WayPointList = {
			FVector(5000, 15000, 0),
			FVector(8000, 16000, 0),
			FVector(12000, 18000, 0),
			FVector(16000, 19000, 0),
			FVector(20000, 20000, 0)
		};
		Route.Waypoints = WayPointList;

		AvailableRoutes.Add(Route);
	}

	// ROUTE 3: Harbor to Factory (Time-Sensitive Hazmat)
	{
		FDeliveryRoute Route;
		Route.RouteName = TEXT("Harbor → Factory (HAZMAT)");
		Route.StartLocation = FVector(2000, 8000, 0);
		Route.DestinationLocation = FVector(18000, 12000, 0);
		Route.DistanceKilometers = 25.0f;
		Route.BaseReward = 850.0f;
		Route.RequiredCargoType = ECargoType::Hazmat;
		
		// Add waypoints
		TArray<FVector> WayPointList = {
			FVector(2000, 8000, 0),
			FVector(6000, 8500, 0),
			FVector(10000, 10000, 0),
			FVector(14000, 11000, 0),
			FVector(18000, 12000, 0)
		};
		Route.Waypoints = WayPointList;

		AvailableRoutes.Add(Route);
	}
}

FDeliveryRoute ARouteManager::GetRouteByIndex(int32 Index) const
{
	if (AvailableRoutes.IsValidIndex(Index))
	{
		return AvailableRoutes[Index];
	}

	UE_LOG(LogTemp, Warning, TEXT("Invalid route index: %d"), Index);
	return FDeliveryRoute();
}

FVector ARouteManager::GetNextWaypoint() const
{
	if (!CurrentRouteProgress.bRouteActive || CurrentRouteProgress.CurrentRoute.Waypoints.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	int32 WaypointIndex = FMath::Min(
		CurrentRouteProgress.CurrentWaypointIndex,
		CurrentRouteProgress.CurrentRoute.Waypoints.Num() - 1
	);

	return CurrentRouteProgress.CurrentRoute.Waypoints[WaypointIndex];
}

float ARouteManager::GetDistanceToNextWaypoint(FVector FromLocation) const
{
	FVector NextWaypoint = GetNextWaypoint();
	if (NextWaypoint.IsZero())
	{
		return 0.0f;
	}

	return FVector::Dist(FromLocation, NextWaypoint);
}

bool ARouteManager::AcceptRoute(int32 RouteIndex)
{
	if (!AvailableRoutes.IsValidIndex(RouteIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot accept invalid route index: %d"), RouteIndex);
		return false;
	}

	// Cancel current route if one exists
	if (CurrentRouteProgress.bRouteActive)
	{
		CancelRoute();
	}

	// Set up new route
	CurrentRouteProgress.CurrentRoute = AvailableRoutes[RouteIndex];
	CurrentRouteProgress.bRouteActive = true;
	CurrentRouteProgress.CurrentWaypointIndex = 0;
	CurrentRouteProgress.DistanceTraveled = 0.0f;
	CurrentRouteProgress.bCargoLoaded = false;

	// Create cargo instance for this route
	CurrentRouteProgress.CurrentCargo.CargoType = AvailableRoutes[RouteIndex].RequiredCargoType;
	CurrentRouteProgress.CurrentCargo.Weight = 2000.0f;  // Default weight
	CurrentRouteProgress.CurrentCargo.bIsLoaded = false;

	UE_LOG(LogTemp, Warning, TEXT("Route accepted: %s"), *CurrentRouteProgress.CurrentRoute.RouteName);
	return true;
}

void ARouteManager::UpdateRouteProgress(float DistanceTraveled, FVector CurrentLocation)
{
	if (!CurrentRouteProgress.bRouteActive)
	{
		return;
	}

	CurrentRouteProgress.DistanceTraveled += DistanceTraveled;

	// Check if we reached next waypoint
	FVector NextWaypoint = GetNextWaypoint();
	float DistanceToWaypoint = FVector::Dist(CurrentLocation, NextWaypoint);

	const float WAYPOINT_PROXIMITY = 500.0f;  // 500cm = 5m

	if (DistanceToWaypoint < WAYPOINT_PROXIMITY)
	{
		CurrentRouteProgress.CurrentWaypointIndex++;

		// Check if route is complete
		if (CurrentRouteProgress.CurrentWaypointIndex >= CurrentRouteProgress.CurrentRoute.Waypoints.Num())
		{
			// Route complete!
			UE_LOG(LogTemp, Warning, TEXT("Route waypoint completed!"));
		}
	}
}

bool ARouteManager::IsPlayerNearDeliveryPoint(FVector PlayerLocation, float ProximityRadius) const
{
	if (!CurrentRouteProgress.bRouteActive)
	{
		return false;
	}

	float DistanceToDestination = FVector::Dist(PlayerLocation, CurrentRouteProgress.CurrentRoute.DestinationLocation);
	return DistanceToDestination < ProximityRadius;
}

void ARouteManager::CompleteRoute()
{
	if (!CurrentRouteProgress.bRouteActive)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Route completed: %s"), *CurrentRouteProgress.CurrentRoute.RouteName);

	CurrentRouteProgress.bRouteActive = false;
	CurrentRouteProgress.bCargoLoaded = false;
}

void ARouteManager::CancelRoute()
{
	if (CurrentRouteProgress.bRouteActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Route cancelled: %s"), *CurrentRouteProgress.CurrentRoute.RouteName);
	}

	CurrentRouteProgress = FRouteProgress();
}
