/**
 * Nexus AI Emulator - Application Core
 * 
 * Main application class that orchestrates all subsystems:
 * - Emulator core (QEMU)
 * - AI upscaling
 * - UI overlay
 * - Memory management
 * - Shader caching
 */

#ifndef NEXUS_APPLICATION_H
#define NEXUS_APPLICATION_H

#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QObject>
#include <QMainWindow>
#include <QScopedPointer>
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QTimer>
#endif
#include <QSystemTrayIcon>
#include <QMenu>
#endif

#include "core/emulator.h"
#include "ai/ai_core.h"
#include "ui/overlay_window.h"
#include "memory/memory_manager.h"
#include "shader/shader_cache.h"
#include "input/input_handler.h"

namespace Nexus {

/**
 * @brief Main application controller
 * 
 * Coordinates all emulator subsystems and manages the application lifecycle.
 */
class Application : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Application(QWidget *parent = nullptr);
    ~Application();
    
    /**
     * @brief Initialize all subsystems
     * @param configPath Path to YAML configuration file
     * @return true if initialization successful
     */
    bool initialize(const QString& configPath);
    
    /**
     * @brief Shutdown all subsystems gracefully
     */
    void shutdown();
    
    /**
     * @brief Disable AI features (command-line override)
     */
    void disableAI();
    
    /**
     * @brief Enable verbose logging
     */
    void setVerboseLogging(bool enabled);
    
    /**
     * @brief Queue an APK for launch
     */
    void queueApkLaunch(const QString& apkPath);
    
    /**
     * @brief Get the emulator core instance
     */
    Core::Emulator* emulator() const { return m_emulator.get(); }
    
    /**
     * @brief Get the AI core instance
     */
    AI::AICore* aiCore() const { return m_aiCore.get(); }
    
    /**
     * @brief Get the UI overlay instance
     */
    UI::OverlayWindow* overlay() const { return m_overlay.get(); }

signals:
    void initializationComplete();
    void shutdownComplete();
    void aiBoostToggled(bool enabled);
    void frameProcessed(int fps, float ai_latency_ms);
    
private slots:
    void onFrameReady();
    void onAiBoostTriggered();
    void onOverlayTriggered();
    void updateMetrics();
    
private:
    bool loadConfiguration(const QString& configPath);
    bool initializeSubsystems();
    void setupUI();
    void setupSystemTray();
    void setupGlobalHotkeys();
    
    // Configuration
    struct Config {
        bool aiEnabled = true;
        bool zeroUiMode = true;
        int ramLimitMb = 8192;
        QString internalResolution = "1280x720";
        QString targetResolution = "2560x1440";
        int targetFps = 120;
        bool verboseLogging = false;
    } m_config;
    
    // Subsystems
    QScopedPointer<Core::Emulator> m_emulator;
    QScopedPointer<AI::AICore> m_aiCore;
    QScopedPointer<UI::OverlayWindow> m_overlay;
    QScopedPointer<Memory::MemoryManager> m_memoryManager;
    QScopedPointer<Shader::ShaderCache> m_shaderCache;
    QScopedPointer<Input::InputHandler> m_inputHandler;
    
    // UI components
    QScopedPointer<QSystemTrayIcon> m_trayIcon;
    QScopedPointer<QMenu> m_trayMenu;
    
    // Timers
    QScopedPointer<QTimer> m_metricsTimer;
    QScopedPointer<QTimer> m_aiInferenceTimer;
    
    // State
    bool m_initialized = false;
    bool m_aiBoostActive = false;
    int m_currentFps = 0;
    float m_aiLatencyMs = 0.0f;
    
    // Pending APK launch
    QString m_pendingApkPath;
};

} // namespace Nexus

#endif // NEXUS_APPLICATION_H
