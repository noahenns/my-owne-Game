// TruckLogisticsSimulator - Truck Pawn Implementation
// Copyright 2026. All rights reserved.

#include "Gameplay/Vehicle/TruckPawn.h"
#include "Gameplay/Vehicle/VehiclePhysicsComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"

ATruckPawn::ATruckPawn()
{
	PrimaryActorTick.TickInterval = 0.016f;
	PrimaryActorTick.bTickEvenWhenPaused = false;

	// Networking
	bReplicates = true;
	bReplicateMovement = true;

	// Don't use character movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create truck mesh
	TruckMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TruckMesh"));
	TruckMesh->SetCollisionEnabled(ECC_GameTraceChannel1);
	RootComponent = TruckMesh;

	// Create physics component
	PhysicsComponent = CreateDefaultSubobject<UVehiclePhysicsComponent>(TEXT("PhysicsComponent"));

	// Create camera boom (follows truck)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 300.0f);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;

	// Create camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->FieldOfView = 90.0f;

	// Initialize fuel
	CurrentFuel = FuelCapacity;
}

void ATruckPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set up Enhanced Input (UE5 standard input system)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(DefaultInputMapping, 0);
		}
	}

	// Update camera boom from properties
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = CameraBoomLength;
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, CameraBoomHeight);
	}

	if (CameraComponent)
	{
		CameraComponent->FieldOfView = CameraFOV;
	}

	CurrentFuel = FuelCapacity;

	UE_LOG(LogTemp, Warning, TEXT("TruckPawn initialized at position: %s"), *GetActorLocation().ToString());
}

void ATruckPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ConsumeFuel(DeltaTime);

	// Update camera boom to look slightly ahead of truck
	if (CameraBoom)
	{
		FVector LookDirection = PhysicsComponent->GetVelocityDirection();
		FVector TargetBoomOffset = FVector(LookDirection.X * 200.0f, LookDirection.Y * 200.0f, CameraBoomHeight);
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoom->SocketOffset, TargetBoomOffset, 0.1f);
	}
}

void ATruckPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent called with null InputComponent!"));
		return;
	}

	// Standard input binding for prototyping (no Enhanced Input required)
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATruckPawn::AccelerateTruck);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATruckPawn::SteerTruck);

	// For now, brake with S key (mapped to negative MoveForward)
	// We'll implement brake separately via input remapping in GameMode
}

void ATruckPawn::AccelerateTruck(float Value)
{
	if (PhysicsComponent)
	{
		if (Value > 0.0f)
		{
			PhysicsComponent->SetThrottleInput(Value);
			PhysicsComponent->SetBrakeInput(0.0f);
		}
		else if (Value < 0.0f)
		{
			PhysicsComponent->SetThrottleInput(0.0f);
			PhysicsComponent->SetBrakeInput(FMath::Abs(Value));
		}
		else
		{
			PhysicsComponent->SetThrottleInput(0.0f);
			PhysicsComponent->SetBrakeInput(0.0f);
		}
	}
}

void ATruckPawn::BrakeTruck(float Value)
{
	if (PhysicsComponent)
	{
		PhysicsComponent->SetBrakeInput(FMath::Clamp(Value, 0.0f, 1.0f));
	}
}

void ATruckPawn::SteerTruck(float Value)
{
	if (PhysicsComponent)
	{
		PhysicsComponent->SetSteeringInput(Value);
	}
}

float ATruckPawn::GetCurrentSpeedKMH() const
{
	return PhysicsComponent ? PhysicsComponent->GetSpeedKMH() : 0.0f;
}

bool ATruckPawn::HasCargo() const
{
	// TODO: Connect to cargo system when implemented
	return false;
}

float ATruckPawn::GetFuelPercentage() const
{
	if (FuelCapacity <= 0.0f)
		return 0.0f;
	return (CurrentFuel / FuelCapacity) * 100.0f;
}

float ATruckPawn::GetCargoWeight() const
{
	// TODO: Connect to cargo system when implemented
	return 0.0f;
}

void ATruckPawn::ConsumeFuel(float DeltaTime)
{
	if (DeltaTime <= 0.0f)
		return;

	// Only consume fuel when moving
	if (PhysicsComponent && PhysicsComponent->IsMoving())
	{
		float DistanceTraveled = PhysicsComponent->GetCurrentSpeed() * DeltaTime;  // meters
		float FuelUsed = DistanceTraveled * FuelConsumptionRate;

		CurrentFuel = FMath::Max(0.0f, CurrentFuel - FuelUsed);

		if (CurrentFuel <= 0.0f)
		{
			// Stop the truck if out of fuel
			if (PhysicsComponent)
			{
				PhysicsComponent->SetThrottleInput(0.0f);
			}
		}
	}
}

void ATruckPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate movement automatically (handled by bReplicateMovement)
	// PhysicsComponent position/rotation will sync across network
}
