// TruckLogisticsSimulator - Network Manager Implementation
// Copyright 2026. All rights reserved.

#include "Multiplayer/TLS_NetworkManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UTLS_NetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		UE_LOG(LogTemp, Warning, TEXT("Network Manager initialized with subsystem: %s"), 
			*OnlineSubsystem->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get online subsystem!"));
	}
}

void UTLS_NetworkManager::Deinitialize()
{
	if (bIsHosting)
	{
		StopServer();
	}

	Super::Deinitialize();
}

void UTLS_NetworkManager::StartLANServer(FString SessionName, int32 MaxPlayers)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No session interface available!"));
		OnSessionCreated.Broadcast(false, TEXT("No session interface"));
		return;
	}

	// Destroy any existing session first
	if (SessionInterface->GetNamedSession(FName(*SessionName)))
	{
		SessionInterface->DestroySession(FName(*SessionName));
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bShouldAdvertise = true;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	CreateSessionCompleteDelegate.BindUObject(this, &UTLS_NetworkManager::OnCreateSessionComplete);

	if (SessionInterface->CreateSession(0, FName(*SessionName), SessionSettings))
	{
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
		SessionState = TEXT("Creating Server...");
		UE_LOG(LogTemp, Warning, TEXT("Creating LAN server: %s (Max players: %d)"), *SessionName, MaxPlayers);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session!"));
		OnSessionCreated.Broadcast(false, TEXT("Failed to create session"));
	}
}

void UTLS_NetworkManager::StopServer()
{
	if (!SessionInterface.IsValid())
		return;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	DestroySessionCompleteDelegate.BindUObject(this, &UTLS_NetworkManager::OnDestroySessionComplete);
	SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	SessionInterface->DestroySession(FName(TEXT("TruckSimServer")));
	bIsHosting = false;
	SessionState = TEXT("Offline");
	UE_LOG(LogTemp, Warning, TEXT("Server stopped"));
}

void UTLS_NetworkManager::FindLANSessions()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No session interface!"));
		return;
	}

	SessionState = TEXT("Searching for servers...");

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FindSessionsCompleteDelegate.BindUObject(this, &UTLS_NetworkManager::OnFindSessionsComplete);
	SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	TSharedPtr<FOnlineSessionSearch> SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;

	if (SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Found %d LAN sessions"), SearchResults.Num());

		for (int32 i = 0; i < SearchResults.Num(); ++i)
		{
			FString ServerName = SearchResults[i].Session.OwningUserName;
			OnSessionFound.Broadcast(i, ServerName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No sessions found"));
	}
}

void UTLS_NetworkManager::JoinLANSession(int32 SessionIndex)
{
	if (SessionIndex < 0 || SessionIndex >= SearchResults.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid session index: %d"), SessionIndex);
		OnSessionJoined.Broadcast(false, TEXT("Invalid session"));
		return;
	}

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No session interface!"));
		return;
	}

	SessionState = TEXT("Joining...");

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	JoinSessionCompleteDelegate.BindUObject(this, &UTLS_NetworkManager::OnJoinSessionComplete);
	SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	FString ServerName = SearchResults[SessionIndex].Session.OwningUserName;
	if (SessionInterface->JoinSession(0, FName(*ServerName), SearchResults[SessionIndex]))
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining session: %s"), *ServerName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to join session"));
		OnSessionJoined.Broadcast(false, TEXT("Failed to join"));
	}
}

void UTLS_NetworkManager::ConnectToIP(FString IPAddress, int32 Port)
{
	if (!GetWorld())
		return;

	FString URL = FString::Printf(TEXT("%s:%d"), *IPAddress, Port);
	GetWorld()->ServerTravel(URL);

	SessionState = TEXT("Connecting to IP...");
	UE_LOG(LogTemp, Warning, TEXT("Connecting to: %s"), *URL);
}

void UTLS_NetworkManager::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		bIsHosting = true;
		SessionState = TEXT("Server Running");
		UE_LOG(LogTemp, Warning, TEXT("Session created successfully: %s"), *SessionName.ToString());

		// Start the server
		if (GetWorld())
		{
			GetWorld()->ServerTravel(TEXT("?listen"));
		}

		OnSessionCreated.Broadcast(true, TEXT("Server created"));
	}
	else
	{
		SessionState = TEXT("Failed to create server");
		UE_LOG(LogTemp, Error, TEXT("Failed to create session: %s"), *SessionName.ToString());
		OnSessionCreated.Broadcast(false, TEXT("Session creation failed"));
	}
}

void UTLS_NetworkManager::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session destroyed: %s"), 
		bWasSuccessful ? TEXT("Success") : TEXT("Failed"));
}

void UTLS_NetworkManager::OnFindSessionsComplete(bool bWasSuccessful)
{
	SessionState = TEXT("Search complete");
	UE_LOG(LogTemp, Warning, TEXT("Session search complete: %d results"), SearchResults.Num());
}

void UTLS_NetworkManager::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		bIsConnected = true;
		SessionState = TEXT("Connected");
		UE_LOG(LogTemp, Warning, TEXT("Successfully joined session"));

		// Travel to the server
		if (SessionInterface.IsValid())
		{
			FString TravelURL;
			if (SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
			{
				GetWorld()->ClientTravel(TravelURL, TRAVEL_Absolute);
			}
		}

		OnSessionJoined.Broadcast(true, TEXT("Joined successfully"));
	}
	else
	{
		SessionState = TEXT("Join failed");
		UE_LOG(LogTemp, Error, TEXT("Failed to join session"));
		OnSessionJoined.Broadcast(false, TEXT("Join failed"));
	}
}
