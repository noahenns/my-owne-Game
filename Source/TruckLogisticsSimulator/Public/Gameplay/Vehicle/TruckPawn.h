// TruckLogisticsSimulator - Truck Pawn (Player Vehicle)
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "TruckPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UVehiclePhysicsComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * Player-controlled truck vehicle
 * Handles input, camera, physics, and cargo
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATruckPawn : public APawn
{
	GENERATED_BODY()

public:
	ATruckPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// VEHICLE CONTROL
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void AccelerateTruck(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void BrakeTruck(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void SteerTruck(float Value);

	// QUERY
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetCurrentSpeedKMH() const;

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	FVector GetTruckPosition() const { return GetActorLocation(); }

	// CARGO
	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool HasCargo() const;

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	float GetFuelPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	float GetCargoWeight() const;

	// COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* TruckMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UVehiclePhysicsComponent* PhysicsComponent;

	// VEHICLE PROPERTIES (Tunable)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Fuel")
	float FuelCapacity = 100.0f;  // Liters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Fuel")
	float FuelConsumptionRate = 0.025f;  // L per meter

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Cargo")
	float MaxCargoWeight = 5000.0f;  // kg (2-axle trailer)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Camera")
	float CameraBoomLength = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Camera")
	float CameraBoomHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Camera")
	float CameraFOV = 90.0f;

protected:
	// Fuel management
	float CurrentFuel;
	void ConsumeFuel(float DeltaTime);

	// Input callbacks (will be bound in SetupPlayerInputComponent)
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	// Enhanced Input System (Unreal 5+)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UEnhancedInputComponent* EnhancedInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UEnhancedInputSubsystem* InputSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultInputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	// Multiplayer replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	float _LastFuelConsumption = 0.0f;
};
