// TruckLogisticsSimulator - Multiplayer Game State
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Core/GameTypes.h"
#include "TLS_GameState.generated.h"

class ARouteManager;

/**
 * Shared game state for all players in multiplayer session
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATLS_GameState : public AGameState
{
	GENERATED_BODY()

public:
	ATLS_GameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	// SHARED ROUTE DATA
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Routes")
	int32 TotalPlayersInGame = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Routes")
	float ElapsedGameTime = 0.0f;

	// Access route manager (replicated)
	UFUNCTION(BlueprintCallable, Category = "Routes")
	ARouteManager* GetRouteManager() const { return _RouteManager; }

	// Multiplayer-specific queries
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	int32 GetConnectedPlayerCount() const { return PlayerArray.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool IsListenServer() const { return bIsListenServer; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Routes")
	ARouteManager* _RouteManager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	bool bIsListenServer = false;

	virtual void Tick(float DeltaSeconds) override;
};
