# TruckLogisticsSimulator

A professional cross-platform truck logistics simulator featuring dynamic economy systems, complex delivery routes, and persistent world state. Built on **Unreal Engine 5.3+**.

> **Status**: Beta Prototype Complete (v0.0.1 - Month 1 Delivery)  
> **Latest Build**: 2026-06-20  
> **Current Phase**: Weeks 1-4 Core Systems Complete  
> **Next Phase**: Testing & Optimization (Weeks 5-6)  
> **Target Release**: 3 months from start  
> **Platforms**: Windows, Linux, macOS

---

## Features

**Core Gameplay** (MVP, v0.1.0):
- 🚚 Drivable truck with realistic physics-based steering, acceleration, and braking
- 📦 Delivery contracts with 10+ routes across a 20 km² sandbox world
- 💰 Dynamic economy: earn money, manage fuel costs, unlock features via progression
- 📊 Experience & leveling system with gameplay-affecting unlocks
- 💾 Persistent save/load system with auto-save
- 🌙 Day/night cycle with time-based contracts
- 🎮 Full gamepad + keyboard support
- 🔊 Immersive audio (engine, cargo, ambient sounds)
- 🌍 Cross-platform (Windows, Linux, macOS)

**Post-Launch Features** (Roadmap):
- Custom truck liveries & paint jobs
- Extended map regions (Europe expansion)
- Dynamic weather physics
- Multiplayer convoy mode
- NPC driver hiring
- Truck customization & tuning

---

## Installation

### Windows

