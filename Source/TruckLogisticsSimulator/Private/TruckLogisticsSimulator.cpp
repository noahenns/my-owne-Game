// Copyright Epic Games, Inc. All Rights Reserved.

#include "TruckLogisticsSimulator.h"

#define LOCTEXT_NAMESPACE "FTruckLogisticsSimulatorModule"

void FTruckLogisticsSimulatorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file.
	UE_LOG(LogTemp, Warning, TEXT("TruckLogisticsSimulator module has been loaded"));
}

void FTruckLogisticsSimulatorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Warning, TEXT("TruckLogisticsSimulator module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTruckLogisticsSimulatorModule, TruckLogisticsSimulator)
