# Nexus AI Emulator - Dependencies

## Required Dependencies

### Build Tools
- **CMake** 3.20+
- **Visual Studio 2022** (with C++ workload)
- **Python 3.10+** (for AI model tools)

### Libraries

#### Qt Framework
- **Qt 6.5+** (Core, Gui, Widgets, Quick)
- Download: https://www.qt.io/download

#### Graphics & Compute
- **Vulkan SDK** 1.3+
- **OpenGL** 4.5+
- **DirectX 12** (Windows)

#### AI/ML
- **ONNX Runtime** 1.15+
- **CUDA Toolkit** 12.0+ (optional, for NVIDIA GPU)
- **DirectML** (optional, for AMD/Intel GPU)

#### Utilities
- **yaml-cpp** (YAML parsing)
- **zlib** (compression)

## Windows Installation

### 1. Install Visual Studio 2022
1. Download Visual Studio 2022 Community
2. Install with "Desktop development with C++" workload
3. Include Windows 10/11 SDK

### 2. Install Qt
```powershell
# Using choco
choco install qt-creator

# Or download from https://www.qt.io/download
```

### 3. Install Vulkan SDK
```powershell
choco install vulkan-sdk
```

### 4. Install CMake
```powershell
choco install cmake
```

### 5. Install ONNX Runtime
```powershell
# Download from https://github.com/microsoft/onnxruntime/releases
# Or build from source
```

### 6. Install CUDA (Optional - for NVIDIA GPU)
```powershell
choco install cuda
```

## Linux Installation

### Ubuntu/Debian
```bash
# Build tools
sudo apt install build-essential cmake git

# Qt
sudo apt install qt6-base-dev qt6-tools-dev

# Vulkan
sudo apt install libvulkan-dev vulkan-tools

# ONNX Runtime
# Build from source or use pre-built binaries

# Other dependencies
sudo apt install libyaml-cpp-dev zlib1g-dev
```

### Fedora
```bash
sudo dnf install cmake gcc-c++ git
sudo dnf install qt6-qtbase-devel
sudo dnf install vulkan-devel
```

## macOS Installation

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake qt@6 vulkan-sdk onnxruntime
```

## Build from Source

```bash
git clone https://github.com/your-org/nexus-emulator.git
cd nexus-emulator
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Verify Installation

```bash
# Check Qt version
qmake --version

# Check Vulkan
vulkaninfo

# Check CMake
cmake --version
```

## Troubleshooting

### Qt not found
```bash
# Set Qt directory
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
```

### Vulkan not found
```bash
# Install Vulkan SDK
# https://vulkan.lunarg.com/sdk/home
```

### ONNX Runtime errors
```bash
# Ensure ONNX Runtime is in library path
export LD_LIBRARY_PATH=/path/to/onnxruntime/lib:$LD_LIBRARY_PATH
```