1. Download latest release from [GitHub Releases](https://github.com/[user]/TruckLogisticsSimulator/releases)
2. Extract `TruckSimulator-Windows.zip`
3. Run `TruckSimulator.exe`

**System Requirements**:
- OS: Windows 10/11 x64
- GPU: NVIDIA GTX 1050, AMD RX 560, or equivalent
- RAM: 8 GB
- Storage: 5 GB SSD space
- DirectX 12 compatible

### Linux

1. Download `TruckSimulator-Linux.tar.gz`
2. Extract: `tar xzf TruckSimulator-Linux.tar.gz`
3. Run: `./TruckSimulator/Binaries/Linux/UE4Shell-Cmd`

**System Requirements**:
- OS: Ubuntu 22.04+ or equivalent
- GPU: Mesa drivers (Vulkan) or proprietary GPU drivers
- RAM: 8 GB
- Storage: 5 GB SSD space

### macOS

1. Download `TruckSimulator-macOS.zip`
2. Extract the archive
3. Double-click `TruckSimulator.app` (or right-click → Open)

**System Requirements**:
- OS: macOS 11.0+ (Apple Silicon M1/M2+ or Intel)
- GPU: Integrated GPU (Apple Silicon) or dedicated
- RAM: 8 GB
- Storage: 5 GB SSD space
- **Note**: Binary is unsigned (see "Unverified Developer" warning)

---

## Gameplay

### Objective

Transport cargo from depot to destination, earn money, gain experience, and unlock new vehicle types and premium routes.

### Controls

#### In Vehicle
| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Accelerate | `W` / `↑` | Right Trigger |
| Brake/Reverse | `S` / `↓` | Left Trigger |
| Steer | `A/D` / `←→` | Left Stick |
| Exit Vehicle | `E` | A Button |
| Pause | `Esc` | Start Button |

#### On Foot
| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Move | `WASD` / Arrow Keys | Left Stick |
| Interact (Board/Contract) | `E` | A Button |
| Pause | `Esc` | Start Button |

### Economy

**Earning Money**:
- Base contract reward varies by route (100–800 currency)
- Distance bonus: +10 per km traveled
- Cargo type multiplier: Fragile +20%, Hazmat +50%
- Deduction: -2.50 per km for fuel

**Example**: 25 km Hazmat route with base 500 → `(500 + 250) × 1.5 - 62.5 = $831.25`

**Experience & Progression**:
- 1 XP per km driven
- 50 XP bonus per delivery completed
- Level up every 100 XP (Level 5 → 3-Axle Trailer, Level 10 → Premium Routes)

**Vehicle Types**:
- **2-Axle Trailer** (Unlocked at Level 1)
  - Capacity: 5 tons
  - Speed: 100 km/h (full speed)
  
- **3-Axle Trailer** (Unlocked at Level 5)
  - Capacity: 8 tons
  - Speed: 85 km/h (slightly slower, more cargo)

---

## Settings

Access via **Main Menu → Settings**:

- **Graphics**: Resolution, quality preset (Low/Medium/High/Epic), V-sync
- **Audio**: Master volume, engine volume, UI volume, music toggle
- **Input**: Controller sensitivity, invert Y-axis, deadzone threshold
- **Gameplay**: Game speed, HUD scale

Settings are auto-saved locally.

---

## Reporting Issues

Found a bug or have a suggestion? Open an issue on [GitHub Issues](https://github.com/[user]/TruckLogisticsSimulator/issues).

**Include**:
1. Bug description
2. Steps to reproduce
3. System info (OS, GPU, resolution)
4. Screenshot/video if applicable

---

## Contributing

Want to help? See [CONTRIBUTING.md](./CONTRIBUTING.md) for coding standards, branching strategy, and contribution guidelines.

**Development Setup**:
- Unreal Engine 5.3+ (free)
- Visual Studio 2022 Community (Windows) or Xcode (macOS)
- Git
- 20 GB disk space

See [DEVELOPMENT.md](./DEVELOPMENT.md) for detailed setup instructions.

---

## Documentation

- **[ARCHITECTURE.md](./ARCHITECTURE.md)** — System design, physics constants, module dependencies
- **[ROADMAP.md](./ROADMAP.md)** — 12-week development plan, feature priorities
- **[DEVELOPMENT.md](./DEVELOPMENT.md)** — Dev environment setup, build instructions, profiling
- **[CONTRIBUTING.md](./CONTRIBUTING.md)** — Code standards, blueprint guidelines, asset naming

---

## Credits

- **Engine**: Unreal Engine 5 (© Epic Games)
- **Physics**: Chaos Dynamics (built-in)
- **Audio**: Freesound.org (placeholder SFX)
- **Assets**: Sketchfab (free models) + Quixel Megascans (environment)

---

## License

This project is licensed under **MIT License** — see [LICENSE](./LICENSE) file for details.

**Third-Party Licenses**:
- Unreal Engine: See [EULA](https://www.unrealengine.com/en-US/eula)
- Plugins & Assets: See respective asset licenses

---

## Community

- **Discord**: [Join our community](https://discord.gg/[invite-code]) (coming soon)
- **Reddit**: [r/TruckLogisticsSimulator](https://reddit.com/r/[subreddit]) (community-driven)
- **GitHub Discussions**: [Ask questions here](https://github.com/[user]/TruckLogisticsSimulator/discussions)

---

## FAQ

**Q: Is this single-player only?**  
A: Yes, v0.1.0 is single-player. Multiplayer is planned for v0.4.0+.

**Q: Will you add more trucks?**  
A: Yes! Truck customization DLC planned post-launch.

**Q: How often are updates released?**  
A: Major updates every 4–8 weeks. Hotfixes as needed.

**Q: Can I mod the game?**  
A: Not yet, but mod support is planned for post-launch.

**Q: Will there be mobile versions?**  
A: Not prioritized. Desktop focus for now.

---

## Version History

| Version | Date | Notes |
|---------|------|-------|
| 0.1.0-alpha | 2026-09-20 (TBD) | Initial release: 10 routes, economy, persistence |
| 0.1.1-patch | TBD | Bug fixes and stability |
| 0.2.0 | TBD | Truck customization DLC, new regions |

---

## Support

If you enjoy the game, please consider:
- ⭐ Starring this repository
- 🐛 Reporting bugs and suggesting features
- 💬 Joining our community
- 📢 Sharing with friends

---

**Made with ❤️ by [Developer Name]**  
*Last Updated: 2026-06-20*

