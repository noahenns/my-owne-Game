# Quick Start Guide — Month 1 Prototype

**Fastest path to playable game: ~2 hours**

---

## 1️⃣ Prerequisites (30 minutes)

### Download & Install
- [ ] **Unreal Engine 5.3+** (free): https://unrealengine.com
- [ ] **Visual Studio 2022 Community** (free): https://visualstudio.com
- [ ] **Git**: https://git-scm.com

### Verify Installation
```bash
# Check UE5 is installed
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" -help

# Check Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" -help

# Check Git
git --version
```

---

## 2️⃣ Clone & Setup (15 minutes)

```bash
# Navigate to your projects folder
cd C:\Users\YourName\Documents\GitHub

# Clone this repo (or use existing)
git clone https://github.com/[owner]/RealCarSim.git
cd RealCarSim

# Generate Unreal project files
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat"

# Verify folder structure
dir Source\TruckLogisticsSimulator  # Should show Public/ Private/ .Build.cs
```

---

## 3️⃣ Compile (30-45 minutes)

### Visual Studio
```bash
# Open Visual Studio solution
start TruckLogisticsSimulator.sln

# In Visual Studio:
# - Select "Development Editor" from config dropdown
# - Build → Rebuild Solution (Ctrl+Shift+B)
# - Wait for compilation (first run ~30 min, incremental ~5 min)
```

### Or Command Line
```bash
# Compile via command line
cd TruckLogisticsSimulator

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
  TruckLogisticsSimulator.sln ^
  /t:Build ^
  /p:Configuration="Development Editor" ^
  /p:Platform=Win64
```

### Verify Compilation
```
✅ Should see: "Build succeeded"
✅ Folder: Binaries\Win64\UE4Editor-TruckLogisticsSimulator.dll should exist
❌ If errors: Delete Intermediate/, Binaries/, regenerate
```

---

## 4️⃣ Launch Editor (5 minutes)

### Method 1: Direct
```bash
double-click TruckLogisticsSimulator.uproject
# Editor will launch automatically
```

### Method 2: Command Line
```bash
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" ^
  "C:\Path\To\TruckLogisticsSimulator\TruckLogisticsSimulator.uproject"
```

### First Launch Setup
When editor opens for first time:
1. **Wait for shader compilation** (~2 minutes)
2. **Accept any warnings** (click OK/ignore)
3. You should see empty editor view

---

## 5️⃣ Create Test Level (10 minutes)

### In Unreal Editor

1. **File → New Level**
   - Select "Empty Level"
   - Click OK

2. **Add Basic Ground**
   - **Place Actors** (left panel)
   - Search: "Ground Plane"
   - Click to place in level
   - Click again on ground, Escape to deselect

3. **Place Truck**
   - **Window → Content Browser**
   - Navigate: `Source/TruckLogisticsSimulator`
   - Look for `ATruckPawn` class
   - Drag into level
   - Position at (5000, 5000, 0) using Details panel

4. **Set Game Mode**
   - **World Settings** (top right dropdown)
   - Game Mode: Select `ATLS_GameMode_Main`
   - Leave other fields default

5. **Save Level**
   - Ctrl+S
   - Name: `L_TestDrive`
   - Location: `Content/Maps/`

---

## 6️⃣ First Playtest (5 minutes)

### Launch Game
```
Click "Play" button (bottom toolbar) or Ctrl+Alt+P
```

### Expected Results
✅ Truck appears in level  
✅ **W** key → truck moves forward  
✅ **A/D** keys → truck turns left/right  
✅ **S** key → truck brakes/reverses  
✅ HUD appears in top-left corner showing:
  - Speed (km/h)
  - Fuel (%)
  - Money ($)
  - Level

### Troubleshooting First Launch

| Issue | Solution |
|-------|----------|
| "No Truck visible" | Truck may be underground. Change spawnpoint Z to 200. |
| "Truck falls through ground" | Add a larger Ground Plane or landscape. |
| "HUD shows but truck won't move" | Check input bindings in Project Settings. |
| "Editor crashes" | Delete Binaries/ and Intermediate/, recompile. |
| "Slow framerate" | Lower viewport quality (top-right dropdown). |

