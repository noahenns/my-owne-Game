# Contributing to TruckLogisticsSimulator

Welcome! This document outlines coding standards, contribution workflow, and best practices for this project.

## Code Standards

### C++ Style Guide

**Naming Conventions** (Unreal Engine standards):

| Element | Convention | Example |
|---------|-----------|---------|
| Classes | Prefix with `A` (Actor), `U` (UObject), `F` (Struct), or `I` (Interface) | `ATruckPawn`, `UCargoSystem`, `FDeliveryRoute`, `IVehicleInput` |
| Variables | CamelCase, private with `_Prefix` | `float TruckSpeed;`, `FVector _LocalPosition;` |
| Functions | CamelCase, verb-noun | `void LoadCargo()`, `FVector GetWaypoint()`, `bool IsRouteComplete()` |
| Constants | All caps with `_` | `static constexpr float MAX_TRUCK_SPEED = 100.0f;` |
| Booleans | Prefix with `b` | `bool bIsLoaded;`, `bool bCanAccelerate;` |
| Pointers | Use `*` after type | `UCargoSystem* CargoSystem;` |
| References | Use `&` for non-null, `TOptional<>` for optional | `const FVector& InLocation;` |

**Example Class Declaration**:

```cpp
// Public/Gameplay/Vehicle/TruckPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TruckPawn.generated.h"

UCLASS()
class TRUCKLOGISTICSSIMULATOR_API ATruckPawn : public APawn
{
    GENERATED_BODY()

public:
    ATruckPawn();
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // Public interface
    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    void AccelerateTruck(float ThrottleInput);
    
    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    float GetCurrentSpeed() const { return CurrentSpeed; }

protected:
    // Physics parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics")
    float MaxSpeed = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Physics")
    float AccelerationRate = 5.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|State")
    float CurrentSpeed = 0.0f;
    
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* CameraBoom;
    
    // Input state
    float _SteeringInput = 0.0f;
    float _ThrottleInput = 0.0f;
    float _BrakeInput = 0.0f;

private:
    void HandleSteering(float DeltaTime);
    void UpdatePhysics(float DeltaTime);
    
    float _CurrentRotation = 0.0f;
};
```

### Comments & Documentation

**Header Comments** — Describe purpose and usage:

```cpp
/**
 * ATruckPawn
 * 
 * Drivable truck pawn with physics-based steering, acceleration, and braking.
 * Supports keyboard and gamepad input. Camera follows third-person perspective.
 * 
 * Usage:
 *   Place in level and set as Possessed pawn in PlayerController.
 *   Physics tuned for arcade-style handling (not simulation-realistic).
 */
```

**Function Comments** — For complex logic:

```cpp
/**
 * CalculateTireFriction
 * 
 * Computes tire grip coefficient based on speed, terrain, and weather.
 * Uses simplified model: friction decreases with speed, varies with terrain.
 * 
 * @param Speed         Current vehicle speed (km/h)
 * @param TerrainWetness 0.0 (dry) to 1.0 (wet)
 * @return Friction coefficient (0.0 to 1.0)
 */
float CalculateTireFriction(float Speed, float TerrainWetness) const;
```

**Inline Comments** — For non-obvious logic:

```cpp
void ATruckPawn::UpdatePhysics(float DeltaTime)
{
    // Apply tire friction: reduce speed based on terrain and steering angle
    float FrictionMultiplier = CalculateTireFriction(CurrentSpeed, 0.0f);
    float AppliedBraking = _BrakeInput > 0.0f ? BrakingDeceleration : 0.0f;
    
    // Update speed with acceleration/braking, clamped to max
    CurrentSpeed += (_ThrottleInput * AccelerationRate - AppliedBraking) * DeltaTime;
    CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.0f, MaxSpeed * FrictionMultiplier);
    
    // TODO: Add aerodynamic drag at high speeds
}
```

### Code Organization

**Header Files** (`.h` in `Public/`):
- Expose only public API
- Use forward declarations to minimize includes
- Document public functions with `UFUNCTION()` macros for Blueprint exposure

**Implementation Files** (`.cpp` in `Private/`):
- Include only what's needed
- Keep implementation details private
- Use static functions for internal helpers

