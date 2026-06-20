// TruckLogisticsSimulator - HUD Display
// Copyright 2026. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TLS_HUD.generated.h"

class ATruckPawn;
class ATLS_GameMode_Main;
class ARouteManager;

/**
 * Simple HUD for displaying vehicle and game information
 */
UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATLS_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ATLS_HUD();

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	// Font for text rendering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	UFont* DefaultFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	FColor TextColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	FColor WarningColor = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	FColor CriticalColor = FColor::Red;

protected:
	ATruckPawn* PlayerTruckPawn = nullptr;
	ATLS_GameMode_Main* GameMode = nullptr;
	ARouteManager* RouteManager = nullptr;

	void DrawSpeedometer();
	void DrawFuelGauge();
	void DrawPlayerStats();
	void DrawRouteInfo();
	void DrawDebugInfo();

	// Utility
	void DrawTextAt(float X, float Y, const FString& Text, const FColor& Color = FColor::White);
	void DrawBar(float X, float Y, float Width, float Height, float Percentage, const FColor& BarColor);
};
