# TruckLogisticsSimulator — Development Setup Guide

## Quick Start

**Prerequisites**:
- Windows 10+, Linux (Ubuntu 22.04+), or macOS 11.0+
- Unreal Engine 5.3+ (free, open-source)
- Git
- 20 GB free disk space (engine + project)

### Installation (Windows)

```bash
# 1. Clone repository
git clone https://github.com/[owner]/TruckLogisticsSimulator.git
cd TruckLogisticsSimulator

# 2. Generate Visual Studio solution
./GenerateProjectFiles.bat

# 3. Open solution in Visual Studio 2022
start TruckLogisticsSimulator.sln

# 4. Compile project (Ctrl+Shift+B)
# → Builds in Debug or Development configuration

# 5. Open in Unreal Editor
# Method A: From VS, right-click TruckLogisticsSimulator → Set as Startup Project → F5
# Method B: Double-click TruckLogisticsSimulator.uproject
```

### Installation (Linux/macOS)

```bash
# 1. Clone repository
git clone https://github.com/[owner]/TruckLogisticsSimulator.git
cd TruckLogisticsSimulator

# 2. Generate project files
chmod +x GenerateProjectFiles.sh
./GenerateProjectFiles.sh

# 3. Build (if using custom build script)
./Build.sh linux shipping
# or macOS:
./Build.sh mac shipping

# 4. Open in Unreal Editor
# Requires Epic Games Launcher installed with UE5.3
open TruckLogisticsSimulator.uproject  # macOS
# or: xdg-open TruckLogisticsSimulator.uproject  # Linux
```

---

## Project Structure

```
TruckLogisticsSimulator/
├── Binaries/                    # Compiled executables (gitignored)
│   ├── Win64/
│   ├── Linux/
│   └── Mac/
├── Intermediate/                # Build intermediate files (gitignored)
├── Saved/                       # Runtime data, logs, saves (gitignored)
│   ├── Logs/
│   ├── SaveGames/
│   └── Config/
├── Source/
│   ├── TruckLogisticsSimulator/
│   │   ├── Public/
│   │   │   ├── TruckLogisticsSimulator.h
│   │   │   ├── Gameplay/
│   │   │   │   ├── Vehicle/
│   │   │   │   │   ├── TruckPawn.h
│   │   │   │   │   └── VehicleInputComponent.h
│   │   │   │   ├── Route/
│   │   │   │   │   ├── DeliveryRoute.h
│   │   │   │   │   └── RouteManager.h
│   │   │   │   ├── Cargo/
│   │   │   │   │   ├── CargoSystem.h
│   │   │   │   │   └── CargoType.h
│   │   │   │   ├── Economy/
│   │   │   │   │   ├── EconomySystem.h
│   │   │   │   │   └── Progression.h
│   │   │   │   ├── Persistence/
│   │   │   │   │   ├── SaveGameManager.h
│   │   │   │   │   └── TimeManager.h
│   │   │   │   └── UI/
│   │   │   │       ├── HUD_Gameplay.h
│   │   │   │       └── [Blueprint widgets]
│   │   │   └── Core/
│   │   │       ├── GameMode_Main.h
│   │   │       └── PlayerController_Main.h
│   │   ├── Private/
│   │   │   ├── TruckLogisticsSimulator.cpp
│   │   │   ├── Gameplay/
│   │   │   │   ├── Vehicle/
│   │   │   │   │   ├── TruckPawn.cpp
│   │   │   │   │   └── VehicleInputComponent.cpp
│   │   │   │   ├── Route/
│   │   │   │   │   ├── DeliveryRoute.cpp
│   │   │   │   │   └── RouteManager.cpp
│   │   │   │   ├── Cargo/
│   │   │   │   │   ├── CargoSystem.cpp
│   │   │   │   │   └── CargoType.cpp
│   │   │   │   ├── Economy/
│   │   │   │   │   ├── EconomySystem.cpp
│   │   │   │   │   └── Progression.cpp
│   │   │   │   ├── Persistence/
│   │   │   │   │   ├── SaveGameManager.cpp
│   │   │   │   │   └── TimeManager.cpp
│   │   │   │   └── UI/
│   │   │   │       └── HUD_Gameplay.cpp
│   │   │   └── Core/
│   │   │       ├── GameMode_Main.cpp
│   │   │       └── PlayerController_Main.cpp
│   │   └── TruckLogisticsSimulator.Build.cs
│   └── TruckLogisticsSimulatorEditor.Target.cs
├── Content/
│   ├── Assets/
│   │   ├── Vehicles/
│   │   │   ├── Truck_Base/
│   │   │   │   ├── Meshes/
│   │   │   │   │   └── SK_Truck_Body
│   │   │   │   ├── Materials/
│   │   │   │   │   └── M_TruckPaint
│   │   │   │   ├── Skeletons/
│   │   │   │   └── Animations/
│   │   │   └── Trailers/
│   │   │       ├── Meshes/
│   │   │       └── Materials/
│   │   ├── Environments/
│   │   │   ├── Maps/
│   │   │   │   ├── L_MainMenu
│   │   │   │   ├── L_Prototype
│   │   │   │   └── L_Sandbox_20km
│   │   │   ├── Landscape/
│   │   │   │   └── [Heightmaps, materials]
│   │   │   └── Props/
│   │   │       ├── Depots/
│   │   │       ├── Roads/
│   │   │       └── Vegetation/
│   │   ├── UI/
│   │   │   ├── Widgets/
│   │   │   │   ├── WBP_MainMenu
│   │   │   │   ├── WBP_HUD_Gameplay
│   │   │   │   ├── WBP_RewardScreen
│   │   │   │   └── WBP_Settings
│   │   │   ├── Textures/
│   │   │   ├── Materials/
│   │   │   └── Fonts/
│   │   ├── Audio/
│   │   │   ├── SFX/
│   │   │   │   ├── Engine/
│   │   │   │   ├── Cargo/
│   │   │   │   ├── UI/
│   │   │   │   └── Ambient/
│   │   │   └── Music/
│   │   └── Animations/
│   ├── Blueprints/
│   │   ├── Character/
│   │   ├── Pawns/
│   │   ├── Systems/
│   │   └── Levels/
│   └── Data/
│       ├── DataTables/
│       │   ├── DT_Routes.csv
│       │   ├── DT_CargoTypes.csv
│       │   └── DT_Vehicles.csv
│       ├── Routes/
│       ├── Cargo/
│       └── Vehicles/
├── Plugins/
│   ├── Marketplace/              # Third-party marketplace plugins (gitignored)
│   └── Custom/
│       └── LogisticsSystem/       # Custom game plugins (future)
├── .github/
│   └── workflows/
│       ├── build.yml             # Main CI/CD pipeline
│       └── release.yml           # Release automation
├── docs/
│   ├── ARCHITECTURE.md           # System design (see ARCHITECTURE.md)
│   ├── PHYSICS.md                # Physics tuning reference
│   ├── ROUTES.md                 # Route design guidelines
│   ├── AUDIO.md                  # Audio implementation guide
│   └── DEPLOYMENT.md             # Build and release procedures
├── scripts/
│   ├── build.py                  # Cross-platform build wrapper
│   ├── package.py                # Packaging script
│   └── generate_changelog.py      # Changelog generation
├── .gitignore                    # Unreal Engine-specific ignores
├── TruckLogisticsSimulator.uproject  # Project file
├── GenerateProjectFiles.bat      # Windows project generation (auto-generated)
├── GenerateProjectFiles.sh       # Unix project generation (auto-generated)
├── README.md                     # User-facing project overview
├── ROADMAP.md                    # Development roadmap
├── DEVELOPMENT.md                # This file
├── CONTRIBUTING.md               # Code standards and contribution guidelines
└── LICENSE                       # Project license

```

