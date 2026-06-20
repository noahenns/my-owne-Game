// TruckLogisticsSimulator - Vehicle Physics Component Implementation
// Copyright 2026. All rights reserved.

#include "Gameplay/Vehicle/VehiclePhysicsComponent.h"
#include "GameFramework/Actor.h"

UVehiclePhysicsComponent::UVehiclePhysicsComponent()
{
	PrimaryComponentTick.TickInterval = 0.016f;  // ~60 FPS
	PrimaryComponentTick.TickType = ETickableTickType::Always;
	bTickInEditor = false;
}

void UVehiclePhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		_CurrentLocation = GetOwner()->GetActorLocation();
		CurrentYaw = GetOwner()->GetActorRotation().Yaw * PI / 180.0f;  // Convert to radians
	}
}

void UVehiclePhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateSpeed(DeltaTime);
	UpdateRotation(DeltaTime);
	UpdatePosition(DeltaTime);
	PerformGroundTrace();
}

void UVehiclePhysicsComponent::SetThrottleInput(float Value)
{
	_ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UVehiclePhysicsComponent::SetBrakeInput(float Value)
{
	_BrakeInput = FMath::Clamp(Value, 0.0f, 1.0f);
}

void UVehiclePhysicsComponent::SetSteeringInput(float Value)
{
	_SteeringInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

FVector UVehiclePhysicsComponent::GetVelocityDirection() const
{
	return FVector(FMath::Cos(CurrentYaw), FMath::Sin(CurrentYaw), 0.0f);
}

float UVehiclePhysicsComponent::CalculateTraction(float WetnessAmount) const
{
	float Traction = BaseTraction;

	// Reduce traction at high speed (understeer)
	float SpeedFactor = FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f);
	Traction *= (1.0f - (SpeedFactor * 0.2f));  // Up to 20% grip loss at max speed

	// Reduce traction on wet surfaces
	if (WetnessAmount > 0.5f)
	{
		Traction *= WetSurfaceTraction;
	}

	return FMath::Clamp(Traction, 0.0f, 1.0f);
}

void UVehiclePhysicsComponent::UpdateSpeed(float DeltaTime)
{
	if (DeltaTime <= 0.0f)
		return;

	float Traction = CalculateTraction();

	// Calculate acceleration/braking
	float Acceleration = 0.0f;

	if (_BrakeInput > 0.0f)
	{
		// Braking (always reduces speed)
		Acceleration = -BrakingDeceleration * _BrakeInput * Traction;
	}
	else if (_ThrottleInput > 0.0f)
	{
		// Forward acceleration
		Acceleration = AccelerationRate * _ThrottleInput * Traction;
	}
	else if (_ThrottleInput < 0.0f)
	{
		// Reverse (only if speed is low)
		if (CurrentSpeed < 1.0f)
		{
			Acceleration = -AccelerationRate * 0.5f * FMath::Abs(_ThrottleInput);
		}
		else
		{
			// Braking effect when trying to reverse while moving forward
			Acceleration = -BrakingDeceleration * 0.3f;
		}
	}
	else
	{
		// Natural drag (coast down)
		Acceleration = -NaturalDrag;
	}

	// Apply acceleration to speed
	CurrentSpeed += Acceleration * DeltaTime;

	// Clamp speed
	float SpeedLimit = (_ThrottleInput < 0.0f && CurrentSpeed < 1.0f) ? MaxReverseSpeed : MaxSpeed;
	CurrentSpeed = FMath::Clamp(CurrentSpeed, -SpeedLimit, SpeedLimit);

	// Stop if speed is very low
	if (FMath::Abs(CurrentSpeed) < 0.1f)
	{
		CurrentSpeed = 0.0f;
	}
}

void UVehiclePhysicsComponent::UpdateRotation(float DeltaTime)
{
	if (DeltaTime <= 0.0f || FMath::Abs(CurrentSpeed) < 0.1f)
		return;

	// Only steer while moving
	if (FMath::Abs(_SteeringInput) > 0.01f)
	{
		// Steering rate depends on speed (slower at higher speeds = more realistic)
		float SpeedFactor = FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f);
		float SteeringRate = MaxTurnRate * SpeedFactor;

		CurrentYaw += _SteeringInput * SteeringRate * DeltaTime;
	}

	// Normalize yaw to 0-2π
	if (CurrentYaw > PI)
		CurrentYaw -= 2.0f * PI;
	if (CurrentYaw < -PI)
		CurrentYaw += 2.0f * PI;

	// Update actor rotation
	if (GetOwner())
	{
		FRotator NewRotation = GetOwner()->GetActorRotation();
		NewRotation.Yaw = CurrentYaw * 180.0f / PI;  // Convert back to degrees
		GetOwner()->SetActorRotation(NewRotation);
	}
}

void UVehiclePhysicsComponent::UpdatePosition(float DeltaTime)
{
	if (DeltaTime <= 0.0f)
		return;

	if (!GetOwner())
		return;

	// Calculate displacement
	FVector Direction = GetVelocityDirection();
	FVector Displacement = Direction * CurrentSpeed * DeltaTime;

	_CurrentLocation += Displacement;

	// Update actor position
	GetOwner()->SetActorLocation(_CurrentLocation, true);
}

void UVehiclePhysicsComponent::PerformGroundTrace()
{
	if (!GetOwner() || !GetWorld())
		return;

	// Simple ground trace to detect if we're on terrain
	FHitResult HitResult;
	FVector TraceStart = _CurrentLocation + FVector(0, 0, 50);
	FVector TraceEnd = _CurrentLocation - FVector(0, 0, 200);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	_bIsOnGround = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_WorldStatic,
		QueryParams
	);

	// Snap to ground if close
	if (_bIsOnGround && HitResult.bBlockingHit)
	{
		float GroundDistance = FVector::Dist(_CurrentLocation, HitResult.ImpactPoint);
		if (GroundDistance < 100.0f)
		{
			_CurrentLocation.Z = HitResult.ImpactPoint.Z;
		}
	}
}
