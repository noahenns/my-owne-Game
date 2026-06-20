// TruckLogisticsSimulator - Multiplayer Player Controller Implementation
// Copyright 2026. All rights reserved.

#include "Multiplayer/TLS_PlayerController.h"
#include "Gameplay/Vehicle/TruckPawn.h"
#include "Gameplay/Vehicle/VehiclePhysicsComponent.h"
#include "Gameplay/Route/RouteManager.h"
#include "Multiplayer/TLS_PlayerState.h"
#include "InputActionValue.h"

ATLS_PlayerController::ATLS_PlayerController()
{
	bReplicates = true;
}

void ATLS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledTruck = Cast<ATruckPawn>(GetPawn());
	PlayerStateRef = GetPlayerState<ATLS_PlayerState>();

	if (!ControlledTruck)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController: No truck pawn!"));
	}

	if (!PlayerStateRef)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController: No player state!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerController initialized for %s"), *GetPlayerName());
}

void ATLS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
		return;

	// Setup axis inputs
	InputComponent->BindAxis("MoveForward", this, &ATLS_PlayerController::Input_Accelerate);
	InputComponent->BindAxis("MoveRight", this, &ATLS_PlayerController::Input_Steer);
	InputComponent->BindAction("Brake", IE_Pressed, this, &ATLS_PlayerController::Input_Brake, 1.0f);
	InputComponent->BindAction("Brake", IE_Released, this, &ATLS_PlayerController::Input_Brake, 0.0f);
	InputComponent->BindAction("Pause", IE_Pressed, this, &ATLS_PlayerController::Input_Pause);
}

void ATLS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ensure truck is still valid
	if (!ControlledTruck)
	{
		ControlledTruck = Cast<ATruckPawn>(GetPawn());
	}
}

void ATLS_PlayerController::Input_Accelerate(float Value)
{
	if (!ControlledTruck || !bInputEnabled)
		return;

	if (Value > 0.0f)
	{
		ControlledTruck->AccelerateTruck(Value);
	}
	else if (Value < 0.0f)
	{
		ControlledTruck->BrakeTruck(-Value);
	}
	else
	{
		ControlledTruck->AccelerateTruck(0.0f);
		ControlledTruck->BrakeTruck(0.0f);
	}
}

void ATLS_PlayerController::Input_Brake(float Value)
{
	if (!ControlledTruck || !bInputEnabled)
		return;

	ControlledTruck->BrakeTruck(Value);
}

void ATLS_PlayerController::Input_Steer(float Value)
{
	if (!ControlledTruck || !bInputEnabled)
		return;

	ControlledTruck->SteerTruck(Value);
}

void ATLS_PlayerController::Input_Pause()
{
	// Toggle pause
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		bool bNewPause = !GetWorld()->IsPaused();
		GetWorld()->SetWorldPauseState(bNewPause);
		UE_LOG(LogTemp, Warning, TEXT("Game %s"), bNewPause ? TEXT("Paused") : TEXT("Resumed"));
	}
}

ATLS_PlayerState* ATLS_PlayerController::GetPlayerStateMultiplayer() const
{
	return PlayerStateRef ? PlayerStateRef : GetPlayerState<ATLS_PlayerState>();
}

void ATLS_PlayerController::Server_AcceptRoute_Implementation(int32 RouteIndex)
{
	if (!HasAuthority())
		return;

	ARouteManager* RouteManager = ARouteManager::GetInstance(GetWorld());
	if (!RouteManager)
		return;

	RouteManager->AcceptRoute(RouteIndex);

	if (PlayerStateRef)
	{
		PlayerStateRef->SetCurrentRoute(RouteIndex);
	}

	UE_LOG(LogTemp, Warning, TEXT("Route %d accepted by %s"), RouteIndex, *GetPlayerName());
}

bool ATLS_PlayerController::Server_AcceptRoute_Validate(int32 RouteIndex)
{
	return RouteIndex >= 0 && RouteIndex < 100;  // Basic validation
}

void ATLS_PlayerController::Server_DeliverCargo_Implementation()
{
	if (!HasAuthority() || !ControlledTruck)
		return;

	ARouteManager* RouteManager = ARouteManager::GetInstance(GetWorld());
	if (!RouteManager || !RouteManager->HasActiveRoute())
		return;

	// Check if at delivery point
	if (RouteManager->IsPlayerNearDeliveryPoint(ControlledTruck->GetTruckPosition(), 1000.0f))
	{
		if (PlayerStateRef)
		{
			// Calculate rewards
			float MoneyEarned = 500.0f;  // Default reward (would calculate based on route)
			int32 XPEarned = 50;

			PlayerStateRef->AddMoney(MoneyEarned);
			PlayerStateRef->AddXP(XPEarned);

			UE_LOG(LogTemp, Warning, TEXT("%s delivered cargo! +$%.2f +%d XP"),
				*GetPlayerName(), MoneyEarned, XPEarned);
		}
	}
}

bool ATLS_PlayerController::Server_DeliverCargo_Validate()
{
	return true;
}
