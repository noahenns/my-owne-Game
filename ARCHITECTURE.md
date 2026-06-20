# TruckLogisticsSimulator — Architecture Document

## Overview

**TruckLogisticsSimulator** (MVP) is a single-player truck logistics game built on **Unreal Engine 5.3+**. The architecture prioritizes:
- **Platform Independence**: C++ abstractions for Windows, Linux, macOS
- **Modularity**: Decoupled gameplay systems (Vehicle, Cargo, Routes, Economy, UI)
- **Performance**: Target 60 FPS on mid-range hardware (8GB RAM, GTX 1050+)
- **Extensibility**: Blueprint-friendly architecture for rapid iteration on content

---

## System Architecture

### Core Module: `TruckLogisticsSimulator`

**Purpose**: Main game module containing all C++ gameplay logic.

**Submodules**:

#### 1. **Vehicle System** (`Public/Gameplay/Vehicle/`)

**Classes**:
- `ATruckPawn` — Drivable vehicle pawn (inherits `APawn`)
  - Steering input handling (keyboard + gamepad)
  - Acceleration/braking physics
  - Collision detection with terrain
  - Camera follow (third-person over-shoulder)
  - **Physics Constants**:
    - `MaxSpeed = 100 km/h`
    - `AccelerationRate = 5.0 units/sec²`
    - `BrakingDeceleration = 8.0 units/sec²`
    - `TurningSpeed = 0.04 radians/sec`
    - `TireGrip = 0.95` (reduces to 0.7 on wet surfaces)

- `UVehicleInputComponent` — Input abstraction
  - Keyboard/Gamepad unified input mapping
  - Cross-platform input polling (Windows, Linux, macOS)

- `UVehiclePhysicsComponent` — Physics simulation
  - Wheel-ground contact tracing
  - Speed clamping and drag
  - Turning radius calculation
  - Braking distance formula: `distance = v² / (2 * deceleration)`

**Dependencies**: Unreal Engine Chaos Physics

---

#### 2. **Cargo System** (`Public/Gameplay/Cargo/`)

**Classes**:
- `UCargoType` — Defines cargo properties (enum-based)
  - `Fragile` — Takes 1.5x damage on collisions, 1.2x reward
  - `Hazmat` — Time-sensitive (delivery window), 1.5x reward
  - `Bulk` — Heavy, reduces truck max speed by 20%, 1.0x reward

- `FCargoInstance` — Runtime cargo data (struct)
  ```cpp
  struct FCargoInstance {
    UCargoType CargoType;
    float Weight;              // kg
    float CurrentDamage;       // 0-100%
    bool bIsLoaded;
    FDateTime DeliveryDeadline; // Hazmat only
  };
  ```

- `UCargoSystem` — Manages cargo state machine
  - States: `Empty → LoadingInProgress → Loaded → Delivering → Delivered`
  - `LoadCargo()` — Plays loading animation (2 sec), disables movement
  - `UnloadCargo()` — Validates delivery location, calculates rewards
  - `ApplyCollisionDamage(float ImpactForce)` — Damages fragile cargo

---

#### 3. **Route System** (`Public/Gameplay/Route/`)

**Classes**:
- `FDeliveryRoute` — Route definition (struct)
  ```cpp
  struct FDeliveryRoute {
    FString RouteName;
    FVector StartLocation;
    FVector EndLocation;
    float DistanceKm;
    ECargoType RequiredCargoType;
    float BaseReward;          // Currency
    TArray<FVector> Waypoints;
  };
  ```

- `ARouteManager` — Singleton manager
  - Maintains route database (10 hardcoded routes)
  - Generates random deliveries
  - Tracks active route progress
  - **MVP Routes**:
    - Urban Depot ↔ Port (12 km, $450 base)
    - Warehouse ↔ Construction Site (18 km, $600 base)
    - Harbor ↔ Factory (25 km, $800 base)
    - (7 additional regional routes)

- `URouteProgress` — Per-route tracking
  - `CurrentWaypoint` index
  - `DistanceTraveled` / `EstimatedDistance`
  - `TimeRemaining` for hazmat
  - `CompletionPercentage`

**Dependencies**: `UCargoSystem`

---

#### 4. **Economy System** (`Public/Gameplay/Economy/`)

**Classes**:
- `FPlayerProgression` — Player state persistence (struct)
  ```cpp
  struct FPlayerProgression {
    float TotalMoney;
    int32 TotalXP;
    int32 CompletedDeliveries;
    int32 CurrentLevel;        // 1 + (TotalXP / 100)
    int32 UnlockedTrailerAxles; // 2 or 3
  };
  ```

- `UEconomySystem` — Reward calculation
  - **Delivery Reward Formula**:
    ```
    base_reward = route.BaseReward
    cargo_multiplier = CargoType.Multiplier
    distance_bonus = distance_km * 10  // $10 per km
    fuel_cost = distance_km * 2.5      // $2.50 per km
    total_reward = (base_reward + distance_bonus) * cargo_multiplier - fuel_cost
    ```
  - **XP Formula**: `XP_earned = distance_km * 1.0 XP/km + completion_bonus(50 XP)`
  - **Level Unlock Thresholds**:
    - Level 5: 3-Axle Trailer
    - Level 10: Premium Routes
    - Level 20: Custom Liveries (future)

- `UProgression` — Manages leveling and unlocks
  - Auto-save on delivery
  - Tracks achievement milestones

**Dependencies**: `UCargoSystem`, `URouteManager`

---

#### 5. **Persistence System** (`Public/Gameplay/Persistence/`)

