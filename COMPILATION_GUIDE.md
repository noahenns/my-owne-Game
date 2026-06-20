# TruckLogisticsSimulator — Month 1 Prototype Compilation Guide

**Status**: Month 1 Beta Prototype (Weeks 1-4 Complete)  
**Build Date**: 2026-06-20  
**Engine**: Unreal Engine 5.3+

---

## What's Implemented

### ✅ Core Systems
- **Vehicle Physics**: Full steering, acceleration, braking with realistic physics tuning
- **Truck Pawn**: Player-controlled vehicle with camera follow
- **Route Manager**: 3 delivery routes with waypoints (Urban↔Port, Warehouse↔Construction, Harbor↔Factory)
- **Cargo System**: Load/unload mechanics, damage tracking, reward calculation
- **Game Mode**: Player progression (money, XP, levels, unlocks)
- **HUD**: Speed, fuel, money, level, route information display

### ✅ Gameplay Features
- W/A/S/D keyboard controls (+ gamepad support ready)
- Fuel consumption based on distance
- Cargo loading/unloading with animation timer
- 10 km - 25 km varied delivery routes
- Real-time damage tracking for fragile cargo
- Hazmat routes with time-sensitive contracts (+50% rewards)
- Dynamic leveling system (unlock trailers at Level 5, premium routes at Level 10)

---

## Compilation Steps

### Prerequisites
- **Unreal Engine 5.3 or 5.4** (free from Epic Games Launcher)
- **Visual Studio 2022 Community** (Windows)
- **Xcode 14+** (macOS) or **GCC 11+** (Linux)
- **Git** (for version control)
- **20+ GB free disk space**

### Windows Build

#### Step 1: Generate Project Files
```bash
cd "c:\Users\[YourUsername]\Documents\GitHub\RealCarSim"
# Right-click TruckLogisticsSimulator.uproject → Generate Visual Studio project files
# OR run from command line:
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat"
```

#### Step 2: Compile in Visual Studio
```bash
# Open TruckLogisticsSimulator.sln
# Build → Rebuild Solution (Ctrl+Shift+B)
# Select "Development Editor" configuration for fastest iteration
# Compilation time: ~30 minutes (first build), ~5 minutes (incremental)
```

#### Step 3: Launch Editor
```bash
# Debug → Start Debugging (F5)
# OR double-click TruckLogisticsSimulator.uproject
# Editor will launch and open the default map
```

### Linux/macOS Build

```bash
cd ~/Documents/GitHub/RealCarSim

# Generate project
./GenerateProjectFiles.sh

# Compile (requires UE5.3 installed)
# On macOS:
/Users/[You]/Epic\ Games/UE_5.3/Engine/Build/Mac/BuildWithXcode.sh TruckLogisticsSimulator Mac Development

# On Linux:
/home/[You]/Epic\ Games/UE_5.3/Engine/Build/Linux/Build.sh TruckLogisticsSimulator Linux Development

# Launch editor
open TruckLogisticsSimulator.uproject  # macOS
# or UE5Editor TruckLogisticsSimulator.uproject  # Linux
```

---

## Editor Setup (First Launch)

When the editor opens, you may see some warnings. Here's what to do:

