# Contributing to Nexus AI Emulator

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## 🎯 Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Focus on what's best for the community

## 🚀 Getting Started

### 1. Fork the Repository

```powershell
# Click "Fork" on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/nexus-ai-emulator.git
cd nexus-ai-emulator
```

### 2. Set Up Development Environment

```powershell
# Install dependencies
choco install qt-creator vulkan-sdk cmake git

# Configure CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

### 3. Create a Branch

```powershell
# Sync with upstream
git remote add upstream https://github.com/ORIGINAL_OWNER/nexus-ai-emulator.git
git fetch upstream
git checkout main
git merge upstream/main

# Create feature branch
git checkout -b feature/your-feature-name
```

## 📝 Making Changes

### Code Style

- Follow **Google C++ Style Guide**
- Use **C++20** features appropriately
- Add **Doxygen** comments for public APIs
- Keep functions small and focused

### Example Code Style

```cpp
/**
 * @brief Initialize AI subsystem
 * @return true if initialization successful
 */
bool AICore::initialize()
{
    qDebug() << "[AICore] Initializing...";
    
    // Implementation here
    
    return true;
}
```

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
feat: add AI frame generation
fix: resolve memory leak in shader cache
docs: update installation instructions
test: add unit tests for memory manager
refactor: improve AI core architecture
chore: update CMake configuration
```

## 🧪 Testing

### Run Tests

```powershell
cd build
cmake --build . --config Debug
ctest --config Debug --verbose
```

### Write Tests

```cpp
#include <QTest>

class TestAICore : public QObject
{
    Q_OBJECT
    
private slots:
    void testInitialization();
    void testFrameProcessing();
};

void TestAICore::testInitialization()
{
    AICore ai;
    bool result = ai.initialize();
    QVERIFY(result);
}
```

## 📤 Submitting Changes

### 1. Update Your Branch

```powershell
git fetch upstream
git rebase upstream/main
```

### 2. Run Tests

```powershell
ctest --config Release
```

### 3. Push and Create PR

```powershell
git push origin feature/your-feature-name
```

Then on GitHub:
1. Click "Pull Request"
2. Fill out the PR template
3. Request review from maintainers

## 📋 Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tests pass
- [ ] New tests added

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added where needed
- [ ] Documentation updated
```

## 🔍 Code Review

Maintainers will review your PR and may request changes. Be responsive to feedback!

## 📞 Questions?

- **Discord**: https://discord.gg/nexus-emulator
- **Issues**: https://github.com/nexus-ai-emulator/issues

---

Thank you for contributing! 🎉
