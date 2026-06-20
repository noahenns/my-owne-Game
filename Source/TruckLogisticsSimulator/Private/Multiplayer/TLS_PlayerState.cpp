// TruckLogisticsSimulator - Multiplayer Player State Implementation
// Copyright 2026. All rights reserved.

#include "Multiplayer/TLS_PlayerState.h"
#include "Net/UnrealNetwork.h"

ATLS_PlayerState::ATLS_PlayerState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATLS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATLS_PlayerState, PlayerProgression);
	DOREPLIFETIME(ATLS_PlayerState, CurrentRouteIndex);
	DOREPLIFETIME(ATLS_PlayerState, CurrentRoute);
	DOREPLIFETIME(ATLS_PlayerState, ReplicatedPlayerColor);
}

void ATLS_PlayerState::AddMoney(float Amount)
{
	if (!HasAuthority())
		return;

	PlayerProgression.TotalMoney += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Player %s: +$%.2f (Total: $%.2f)"),
		*GetPlayerName(), Amount, PlayerProgression.TotalMoney);
}

void ATLS_PlayerState::AddXP(int32 Amount)
{
	if (!HasAuthority())
		return;

	int32 PreviousLevel = PlayerProgression.GetCurrentLevel();
	PlayerProgression.TotalXP += Amount;
	int32 NewLevel = PlayerProgression.GetCurrentLevel();

	if (NewLevel > PreviousLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s: LEVEL UP! Now level %d"),
			*GetPlayerName(), NewLevel);
	}
}

void ATLS_PlayerState::SetCurrentRoute(int32 RouteIndex)
{
	if (!HasAuthority())
		return;

	CurrentRouteIndex = RouteIndex;
	UE_LOG(LogTemp, Warning, TEXT("Player %s: Accepted route %d"), *GetPlayerName(), RouteIndex);
}

void ATLS_PlayerState::OnRep_PlayerColor()
{
	PlayerColor = ReplicatedPlayerColor;
}

void ATLS_PlayerState::AssignPlayerColor()
{
	if (!HasAuthority())
		return;

	// Assign colors based on player index
	int32 PlayerIndex = GetPlayerId();
	switch (PlayerIndex % 4)
	{
	case 0:
		ReplicatedPlayerColor = FLinearColor::Red;
		break;
	case 1:
		ReplicatedPlayerColor = FLinearColor::Blue;
		break;
	case 2:
		ReplicatedPlayerColor = FLinearColor::Green;
		break;
	case 3:
		ReplicatedPlayerColor = FLinearColor::Yellow;
		break;
	}

	OnRep_PlayerColor();
}
