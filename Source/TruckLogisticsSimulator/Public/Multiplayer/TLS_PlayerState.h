// TruckLogisticsSimulator - Multiplayer Player State
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Core/GameTypes.h"
#include "TLS_PlayerState.generated.h"

/**
 * Tracks player-specific progression in multiplayer
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATLS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATLS_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// PROGRESSION
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Progression")
	FPlayerProgression PlayerProgression;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
	int32 CurrentRouteIndex = -1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Progression")
	FRouteProgress CurrentRoute;

	// UI
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	FLinearColor PlayerColor = FLinearColor::White;

	// Setters
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddMoney(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddXP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void SetCurrentRoute(int32 RouteIndex);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor)
	FLinearColor ReplicatedPlayerColor;

	UFUNCTION()
	void OnRep_PlayerColor();

	// Assign unique color per player
	void AssignPlayerColor();
};
