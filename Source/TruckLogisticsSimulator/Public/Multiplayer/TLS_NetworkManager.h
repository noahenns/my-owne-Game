// TruckLogisticsSimulator - Network Manager
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TLS_NetworkManager.generated.h"

/**
 * Handles LAN multiplayer hosting, joining, and session management
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API UTLS_NetworkManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// LAN SERVER HOSTING
	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Hosting")
	void StartLANServer(FString SessionName = TEXT("TruckSimServer"), int32 MaxPlayers = 4);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Hosting")
	void StopServer();

	// LAN CLIENT JOINING
	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Hosting")
	void FindLANSessions();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Hosting")
	void JoinLANSession(int32 SessionIndex);

	// DIRECT IP JOIN (for testing)
	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Hosting")
	void ConnectToIP(FString IPAddress, int32 Port = 7777);

	// STATE QUERIES
	UFUNCTION(BlueprintCallable, Category = "Multiplayer|State")
	bool IsHosting() const { return bIsHosting; }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|State")
	bool IsConnected() const { return bIsConnected; }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|State")
	FString GetSessionState() const { return SessionState; }

	// Events (multicast to UI)
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSessionCreated, bool, FString);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSessionFound, int32, FString);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSessionJoined, bool, FString);

	FOnSessionCreated OnSessionCreated;
	FOnSessionFound OnSessionFound;
	FOnSessionJoined OnSessionJoined;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer|State")
	bool bIsHosting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer|State")
	bool bIsConnected = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer|State")
	FString SessionState = TEXT("Offline");

	// Callbacks
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	class IOnlineSubsystem* OnlineSubsystem = nullptr;
	class IOnlineSessionPtr SessionInterface = nullptr;
	TArray<FOnlineSessionSearchResult> SearchResults;
};
