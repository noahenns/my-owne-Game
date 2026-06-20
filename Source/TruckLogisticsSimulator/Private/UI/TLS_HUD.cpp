// TruckLogisticsSimulator - HUD Implementation
// Copyright 2026. All rights reserved.

#include "UI/TLS_HUD.h"
#include "Gameplay/Vehicle/TruckPawn.h"
#include "Core/TLS_GameMode_Main.h"
#include "Gameplay/Route/RouteManager.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

ATLS_HUD::ATLS_HUD()
{
	PrimaryActorTick.TickInterval = 0.1f;
}

void ATLS_HUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerTruckPawn = Cast<ATruckPawn>(GetOwningPawn());
	GameMode = Cast<ATLS_GameMode_Main>(GetWorld()->GetAuthGameMode());
	RouteManager = ARouteManager::GetInstance(GetWorld());

	if (!DefaultFont)
	{
		DefaultFont = GEngine->GetSmallFont();
	}
}

void ATLS_HUD::DrawHUD()
{
	Super::DrawHUD();

	if (!PlayerTruckPawn || !GameMode)
		return;

	// Draw HUD elements
	DrawSpeedometer();
	DrawFuelGauge();
	DrawPlayerStats();
	DrawRouteInfo();
	DrawDebugInfo();
}

void ATLS_HUD::DrawSpeedometer()
{
	float Speed = PlayerTruckPawn->GetCurrentSpeedKMH();
	FString SpeedText = FString::Printf(TEXT("Speed: %.1f km/h"), Speed);

	// Color changes with speed
	FColor SpeedColor = TextColor;
	if (Speed > 80.0f)
		SpeedColor = WarningColor;
	if (Speed > 100.0f)
		SpeedColor = CriticalColor;

	DrawTextAt(50.0f, 50.0f, SpeedText, SpeedColor);

	// Speed bar
	float SpeedPercentage = FMath::Clamp(Speed / 120.0f, 0.0f, 1.0f);
	DrawBar(50.0f, 70.0f, 200.0f, 20.0f, SpeedPercentage, FColor::Green);
}

void ATLS_HUD::DrawFuelGauge()
{
	float FuelPercent = PlayerTruckPawn->GetFuelPercentage();
	FString FuelText = FString::Printf(TEXT("Fuel: %.1f%%"), FuelPercent);

	FColor FuelColor = TextColor;
	if (FuelPercent < 25.0f)
		FuelColor = WarningColor;
	if (FuelPercent < 10.0f)
		FuelColor = CriticalColor;

	DrawTextAt(50.0f, 100.0f, FuelText, FuelColor);

	// Fuel bar
	DrawBar(50.0f, 120.0f, 200.0f, 20.0f, FuelPercent / 100.0f, FColor::Yellow);
}

void ATLS_HUD::DrawPlayerStats()
{
	FPlayerProgression Progress = GameMode->GetPlayerProgression();

	FString MoneyText = FString::Printf(TEXT("Money: $%.2f"), Progress.TotalMoney);
	FString LevelText = FString::Printf(TEXT("Level: %d (XP: %d)"), Progress.GetCurrentLevel(), Progress.TotalXP);
	FString DeliveriesText = FString::Printf(TEXT("Deliveries: %d"), Progress.CompletedDeliveries);

	DrawTextAt(Canvas->SizeX - 300.0f, 50.0f, MoneyText, TextColor);
	DrawTextAt(Canvas->SizeX - 300.0f, 75.0f, LevelText, TextColor);
	DrawTextAt(Canvas->SizeX - 300.0f, 100.0f, DeliveriesText, TextColor);
}

void ATLS_HUD::DrawRouteInfo()
{
	if (!RouteManager || !RouteManager->HasActiveRoute())
	{
		DrawTextAt(Canvas->SizeX / 2.0f - 100.0f, Canvas->SizeY - 100.0f, TEXT("No Active Route"), WarningColor);
		return;
	}

	FRouteProgress Progress = RouteManager->GetCurrentRouteProgress();
	FVector NextWaypoint = RouteManager->GetNextWaypoint();
	float DistanceToNext = RouteManager->GetDistanceToNextWaypoint(PlayerTruckPawn->GetTruckPosition());

	FString RouteText = FString::Printf(TEXT("Route: %s"), *Progress.CurrentRoute.RouteName);
	FString DistanceText = FString::Printf(TEXT("Distance to waypoint: %.1f m"), DistanceToNext);
	FString ProgressText = FString::Printf(TEXT("Route progress: %.1f%%"), Progress.GetCompletionPercentage() * 100.0f);

	DrawTextAt(Canvas->SizeX / 2.0f - 150.0f, Canvas->SizeY - 150.0f, RouteText, TextColor);
	DrawTextAt(Canvas->SizeX / 2.0f - 150.0f, Canvas->SizeY - 120.0f, DistanceText, TextColor);
	DrawTextAt(Canvas->SizeX / 2.0f - 150.0f, Canvas->SizeY - 90.0f, ProgressText, TextColor);
}

void ATLS_HUD::DrawDebugInfo()
{
	if (!PlayerTruckPawn)
		return;

	FVector TruckPos = PlayerTruckPawn->GetTruckPosition();
	FString PosText = FString::Printf(TEXT("Position: %.0f, %.0f, %.0f"), TruckPos.X, TruckPos.Y, TruckPos.Z);

	DrawTextAt(50.0f, Canvas->SizeY - 50.0f, PosText, FColor::Cyan);
}

void ATLS_HUD::DrawTextAt(float X, float Y, const FString& Text, const FColor& Color)
{
	if (!DefaultFont)
		DefaultFont = GEngine->GetSmallFont();

	Canvas->SetDrawColor(Color);
	Canvas->DrawText(DefaultFont, Text, X, Y);
}

void ATLS_HUD::DrawBar(float X, float Y, float Width, float Height, float Percentage, const FColor& BarColor)
{
	// Draw background
	Canvas->SetDrawColor(FColor::DarkGray);
	Canvas->DrawTile(X, Y, Width, Height, 0.0f, 0.0f, 32.0f, 32.0f);

	// Draw filled portion
	float FilledWidth = Width * FMath::Clamp(Percentage, 0.0f, 1.0f);
	Canvas->SetDrawColor(BarColor);
	Canvas->DrawTile(X, Y, FilledWidth, Height, 0.0f, 0.0f, 32.0f, 32.0f);

	// Draw border
	Canvas->SetDrawColor(FColor::White);
	Canvas->DrawTile(X - 2.0f, Y - 2.0f, Width + 4.0f, 2.0f, 0, 0, 32, 32);  // Top
	Canvas->DrawTile(X - 2.0f, Y + Height, Width + 4.0f, 2.0f, 0, 0, 32, 32); // Bottom
	Canvas->DrawTile(X - 2.0f, Y, 2.0f, Height, 0, 0, 32, 32);                // Left
	Canvas->DrawTile(X + Width, Y, 2.0f, Height, 0, 0, 32, 32);               // Right
}