---

## Development Workflow

### Branch Strategy (Git Flow)

```
main (stable, release-ready)
  ↑
develop (integration branch)
  ↑
  ├─ feature/truck-physics
  ├─ feature/delivery-routes
  ├─ feature/cargo-system
  ├─ feature/ui-hud
  ├─ fix/physics-instability
  └─ hotfix/critical-crash
```

**Branch Rules**:
- `main`: Protected branch, requires PR with 1 approval before merge
- `develop`: Integration branch, direct commits allowed (use PRs for visibility)
- `feature/*`: Created from `develop`, PR back to `develop`
- `fix/*`: Bug fixes, PR to `develop`
- `hotfix/*`: Critical post-release fixes, PR to both `main` and `develop`

### Creating a Feature Branch

```bash
# 1. Update develop
git checkout develop
git pull origin develop

# 2. Create feature branch
git checkout -b feature/vehicle-physics

# 3. Make commits (see Commit Policy below)
git add .
git commit -m "feat(vehicle): implement steering mechanics"

# 4. Push to GitHub
git push origin feature/vehicle-physics

# 5. Create Pull Request on GitHub
# → GitHub: Create PR, add description, assign reviewer
# → Link to GitHub Issue if applicable

# 6. After approval, merge via GitHub UI
# → Delete branch after merge
```

### Commit Policy

**Make frequent commits** — one commit per completed task or milestone.

**Commit Message Format**:
```
<type>(<scope>): <subject>

<body>

Fixes #<issue-number>
Relates-To: <other-issue-number>
```

**Types**:
- `feat` — New feature
- `fix` — Bug fix
- `refactor` — Code refactoring (no feature change)
- `docs` — Documentation
- `style` — Code style (formatting, missing semicolons)
- `test` — Test additions or fixes
- `perf` — Performance improvement
- `build` — Build system or dependency change
- `ci` — CI/CD changes
- `chore` — Maintenance tasks

**Scope** (from module name):
- `vehicle` — Truck physics system
- `cargo` — Cargo loading/unloading
- `route` — Delivery routes
- `economy` — Economy and progression
- `ui` — User interface
- `persistence` — Save/load system
- `time` — Time management
- `audio` — Sound system
- `physics` — Physics engine
- `build` — Build/packaging
- `ci` — CI/CD automation

