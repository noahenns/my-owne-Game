// TruckLogisticsSimulator - Cargo System
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/GameTypes.h"
#include "CargoSystem.generated.h"

/**
 * Manages cargo loading, unloading, and damage
 */
UCLASS(ClassGroup = (Cargo), meta = (BlueprintSpawnableComponent))
class TRUCKLOGISTICSSIMULATOR_API UCargoSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UCargoSystem();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// CARGO STATE
	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool HasCargo() const { return CurrentCargo.bIsLoaded; }

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	float GetCargoWeight() const { return HasCargo() ? CurrentCargo.Weight : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	float GetCargoDamage() const { return CurrentCargo.CurrentDamage; }

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	FCargoInstance GetCurrentCargo() const { return CurrentCargo; }

	// CARGO OPERATIONS
	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool TryLoadCargo(const FCargoInstance& InCargo);

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool TryUnloadCargo(float& OutMoneyEarned, float& OutXPEarned);

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	void ApplyCollisionDamage(float ImpactForce);

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool ValidateDeliveryLocation(FVector DeliveryPoint, float ProximityThreshold = 1000.0f);

	// PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Settings")
	float MaxCargoWeight = 5000.0f;  // kg

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Settings")
	float FragileDamageMultiplier = 1.5f;  // Fragile cargo takes more damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Settings")
	float LoadingTime = 2.0f;  // Seconds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Settings")
	float HazmatTimeMultiplier = 2.0f;  // Hazmat rewards double XP/money for time pressure

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo|State")
	FCargoInstance CurrentCargo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo|State")
	bool bIsLoading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo|State")
	float LoadingTimer = 0.0f;

	// Internal methods
	void UpdateLoadingState(float DeltaTime);

	// Cached delivery location for validation
	FVector _LastDeliveryLocation = FVector::ZeroVector;
};
