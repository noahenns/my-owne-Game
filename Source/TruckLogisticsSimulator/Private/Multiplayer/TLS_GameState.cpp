// TruckLogisticsSimulator - Multiplayer Game State Implementation
// Copyright 2026. All rights reserved.

#include "Multiplayer/TLS_GameState.h"
#include "Gameplay/Route/RouteManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ATLS_GameState::ATLS_GameState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ATLS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATLS_GameState, TotalPlayersInGame);
	DOREPLIFETIME(ATLS_GameState, ElapsedGameTime);
}

void ATLS_GameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Get or spawn route manager
		_RouteManager = ARouteManager::GetInstance(GetWorld());
		if (!_RouteManager)
		{
			_RouteManager = GetWorld()->SpawnActor<ARouteManager>();
		}

		// Detect if listen server
		bIsListenServer = GetNetMode() == NM_ListenServer;

		UE_LOG(LogTemp, Warning, TEXT("Game State initialized - Listen Server: %s, Players: %d"),
			bIsListenServer ? TEXT("Yes") : TEXT("No"), PlayerArray.Num());
	}
}

void ATLS_GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		ElapsedGameTime += DeltaSeconds;
		TotalPlayersInGame = PlayerArray.Num();
	}
}
