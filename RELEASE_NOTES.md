# 🎉 Nexus AI Emulator v0.1.0-alpha

## Release Notes

**Release Date**: March 22, 2026

### ✨ What's New

First alpha release of Nexus AI Emulator - Android 13 emulator with AI-powered upscaling!

### 📦 Source Code Only

This release contains the complete source code for the Nexus AI Emulator project.

**Note**: This is a source code release. Building requires:
- Windows 10/11
- Visual Studio 2022
- Qt 6.5+
- Vulkan SDK
- CMake 3.20+

### 📁 What's Included

- ✅ Full C++20 source code (33 files)
- ✅ AI upscaling modules
- ✅ Glassmorphism UI system
- ✅ Memory management
- ✅ Shader cache system
- ✅ Test suite
- ✅ Documentation
- ✅ Build configuration (CMake)

### 🔧 Build Instructions

```powershell
# Clone or extract source
cd nexus-ai-emulator

# Create build directory
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Install
cmake --install . --config Release
```

### 📋 Requirements

| Component | Version | Required |
|-----------|---------|----------|
| OS | Windows 10/11 (64-bit) | ✅ |
| Compiler | MSVC 2019+ | ✅ |
| Qt | 6.5+ | ✅ |
| Vulkan SDK | 1.3+ | ✅ |
| CMake | 3.20+ | ✅ |

### 🐛 Known Issues

1. **Alpha Stage**: This is early alpha software
2. **Stubs**: Some features are stub implementations
3. **AI Models**: Not included (placeholders only)
4. **QEMU**: Requires manual setup
5. **Linux/macOS**: Limited support (Windows-focused)

### 📚 Documentation

- [README.md](README.md) - Getting started
- [QUICKSTART.md](QUICKSTART.md) - Quick start guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Architecture details
- [DEPENDENCIES.md](DEPENDENCIES.md) - Dependency installation

### 🔗 Links

- **Repository**: https://github.com/ByPavel22/x16BDstacks
- **Issues**: https://github.com/ByPavel22/x16BDstacks/issues
- **Discussions**: https://github.com/ByPavel22/x16BDstacks/discussions

### 📄 License

MIT License - See LICENSE file for details.

### 🙏 Acknowledgments

Thanks to all contributors and the open-source community!

---

**Full changelog**: See [CHANGELOG.md](CHANGELOG.md)
