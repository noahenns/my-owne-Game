// TruckLogisticsSimulator - Vehicle Physics Component
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehiclePhysicsComponent.generated.h"

/**
 * Vehicle physics simulation
 * Handles speed, acceleration, braking, steering, and collision
 */
UCLASS(ClassGroup = (Vehicle), meta = (BlueprintSpawnableComponent))
class TRUCKLOGISTICSSIMULATOR_API UVehiclePhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVehiclePhysicsComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// INPUT INTERFACE
	void SetThrottleInput(float Value);
	void SetBrakeInput(float Value);
	void SetSteeringInput(float Value);

	// QUERY INTERFACE
	UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
	float GetCurrentSpeed() const { return CurrentSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
	float GetCurrentRotation() const { return CurrentYaw; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
	FVector GetVelocityDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
	bool IsMoving() const { return CurrentSpeed > 0.1f; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle|Physics")
	float GetSpeedKMH() const { return CurrentSpeed * 3.6f; }  // Convert m/s to km/h

	// PHYSICS PARAMETERS (Tweakable)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Speed")
	float MaxSpeed = 30.0f;  // m/s (108 km/h)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Speed")
	float MaxReverseSpeed = 10.0f;  // m/s (36 km/h)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Acceleration")
	float AccelerationRate = 15.0f;  // m/s²

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Acceleration")
	float BrakingDeceleration = 20.0f;  // m/s²

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Acceleration")
	float NaturalDrag = 1.0f;  // Natural speed loss

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Steering")
	float MaxTurnRate = 2.5f;  // Radians per second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Steering")
	float SteeringDamping = 0.95f;  // How quickly steering returns to center

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Traction")
	float BaseTraction = 0.95f;  // Base tire grip (0-1)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Physics|Traction")
	float WetSurfaceTraction = 0.70f;  // Traction on wet surfaces

protected:
	// Current input state
	float _ThrottleInput = 0.0f;
	float _BrakeInput = 0.0f;
	float _SteeringInput = 0.0f;

	// Current physics state
	float CurrentSpeed = 0.0f;  // m/s
	float CurrentYaw = 0.0f;    // Rotation angle in radians

	// Update functions
	void UpdateSpeed(float DeltaTime);
	void UpdateRotation(float DeltaTime);
	void UpdatePosition(float DeltaTime);

	// Utility functions
	float CalculateTraction(float WetnessAmount = 0.0f) const;
	void PerformGroundTrace();

private:
	FVector _CurrentLocation = FVector::ZeroVector;
	bool _bIsOnGround = true;
};