**Classes**:
- `USaveGameManager` — Handles save/load operations
  - **Save Location**: `Saved/SaveGames/PlayerData.json`
  - **Save Data Structure**:
    ```json
    {
      "PlayerProgression": { /* FPlayerProgression */ },
      "CurrentRoute": { /* FDeliveryRoute */ },
      "VehicleState": {
        "Position": [x, y, z],
        "Rotation": angle,
        "Speed": velocity,
        "CurrentCargo": { /* FCargoInstance */ }
      },
      "GameTime": { "Hour": 14, "Day": 3 },
      "LastSaveTime": "2026-06-20 14:30:00"
    }
    ```
  - Auto-save every 5 minutes + on delivery completion
  - Load on game start ("Continue" button)

- `UTimeManager` — In-game time system
  - 1 in-game minute = 2 real seconds (configurable)
  - Day/night cycle affects lighting
  - Time affects route urgency (hazmat deadlines)

**Dependencies**: Platform I/O abstractions

---

#### 6. **UI System** (`Public/Gameplay/UI/` → Blueprint-driven)

**Core HUD**:
- `AHUD_Gameplay` (C++ class)
  - Vehicle speed gauge (0–120 km/h)
  - Fuel indicator (0–100%)
  - Destination compass + distance remaining
  - Cargo weight indicator
  - Minimap widget showing player position + waypoints

**Main Menu**:
- `WBP_MainMenu` (UMG Widget Blueprint)
  - New Game, Continue, Settings, Quit buttons
  - Difficulty selection (affects rewards/penalties)

**Delivery UI**:
- `WBP_RewardScreen` (Blueprint)
  - Displays: distance traveled, cargo type, money earned, XP gained
  - Animated counter for money

**Settings Menu**:
- Graphics: resolution, quality preset, V-sync
- Audio: master volume, engine volume, UI volume
- Input: controller sensitivity, invert axis

---

### Module Architecture Diagram

```
TruckLogisticsSimulator (Main Module)
├── Vehicle System
│   ├── ATruckPawn
│   ├── UVehicleInputComponent
│   └── UVehiclePhysicsComponent
├── Cargo System
│   ├── UCargoType
│   └── UCargoSystem
├── Route System
│   ├── FDeliveryRoute
│   ├── ARouteManager (Singleton)
│   └── URouteProgress
├── Economy System
│   ├── UEconomySystem
│   └── UProgression
├── Persistence System
│   ├── USaveGameManager
│   └── UTimeManager
└── UI System (Blueprints)
    ├── AHUD_Gameplay
    ├── WBP_MainMenu
    ├── WBP_RewardScreen
    └── WBP_SettingsMenu
```

**Dependencies Flow**:
```
ATruckPawn (Vehicle)
  ↓
UCargoSystem (Cargo)
  ↓
ARouteManager (Routes)
  ↓
UEconomySystem (Economy) → UProgression → USaveGameManager
  ↓
UTimeManager (Persistence)
```

---

## Platform Abstraction

### Cross-Platform Considerations

**File I/O**:
- Use `FPlatformFile::GetPlatformFile()` for all file operations
- Path separators handled by Unreal Engine (`FPaths::` utility functions)
- `Saved/SaveGames/` directory created automatically per-platform

**Input Handling**:
- Gamepad API abstraction in `UVehicleInputComponent`
- Button mapping unified across platforms (Xbox-style controller assumed)
- Keyboard fallback always available

**Audio**:
- OpenAL Core (Unreal default) handles platform audio
- No platform-specific audio codec handling needed

**Rendering**:
- Direct3D 12 (Windows), Vulkan (Linux), Metal (macOS)
- Unreal Engine abstracts graphics API; no platform-specific renderer calls

---

## Physics Tuning Reference

**Tire Friction Model** (simplified):
```cpp
float CalculateTireFriction(float speed, float terrainWetness) {
  float baseFriction = 0.95f;
  float speedDamping = FMath::Clamp(speed / MaxSpeed, 0.0f, 1.0f);
  float weatherModifier = terrainWetness > 0.5f ? 0.7f : 1.0f;
  return baseFriction * speedDamping * weatherModifier;
}
```

**Braking Distance** (real-world formula):
```
distance_m = (speed_km_h / 3.6)² / (2 * deceleration_m_s²)
Example: 50 km/h, 8 m/s² deceleration = 12 m braking distance
```

---

## Performance Targets

- **Frame Rate**: 60 FPS sustained (minimum 55 FPS)
- **Peak Memory**: < 8 GB (target 6 GB)
- **Draw Calls**: < 500 per frame (simple geometry)
- **Disk Space**: < 5 GB per platform
- **Load Time**: < 3 seconds (route load)
- **Save Time**: < 500 ms (JSON serialization)

---

## Dependency Management

**Core Dependencies** (Unreal Engine built-in):
- `Core`, `Engine`, `InputCore`, `Chaos`
- `UMG` (UI framework)
- `Json` (save file serialization)

**Third-Party** (future, not in MVP):
- Steam integration (post-launch)
- Audio middleware (Wwise, FMOD — future)
- Versioning/auto-update system (future)

---

## Future Extensibility

**Plugin Architecture** (Post-MVP):
- `LogisticsSystem` plugin (modular cargo/route systems)
- `DynamicWeather` plugin (weather physics)
- `MultiplayerCoastal` plugin (convoy mode)

**Content Streams** (DLC):
- Custom trucks + liveries
- New regions (Europe, Asia)
- Time-limited events (seasonal)

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-06-20 | Initial architecture for MVP (v0.1.0-alpha) |

