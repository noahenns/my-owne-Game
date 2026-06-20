// TruckLogisticsSimulator - Multiplayer Player Controller
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TLS_PlayerController.generated.h"

class ATruckPawn;
class ATLS_PlayerState;

/**
 * Player controller for truck logistics simulator
 * Handles input, camera, and multiplayer sync
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATLS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATLS_PlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	// Get controlled truck
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	ATruckPawn* GetControlledTruck() const { return ControlledTruck; }

	// Get player state
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	ATLS_PlayerState* GetPlayerStateMultiplayer() const;

	// Server RPC to accept route
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Routes")
	void Server_AcceptRoute(int32 RouteIndex);

	// Server RPC for delivering cargo
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Cargo")
	void Server_DeliverCargo();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
	ATruckPawn* ControlledTruck = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	ATLS_PlayerState* PlayerStateRef = nullptr;

	// Input callbacks
	void Input_Accelerate(float Value);
	void Input_Brake(float Value);
	void Input_Steer(float Value);
	void Input_Pause();

private:
	bool bInputEnabled = true;
};