**File Structure**:
```cpp
// Public/Gameplay/Cargo/CargoSystem.h

// 1. Includes
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CargoSystem.generated.h"

// 2. Forward declarations
class ARouteManager;

// 3. Enums & type definitions
UENUM(BlueprintType)
enum class ECargoType : uint8
{
    Fragile UMETA(DisplayName = "Fragile"),
    Hazmat UMETA(DisplayName = "Hazmat"),
    Bulk UMETA(DisplayName = "Bulk")
};

// 4. Structs
USTRUCT(BlueprintType)
struct FCargoInstance
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadWrite)
    ECargoType CargoType = ECargoType::Bulk;
    
    UPROPERTY(BlueprintReadWrite)
    float Weight = 0.0f;
};

// 5. Main class declaration
UCLASS(ClassGroup=(Cargo), meta=(BlueprintSpawnableComponent))
class TRUCKLOGISTICSSIMULATOR_API UCargoSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UCargoSystem();
    
    virtual void BeginPlay() override;
    
    // Public API
    UFUNCTION(BlueprintCallable, Category = "Cargo")
    void LoadCargo(const FCargoInstance& InCargo);
    
    UFUNCTION(BlueprintCallable, Category = "Cargo")
    void UnloadCargo();
    
    UFUNCTION(BlueprintCallable, Category = "Cargo")
    bool IsLoaded() const { return bIsLoaded; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo|State")
    FCargoInstance CurrentCargo;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo|State")
    bool bIsLoaded = false;

private:
    void ApplyLoadingAnimation();
    void ValidateDelivery();
};
```

---

## Blueprint Guidelines

### Blueprint Naming

| Asset Type | Prefix | Example |
|------------|--------|---------|
| Character Blueprint | `BP_` | `BP_TruckCharacter`, `BP_PlayerController` |
| Pawn Blueprint | `BP_` | `BP_TruckVehicle` |
| Actor Blueprint | `BP_` | `BP_DepotMarker`, `BP_WaypointIndicator` |
| Widget Blueprint | `WBP_` | `WBP_HUD_Speedometer`, `WBP_Menu_Main` |
| Data Table | `DT_` | `DT_Routes`, `DT_CargoTypes` |
| Enum | `E_` | `E_CargoType`, `E_GameState` |

### Blueprint Organization

**Folder Structure in Content Browser**:
```
Content/
├── Blueprints/
│   ├── Character/
│   │   └── BP_TruckCharacter
│   ├── Pawns/
│   │   └── BP_TruckVehicle
│   ├── GameModes/
│   │   └── BP_GameMode_Main
│   └── Systems/
│       └── BP_RouteManager
└── DataTables/
    ├── DT_Routes
    ├── DT_CargoTypes
    └── DT_Vehicles
```

### Blueprint Best Practices

1. **Keep logic in C++** where possible (performance, reusability)
2. **Use Blueprints for**:
   - UI/UMG widgets
   - Level layout and actor placement
   - Quick content iteration
3. **Expose properties with `UPROPERTY(EditAnywhere)` for tweaking in Details panel**
4. **Use meaningful variable names** in Blueprint nodes (auto-renamed by property name)
5. **Comment Blueprint graphs** using Comment nodes

---

## Asset Management

### Asset Naming Convention

| Asset Type | Prefix | Example |
|-----------|--------|---------|
| Static Mesh | `SM_` | `SM_Cargo_Box_Large`, `SM_Road_Asphalt` |
| Skeletal Mesh | `SK_` | `SK_TruckCab_001`, `SK_Driver` |
| Material | `M_` | `M_TruckPaint_Red`, `M_Road_Asphalt` |
| Material Instance | `MI_` | `MI_TruckPaint_Blue` |
| Particle System | `PS_` | `PS_Dust_Trail`, `PS_Engine_Smoke` |
| Sound Wave | `S_` | `S_Engine_Idle`, `S_Cargo_Loading` |
| Sound Cue | `Cue_` | `Cue_Engine`, `Cue_UI_Click` |
| Animation Sequence | `AS_` | `AS_Truck_Accelerate`, `AS_Cargo_Loading` |
| Level/Map | `L_` | `L_MainMenu`, `L_Sandbox_20km` |
| Blueprint | `BP_` or `WBP_` | (see Blueprint guidelines above) |
| Texture | `T_` | `T_TruckPaint_Base`, `T_Road_Asphalt_Normal` |