**Examples**:
```
feat(vehicle): implement truck steering with input smoothing

Add steering input with deadzone threshold (0.08) and ramping
to simulate realistic vehicle response. Supports both keyboard
and gamepad input.

Fixes #12

---

fix(cargo): resolve cargo clipping through truck body

Use swept collision for cargo placement to prevent overlap
with truck geometry.

Relates-To: #45

---

refactor(physics): consolidate friction calculations

Merge `CalculateTireFriction` and `GetWheelGrip` into single
function for maintainability.

---

docs(readme): add Linux build instructions

---

perf(hud): optimize minimap rendering

Reduce minimap draw calls from 12 to 3 by batching updates.
```

---

## Code Review Checklist

**Before merging a PR, verify**:

- [ ] Code compiles without warnings or errors
- [ ] Follows naming conventions (see CONTRIBUTING.md)
- [ ] No breaking changes to existing systems
- [ ] Performance benchmarks acceptable (< 3% FPS impact)
- [ ] Comments on complex logic (physics, algorithms)
- [ ] No debug logging left in code (or marked with `TODO: Remove`)
- [ ] Tests pass (if applicable)
- [ ] Asset files organized per naming conventions

---

## Testing & Profiling

### Local Testing Checklist

Before pushing a branch:

1. **Compile Check**:
   ```bash
   Ctrl+Shift+B  # Visual Studio: Build Solution
   ```

2. **Editor Playtest**:
   - Open map in Editor
   - Play for 5 minutes
   - Check console (Window → Developer Tools → Output Log) for warnings/errors
   - Verify expected behavior works

3. **Packaging Test** (before release):
   ```bash
   Unreal → Packaging Settings → Show Output Log
   File → Package Project → Windows → Save
   # Test packaged .exe on clean machine
   ```

### Performance Profiling

**Unreal Insights** (built-in profiler):

```
Ctrl+Shift+A  # Open Insights
# Captures frame data, memory usage, GPU costs
# Look for: FPS dips, memory leaks, draw call spikes
```

**Target Performance**:
- FPS: 60 sustained (minimum 55)
- Memory: < 8 GB peak
- Load time: < 3 seconds (route load)
- Save time: < 500 ms

---

## CI/CD Pipeline

### GitHub Actions Workflows

**Automatic Build** (on push to `develop` or PR to `main`):
- Compiles for Windows (Visual Studio 2022)
- Compiles for Linux (Ubuntu 22.04)
- Compiles for macOS (latest Xcode)
- Reports any compilation errors in PR

**Manual Package** (tag-triggered):
- Push tag: `git tag -a v0.1.0-alpha && git push origin v0.1.0-alpha`
- GitHub Actions automatically:
  - Builds all 3 platforms
  - Packages binaries (`.zip`, `.tar.gz`)
  - Generates changelog
  - Creates GitHub Release
  - Uploads artifacts

### Monitoring CI

View build status: **Actions tab** on GitHub

If build fails:
1. Click failed workflow
2. View logs for error
3. Fix locally, commit, push
4. GitHub Actions auto-retries

---

## Environment Variables & Configuration

### Editor Preferences

1. **Edit → Editor Preferences**:
   - Source Code Editor: Visual Studio 2022
   - Enable Real-Time Rendering
   - Live Coding: Enabled

2. **Project Settings**:
   - Edit → Project Settings:
     - Engine → Physics → Use Chaos Dynamics
     - Engine → Rendering → Default Materials
     - Input: Map keys (WASD, arrow keys, gamepad)

### Build Configuration

**Development**: Fast compilation, debug symbols, used for iteration
```
Build Configuration → Development Editor
```

**Shipping**: Optimized, no debug symbols, used for releases
```
Build Configuration → Shipping
```

---

## Common Issues & Solutions

### Issue: Project won't compile

**Solution**:
1. Delete `Intermediate/` and `Binaries/` folders
2. Regenerate project files: `GenerateProjectFiles.bat`
3. Recompile

### Issue: Git won't pull (Unreal files causing conflicts)

**Solution** — Use `.gitattributes` to merge binary files:
```
# Add to .gitattributes
*.umap binary
*.uasset binary
*.png binary
*.jpg binary
```

### Issue: Crashes on Linux/macOS build

**Solution**:
1. Check console logs: `Saved/Logs/[project]-latest.log`
2. Look for platform-specific errors
3. Verify file paths use `/` not `\`
4. Test cross-platform code paths

### Issue: FPS drops to 30 FPS

**Solution**:
1. Profile with Unreal Insights (Ctrl+Shift+A)
2. Identify bottleneck (CPU, GPU, I/O)
3. Optimize top cost function
4. Re-profile to verify improvement

---

## Tools & Extensions

### Recommended VS Code Extensions (if editing Blueprints/assets)
- Unreal Engine Marketplace Extension
- C++ IntelliSense

### Recommended Visual Studio Extensions
- Unreal Engine 5 Editing Utilities
- Coding Guidelines Checker

---

## Documentation Update Guidelines

When making code changes:

1. **Update ARCHITECTURE.md** if system design changes
2. **Update ROADMAP.md** if timelines shift
3. **Add comments** to non-obvious code
4. **Create docs/SYSTEM_NAME.md** for new major system

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-06-20 | Initial development setup guide |