---

## 7️⃣ Next: Load Real Game (5 minutes)

Once basic test works:

1. **Create Game Mode Blueprint**
   - Right-click in Content Browser
   - Blueprint Class → GameMode Base
   - Parent: `ATLS_GameMode_Main`
   - Name: `BP_GameMode_Main`
   - Compile & Save

2. **Set as Default**
   - Edit → Project Settings → Maps & Modes
   - Default GameMode Class: `BP_GameMode_Main`
   - Default Map: `L_TestDrive`

3. **Play Again**
   - Ctrl+Alt+P
   - Should show game mode, routes loading

---

## 📊 What Should Work

### Input/Driving
- ✅ Smooth truck acceleration (0-100 km/h)
- ✅ Realistic steering (not instant, has turn radius)
- ✅ Braking/reverse
- ✅ Fuel consumption (depletes as you drive)

### HUD Display
- ✅ Speed gauge updates in real-time
- ✅ Fuel bar decreases
- ✅ Money shows (starts at $1000)
- ✅ Level shows (starts at 0 XP)

### Physics
- ✅ Truck doesn't instantly teleport or jitter
- ✅ Gravity works (truck doesn't fly)
- ✅ Turning has momentum (realistic handling)
- ✅ No hard collisions with terrain

---

## 📖 Documentation

After first playtest, read:

1. **PROTOTYPE_SUMMARY.md** - What's been built
2. **ARCHITECTURE.md** - System design overview
3. **COMPILATION_GUIDE.md** - Detailed build setup
4. **CONTRIBUTING.md** - How to add features

---

## 🎯 Common Next Steps

### Want to add features?
```bash
# Create feature branch
git checkout -b feature/your-feature-name

# Make code changes in Source/
# Edit in Visual Studio
# Compile (Ctrl+Shift+B in VS)
# Test in Editor (Ctrl+Alt+P)

# Commit
git add -A
git commit -m "feat: your feature description"
```

### Want to tweak physics?
```
Edit: Source/TruckLogisticsSimulator/Private/Gameplay/Vehicle/VehiclePhysicsComponent.cpp

Look for:
- MaxSpeed = 30.0f;           // m/s
- AccelerationRate = 15.0f;   // m/s²
- BrakingDeceleration = 20.0f;

Change values, recompile (Ctrl+Shift+B), play (Ctrl+Alt+P)
```

### Want to add more routes?
```
Edit: Source/TruckLogisticsSimulator/Private/Gameplay/Route/RouteManager.cpp

In InitializeDefaultRoutes() function:
- Add FDeliveryRoute (name, location, distance, etc.)
- Function auto-creates waypoints
- Recompile and test
```

---

## ⏱️ Time Breakdown

| Step | Time | Cumulative |
|------|------|-----------|
| Install Prerequisites | 30 min | 30 min |
| Clone & Setup | 15 min | 45 min |
| Compile | 40 min | 85 min |
| Launch Editor | 5 min | 90 min |
| Create Test Level | 10 min | 100 min |
| First Playtest | 5 min | 105 min |

**Total: ~2 hours to playable game!**

---

## 🆘 Help & Support

### Compilation Issues?
→ See **COMPILATION_GUIDE.md** (Troubleshooting section)

### Gameplay Not Working?
→ Check **ARCHITECTURE.md** (System Overview)

### Want to Extend?
→ Follow **CONTRIBUTING.md** (Code Standards)

### Need to Debug?
→ Enable logging: Check `Saved/Logs/` after playtesting

---

## ✨ You're Ready!

You now have a **playable truck logistics simulator prototype** ready for testing, evaluation, and iteration.

**Next Phase**: Polish, optimize, and prepare for external testing/evaluation.

**Questions?** Check the documentation files or review source code comments.

Good luck! 🚚