### Asset Versioning

- Use version numbers in asset names: `SM_Truck_v001`, `SM_Truck_v002`
- Keep old versions for rollback capability
- Clean up unused versions before releases

---

## Performance Guidelines

### CPU Performance Targets

- **Frame Time**: 16.67 ms (60 FPS)
- **Physics Update**: < 3 ms
- **Rendering**: < 10 ms
- **Game Logic**: < 3 ms

**Profiling Workflow**:
1. Open Unreal Insights (Ctrl+Shift+A)
2. Identify frame spike
3. Drill down into bottleneck (CPU vs GPU)
4. Optimize hot function
5. Re-profile to verify improvement

### Memory Targets

- **Peak Usage**: < 8 GB
- **Runtime (in-game)**: < 6 GB
- **Audio Streaming**: Enabled (don't load all audio at startup)
- **Texture Streaming**: Enabled (don't load all textures to VRAM)

### Draw Call Targets

- **Opaque**: < 300 draw calls per frame
- **Transparent**: < 100 draw calls per frame
- **Total**: < 500 per frame

**Optimization Tips**:
- Use material instances instead of unique materials
- Batch small actors into single mesh
- Use Level of Detail (LOD) for distant objects
- Disable shadows for non-critical objects

---

## Testing

### Unit Testing (Unreal Testing Framework)

Example test:

```cpp
// Source/TruckLogisticsSimulator/Tests/VehiclePhysicsTests.cpp

#include "Misc/AutomationTest.h"
#include "Gameplay/Vehicle/TruckPawn.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruckPhysicsSpeedTest,
    "TruckLogisticsSimulator.Vehicle.SpeedClamping",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FTruckPhysicsSpeedTest::RunTest(const FString& Parameters)
{
    ATruckPawn* Truck = NewObject<ATruckPawn>();
    Truck->AccelerateTruck(1.0f); // Max throttle
    
    // Truck should not exceed max speed
    TestTrue("Speed clamped to max", Truck->GetCurrentSpeed() <= 100.0f);
    
    return true;
}
```

Run tests: **Window → Developer Tools → Automation**

### Manual Testing Checklist

Before committing:
- [ ] Feature works as designed
- [ ] No new console warnings
- [ ] Performance acceptable (no FPS dips)
- [ ] Tested on target hardware (or Editor in minimum quality)

---

## Deployment & Release

### Version Numbering (SemVer)

Format: `MAJOR.MINOR.PATCH`

- `0.1.0` — First Alpha
- `0.1.1` — Patch (bug fix)
- `0.2.0` — Minor update (new feature, backward compatible)
- `1.0.0` — Full release

### Release Branch

Before release, create a release branch:

```bash
git checkout -b release/v0.1.0 develop
# Update version numbers, changelog
git commit -m "release: v0.1.0-alpha"
git push origin release/v0.1.0

# After approval:
git checkout main
git merge release/v0.1.0 --no-ff
git tag -a v0.1.0-alpha -m "Alpha Release"
git push origin main
git push origin v0.1.0-alpha
```

---

## PR Template

When creating a pull request, use this template:

```markdown
## Description
Brief summary of changes.

## Related Issues
Closes #123

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation

## Testing
Describe how to test the changes.

## Performance Impact
Any expected performance changes?

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex logic
- [ ] No debug logging left
- [ ] Performance acceptable
```

---

## Community Guidelines

- **Be Respectful**: Constructive feedback only
- **Search Issues First**: Avoid duplicate reports
- **Clear Descriptions**: Include steps to reproduce bugs
- **Minimal Reproduction**: Provide simplest example of issue
- **Attribution**: Credit other contributors

---

## Getting Help

- **Questions**: GitHub Discussions or Issues
- **Bug Reports**: GitHub Issues with reproduction steps
- **Feature Requests**: GitHub Discussions → Ideas

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-06-20 | Initial contribution guidelines |

