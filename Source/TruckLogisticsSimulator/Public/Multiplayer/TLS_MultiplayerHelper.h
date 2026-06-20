// TruckLogisticsSimulator - Multiplayer Menu Helper
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "TLS_MultiplayerHelper.generated.h"

class UTLS_NetworkManager;

/**
 * Quick multiplayer helper for testing
 * Usage: In console: ShowMultiplayerMenu
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API UTLS_MultiplayerHelper : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Console commands
	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void StartServer(int32 MaxPlayers = 4);

	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void StopServer();

	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void FindServers();

	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void JoinServer(int32 ServerIndex = 0);

	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void ConnectIP(FString IPAddress);

	UFUNCTION(Exec, BlueprintCallable, Category = "Multiplayer")
	void PrintNetworkStatus();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	UTLS_NetworkManager* NetworkManager = nullptr;
};
