# Nexus AI Emulator - Quick Start Guide

## Prerequisites

- Windows 10/11 (64-bit)
- Visual Studio 2022 with C++ workload
- CMake 3.20+
- Qt 6.5+
- Vulkan SDK

## Building

### 1. Clone the repository
```bash
git clone https://github.com/your-org/nexus-emulator.git
cd nexus-emulator
```

### 2. Create build directory
```bash
mkdir build
cd build
```

### 3. Configure with CMake
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Or use presets:
```bash
cmake --preset x64-release
```

### 4. Build
```bash
cmake --build . --config Release
```

### 5. Run tests (optional)
```bash
ctest --config Release
```

## Running

After building, run the emulator:

```bash
# Windows
out\install\x64-release\bin\NexusEmulator.exe

# Or from build directory
Release\NexusEmulator.exe
```

## Command Line Options

```
NexusEmulator [options]

Options:
  -c, --config <path>    Path to configuration file
  --no-ai                Disable AI upscaling features
  -v, --verbose          Enable verbose logging
  --apk <path>           APK file to launch on startup
  -h, --help             Show help
  --version              Show version
```

## Configuration

Edit `configs/nexus-config.yaml` to customize:

```yaml
ai:
  enabled: true
  frame_generation:
    target_fps: 120
    internal_resolution: "1280x720"
    target_resolution: "2560x1440"

memory:
  ram_limit_mb: 8192
  paging_enabled: true

ui:
  zero_ui: true
  glassmorphism: true
```

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| F12 | Toggle AI Boost |
| Ctrl+Shift+O | Toggle Overlay |
| Alt+Return | Fullscreen |
| F11 | Screenshot |
| Pause | Pause/Resume |

## Troubleshooting

### Build fails with "Qt not found"
```bash
# Set Qt path
set CMAKE_PREFIX_PATH=C:\Qt\6.5\msvc2019_64
```

### "Vulkan not found" error
1. Install Vulkan SDK from https://vulkan.lunarg.com/
2. Restart your computer

### AI features not working
1. Check if ONNX Runtime is installed
2. Verify models exist in `models/` directory
3. Check GPU drivers are up to date

### Performance issues
1. Enable hardware acceleration in BIOS
2. Update GPU drivers
3. Reduce target resolution in config
4. Disable AI features with `--no-ai`

## Next Steps

1. **Install Android System Image**: Download from Android Studio SDK Manager
2. **Configure QEMU**: Place QEMU binaries in `qemu/` directory
3. **Add AI Models**: Download pre-trained models or train your own
4. **Customize**: Edit configuration files for your hardware

## Getting Help

- Documentation: `docs/` directory
- Architecture: See `docs/ARCHITECTURE.md`
- Dependencies: See `DEPENDENCIES.md`
- Issues: https://github.com/your-org/nexus-emulator/issues

## License

MIT License - See LICENSE file for details.
