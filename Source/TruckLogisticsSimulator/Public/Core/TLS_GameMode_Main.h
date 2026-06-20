// TruckLogisticsSimulator - Main Game Mode
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/GameTypes.h"
#include "TLS_GameMode_Main.generated.h"

class ATruckPawn;
class ARouteManager;
class ATLS_GameState;
class ATLS_PlayerState;
class ATLS_PlayerController;

/**
 * Main game mode for truck logistics simulator
 * Handles multiplayer sessions and player progression
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATLS_GameMode_Main : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATLS_GameMode_Main();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Multiplayer callbacks
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// Get game state
	UFUNCTION(BlueprintCallable, Category = "Game")
	ATLS_GameState* GetGameState_Custom() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	ATLS_GameState* GameStateRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	ARouteManager* RouteManagerInstance = nullptr;

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void UpdateConnectedPlayers();

private:
	FTimerHandle _UpdateTimerHandle;
};
