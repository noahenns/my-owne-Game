// TruckLogisticsSimulator - Main Game Mode Implementation
// Copyright 2026. All rights reserved.

#include "Core/TLS_GameMode_Main.h"
#include "Gameplay/Vehicle/TruckPawn.h"
#include "Gameplay/Route/RouteManager.h"
#include "Multiplayer/TLS_GameState.h"
#include "Multiplayer/TLS_PlayerState.h"
#include "Multiplayer/TLS_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PlayerController.h"

ATLS_GameMode_Main::ATLS_GameMode_Main()
{
	PrimaryActorTick.TickInterval = 0.1f;

	// Use our custom pawn
	DefaultPawnClass = ATruckPawn::StaticClass();

	// Set custom game state and player controller
	GameStateClass = ATLS_GameState::StaticClass();
	PlayerStateClass = ATLS_PlayerState::StaticClass();
	PlayerControllerClass = ATLS_PlayerController::StaticClass();

	bUseClassesForPlayers = false;
	bStartPlayersAsSpectators = false;
}

void ATLS_GameMode_Main::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("Game Mode initialized"));
}

void ATLS_GameMode_Main::BeginPlay()
{
	Super::BeginPlay();

	// Get game state
	GameStateRef = Cast<ATLS_GameState>(GameState);
	if (!GameStateRef)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get game state!"));
	}

	// Get route manager from game state
	if (GameStateRef)
	{
		RouteManagerInstance = GameStateRef->GetRouteManager();
	}

	UE_LOG(LogTemp, Warning, TEXT("Game started! Mode: %s"), 
		GetNetMode() == NM_ListenServer ? TEXT("Listen Server") : TEXT("Dedicated Server"));
}

void ATLS_GameMode_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateConnectedPlayers();
}

void ATLS_GameMode_Main::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
		return;

	ATLS_PlayerState* PS = NewPlayer->GetPlayerState<ATLS_PlayerState>();
	if (PS)
	{
		PS->PlayerProgression.TotalMoney = 1000.0f;
		PS->PlayerProgression.TotalXP = 0;
		PS->PlayerProgression.CompletedDeliveries = 0;

		UE_LOG(LogTemp, Warning, TEXT("Player joined: %s"), *PS->GetPlayerName());
	}

	// Spawn truck for this player
	ATruckPawn* Truck = Cast<ATruckPawn>(NewPlayer->GetPawn());
	if (Truck)
	{
		Truck->SetActorLocation(FVector(5000, 5000, 100));
		UE_LOG(LogTemp, Warning, TEXT("Truck spawned for player at (5000, 5000, 100)"));
	}
}

void ATLS_GameMode_Main::Logout(AController* Exiting)
{
	ATLS_PlayerState* PS = Exiting ? Exiting->GetPlayerState<ATLS_PlayerState>() : nullptr;
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player left: %s (Money: $%.2f)"), 
			*PS->GetPlayerName(), PS->PlayerProgression.TotalMoney);
	}

	Super::Logout(Exiting);
}

void ATLS_GameMode_Main::UpdateConnectedPlayers()
{
	// This is called every tick to keep track of connected players
	// Can be used for scoreboard updates, player count, etc.
}

ATLS_GameState* ATLS_GameMode_Main::GetGameState_Custom() const
{
	return GameStateRef;
}