### 1. **Create Default Map**
- **File → New Level**
- **Landscape**: Add terrain (optional for MVP, we'll use simple plane)
- **Save As**: `Content/Maps/L_MainGame`

### 2. **Set Game Mode**
- **Edit → Project Settings → Maps & Modes**
- Set "Default GameMode" to `BP_GameMode_Main` (create this Blueprint)
- Set "Default Player Pawn Class" to `ATruckPawn`
- Set "Default HUD Class" to `ATLS_HUD`

### 3. **Set Default Map**
- **Edit → Project Settings → Maps & Modes**
- "Server Default Map" → `L_MainGame`
- "Default Map" → `L_MainGame`

### 4. **Create GameMode Blueprint**
- **Right-click in Content → Blueprint Class → GameMode**
- Base Class: `ATLS_GameMode_Main`
- Name: `BP_GameMode_Main`
- **Edit Settings**:
  - Default Pawn Class: `ATruckPawn`
  - HUD Class: `ATLS_HUD`
- **Compile & Save**

### 5. **Place Truck in Level**
- Drag `BP_TruckPawn` (or spawn `ATruckPawn`) into level
- Position at (5000, 5000, 0) to start
- Select as "Player Start" (optional)

### 6. **Add Landscape or Terrain**
- **Landscape → Create** (if you want terrain)
- OR use simple `Ground Plane` prop
- Ensure truck doesn't fall through world

---

## First Playtest

### Launch Game
1. **Click "Play" button in Editor** (or Ctrl+Alt+P)
2. **Truck will spawn at player start location**

### Test Controls
| Key | Action |
|-----|--------|
| `W` | Accelerate forward |
| `S` | Brake/reverse |
| `A` | Steer left |
| `D` | Steer right |
| `Esc` | Pause game |

### Expected Behavior
- Truck accelerates smoothly to ~100 km/h max speed
- Steering is responsive but not instant (realistic)
- Fuel gauge decreases as you drive
- HUD displays speed, fuel, money, level in corners

---

## Troubleshooting Compilation Errors

### Error: "UnknownAPI" or undefined symbols
**Solution**: Delete `Intermediate/` and `Binaries/` folders, regenerate project files

### Error: "Missing include files"
**Solution**: Check that all header files are in correct `Public/` folders:
- `Public/Core/GameTypes.h`
- `Public/Gameplay/Vehicle/VehiclePhysicsComponent.h`
- `Public/Gameplay/Vehicle/TruckPawn.h`
- `Public/Gameplay/Route/RouteManager.h`
- `Public/Gameplay/Cargo/CargoSystem.h`
- `Public/Core/TLS_GameMode_Main.h`
- `Public/UI/TLS_HUD.h`

### Error: "LNK2001 unresolved external symbol"
**Solution**: Ensure implementation `.cpp` files exist in `Private/` folders matching headers

### Error: "Assertion failed in Unreal Insights"
**Solution**: This often happens on first launch. Ignore and continue. Close Insights dialog.

---

## Performance Notes

### Target Specifications
- **Frame Rate**: 60 FPS sustained
- **Peak Memory**: < 8 GB
- **Load Time**: < 3 seconds

### Optimization Tips
- Run in "Development Editor" mode (faster than "Debug")
- Set viewport quality to "High" or lower if performance dips
- Use **Ctrl+Shift+A** to profile with Unreal Insights

---

## Next Steps After Compilation

### Phase 1 Verification (Weeks 1-4 Complete)
- [ ] Truck physics responsive and stable
- [ ] Can drive 10+ minutes without crashes
- [ ] Fuel system working (depletes with distance)
- [ ] HUD displays correct information
- [ ] Route manager loads 3 routes

### Phase 2 Next (Weeks 5-8)
- Cargo loading/unloading animation
- Deliver cargo to destination
- Reward screen (money/XP gained)
- Save/load persistence
- Main menu UI

### Phase 3 Next (Weeks 9-12)
- Cross-platform CI/CD builds
- Release automation
- Multi-platform packaging (Windows/Linux/macOS)

---

## Tips for Development

### Fast Iteration
- Make code changes
- **Ctrl+Shift+B** (compile in VS)
- **Ctrl+Alt+P** (play in editor)
- Changes apply without restarting editor

### Hot Reload
- Make code changes
- **Compile** in Visual Studio
- **Ctrl+Shift+R** in Editor to hot reload
- Most changes apply instantly

### Blueprint Iteration
- Create Blueprint derivatives of C++ classes
- Tweak values without recompiling
- **Right-click C++ Class → Create Blueprint Based on This**

---

## File Structure Reference

```
Source/
├── TruckLogisticsSimulator/
│   ├── Public/
│   │   ├── Core/
│   │   │   ├── GameTypes.h (data structures)
│   │   │   └── TLS_GameMode_Main.h
│   │   ├── Gameplay/
│   │   │   ├── Vehicle/
│   │   │   │   ├── TruckPawn.h (main player vehicle)
│   │   │   │   └── VehiclePhysicsComponent.h (physics simulation)
│   │   │   ├── Route/
│   │   │   │   └── RouteManager.h (route management)
│   │   │   └── Cargo/
│   │   │       └── CargoSystem.h (cargo handling)
│   │   └── UI/
│   │       └── TLS_HUD.h (on-screen display)
│   └── Private/
│       └── (implementation .cpp files)

Content/
├── Maps/
│   └── L_MainGame.umap (main gameplay level)
├── Blueprints/
│   ├── BP_GameMode_Main
│   └── BP_TruckPawn
└── Assets/ (meshes, materials, audio - TBD)
```

---

## Building for Distribution (Week 10+)

After testing and iterating on the prototype:

```bash
# Package for shipping
Unreal → Packaging Settings
File → Package Project → Windows (Shipping)

# This creates a standalone .exe that can run without the editor
# Output: Packaged/WindowsNoEditor/TruckSimulator.exe
```

---

## Questions or Issues?

- Check **DEVELOPMENT.md** for Git and branching workflow
- Review **ARCHITECTURE.md** for system design details
- See **CONTRIBUTING.md** for code standards

---

**Estimated Time to First Playable Build**: 45 minutes (including compilation)  
**Next Major Milestone**: Playable delivery route completion (Week 4)

Good luck! 🚚

