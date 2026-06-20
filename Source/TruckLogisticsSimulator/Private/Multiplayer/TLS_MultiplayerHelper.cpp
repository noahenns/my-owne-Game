// TruckLogisticsSimulator - Multiplayer Helper Implementation
// Copyright 2026. All rights reserved.

#include "Multiplayer/TLS_MultiplayerHelper.h"
#include "Multiplayer/TLS_NetworkManager.h"
#include "Engine/World.h"

void UTLS_MultiplayerHelper::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Get network manager from world
	if (UWorld* World = GetWorld())
	{
		NetworkManager = World->GetSubsystem<UTLS_NetworkManager>();
		if (NetworkManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiplayer Helper initialized"));
		}
	}
}

void UTLS_MultiplayerHelper::StartServer(int32 MaxPlayers)
{
	if (!NetworkManager)
	{
		UE_LOG(LogTemp, Error, TEXT("No network manager!"));
		return;
	}

	FString ServerName = FString::Printf(TEXT("TruckSimServer_%d"), FMath::Rand());
	NetworkManager->StartLANServer(ServerName, MaxPlayers);

	UE_LOG(LogTemp, Warning, TEXT("Starting server: %s (Max %d players)"), *ServerName, MaxPlayers);
}

void UTLS_MultiplayerHelper::StopServer()
{
	if (!NetworkManager)
		return;

	NetworkManager->StopServer();
	UE_LOG(LogTemp, Warning, TEXT("Server stopped"));
}

void UTLS_MultiplayerHelper::FindServers()
{
	if (!NetworkManager)
	{
		UE_LOG(LogTemp, Error, TEXT("No network manager!"));
		return;
	}

	NetworkManager->FindLANSessions();
	UE_LOG(LogTemp, Warning, TEXT("Searching for LAN servers..."));
}

void UTLS_MultiplayerHelper::JoinServer(int32 ServerIndex)
{
	if (!NetworkManager)
	{
		UE_LOG(LogTemp, Error, TEXT("No network manager!"));
		return;
	}

	NetworkManager->JoinLANSession(ServerIndex);
	UE_LOG(LogTemp, Warning, TEXT("Joining server %d..."), ServerIndex);
}

void UTLS_MultiplayerHelper::ConnectIP(FString IPAddress)
{
	if (!NetworkManager)
	{
		UE_LOG(LogTemp, Error, TEXT("No network manager!"));
		return;
	}

	if (IPAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: ConnectIP <IP_ADDRESS>"));
		return;
	}

	NetworkManager->ConnectToIP(IPAddress);
	UE_LOG(LogTemp, Warning, TEXT("Connecting to %s..."), *IPAddress);
}

void UTLS_MultiplayerHelper::PrintNetworkStatus()
{
	if (!NetworkManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No network manager"));
		return;
	}

	bool bHosting = NetworkManager->IsHosting();
	bool bConnected = NetworkManager->IsConnected();
	FString Status = NetworkManager->GetSessionState();

	UE_LOG(LogTemp, Warning, TEXT("=== NETWORK STATUS ==="));
	UE_LOG(LogTemp, Warning, TEXT("Hosting: %s"), bHosting ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogTemp, Warning, TEXT("Connected: %s"), bConnected ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogTemp, Warning, TEXT("Status: %s"), *Status);
	UE_LOG(LogTemp, Warning, TEXT("======================"));
}
