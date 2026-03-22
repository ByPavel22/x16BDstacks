/**
 * Nexus AI Emulator - Application Implementation
 */

#include "application.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QShortcut>
#include <QKeySequence>
#include <QHotkey>
#include <yaml-cpp/yaml.h>
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QDebug>
#endif

namespace Nexus {

Application::Application(QWidget *parent)
    : QMainWindow(parent)
{
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize(const QString& configPath)
{
    qDebug() << "[Application] Initializing...";
    
    // Load configuration
    if (!loadConfiguration(configPath)) {
        qCritical() << "[Application] Failed to load configuration";
        return false;
    }
    
    // Initialize subsystems
    if (!initializeSubsystems()) {
        qCritical() << "[Application] Failed to initialize subsystems";
        return false;
    }
    
    // Setup UI
    setupUI();
    setupSystemTray();
    setupGlobalHotkeys();
    
    m_initialized = true;
    
    qDebug() << "[Application] Initialization complete";
    emit initializationComplete();
    
    return true;
}

void Application::shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    qDebug() << "[Application] Shutting down...";
    
    // Stop timers
    m_metricsTimer->stop();
    m_aiInferenceTimer->stop();
    
    // Shutdown subsystems in reverse order
    m_inputHandler.reset();
    m_shaderCache->flush();
    m_memoryManager.reset();
    m_overlay.reset();
    m_aiCore.reset();
    m_emulator.reset();
    
    m_initialized = false;
    
    qDebug() << "[Application] Shutdown complete";
    emit shutdownComplete();
}

void Application::disableAI()
{
    m_config.aiEnabled = false;
    if (m_aiCore) {
        m_aiCore->setEnabled(false);
    }
    qDebug() << "[Application] AI features disabled";
}

void Application::setVerboseLogging(bool enabled)
{
    m_config.verboseLogging = enabled;
    // TODO: Set global logging level
}

void Application::queueApkLaunch(const QString& apkPath)
{
    m_pendingApkPath = apkPath;
}

bool Application::loadConfiguration(const QString& configPath)
{
    qDebug() << "[Application] Loading configuration from:" << configPath;
    
    if (!QFile::exists(configPath)) {
        qWarning() << "[Application] Config file not found, using defaults";
        return true; // Use defaults
    }
    
    try {
        YAML::Node config = YAML::LoadFile(configPath.toStdString());
        
        // Parse AI settings
        if (config["ai"]) {
            m_config.aiEnabled = config["ai"]["enabled"].as<bool>(true);
        }
        
        // Parse memory settings
        if (config["memory"]) {
            m_config.ramLimitMb = config["memory"]["ram_limit_mb"].as<int>(8192);
        }
        
        // Parse display settings
        if (config["emulator"]["display"]) {
            // TODO: Parse resolution, DPI, etc.
        }
        
        // Parse UI settings
        if (config["ui"]) {
            m_config.zeroUiMode = config["ui"]["zero_ui"]["enabled"].as<bool>(true);
        }
        
        return true;
        
    } catch (const YAML::Exception& e) {
        qCritical() << "[Application] YAML parsing error:" << e.what();
        return false;
    }
}

bool Application::initializeSubsystems()
{
    qDebug() << "[Application] Initializing subsystems...";
    
    // 1. Initialize Emulator Core (QEMU)
    m_emulator.reset(new Core::Emulator());
    if (!m_emulator->initialize()) {
        qCritical() << "[Application] Failed to initialize emulator core";
        return false;
    }
    connect(m_emulator.data(), &Core::Emulator::frameReady,
            this, &Application::onFrameReady);
    
    // 2. Initialize AI Core
    if (m_config.aiEnabled) {
        m_aiCore.reset(new AI::AICore());
        if (!m_aiCore->initialize()) {
            qWarning() << "[Application] AI Core initialization failed, disabling AI";
            m_config.aiEnabled = false;
        } else {
            connect(m_aiCore.data(), &AI::AICore::boostToggled,
                    this, &Application::onAiBoostTriggered);
        }
    }
    
    // 3. Initialize Memory Manager
    m_memoryManager.reset(new Memory::MemoryManager());
    m_memoryManager->setRamLimit(m_config.ramLimitMb);
    if (!m_memoryManager->initialize()) {
        qWarning() << "[Application] Memory Manager initialization degraded";
    }
    
    // 4. Initialize Shader Cache
    m_shaderCache.reset(new Shader::ShaderCache());
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/shaders";
    QDir().mkpath(cachePath);
    m_shaderCache->setCachePath(cachePath);
    if (!m_shaderCache->initialize()) {
        qWarning() << "[Application] Shader Cache initialization failed";
    }
    
    // 5. Initialize Input Handler
    m_inputHandler.reset(new Input::InputHandler());
    if (!m_inputHandler->initialize()) {
        qCritical() << "[Application] Input Handler initialization failed";
        return false;
    }
    connect(m_inputHandler.data(), &Input::InputHandler::overlayTriggered,
            this, &Application::onOverlayTriggered);
    
    // 6. Initialize UI Overlay
    m_overlay.reset(new UI::OverlayWindow());
    if (!m_overlay->initialize()) {
        qCritical() << "[Application] UI Overlay initialization failed";
        return false;
    }
    
    return true;
}

void Application::setupUI()
{
    // Set window properties
    setWindowTitle("Nexus AI Emulator");
    setMinimumSize(QSize(800, 600));
    resize(QSize(1280, 720));
    
    // Apply glassmorphism effect on Windows
#ifdef Q_OS_WIN
    // TODO: Apply DWM blur behind effect
#endif
    
    // Show overlay if not in Zero UI mode
    if (!m_config.zeroUiMode) {
        m_overlay->show();
    }
}

void Application::setupSystemTray()
{
    m_trayMenu.reset(new QMenu(this));
    
    QAction* showAction = m_trayMenu->addAction("Show");
    connect(showAction, &QAction::triggered, this, &QWidget::show);
    
    QAction* aiBoostAction = m_trayMenu->addAction("AI Boost");
    aiBoostAction->setCheckable(true);
    connect(aiBoostAction, &QAction::toggled, this, &Application::onAiBoostTriggered);
    
    m_trayMenu->addSeparator();
    
    QAction* quitAction = m_trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    m_trayIcon.reset(new QSystemTrayIcon(this));
    m_trayIcon->setContextMenu(m_trayMenu.get());
    m_trayIcon->setIcon(QIcon(":/icons/nexus_icon.png"));
    m_trayIcon->setToolTip("Nexus AI Emulator");
    m_trayIcon->show();
    
    connect(m_trayIcon.data(), &QSystemTrayIcon::activated,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::DoubleClick) {
                    show();
                    raise();
                    activateWindow();
                }
            });
}

void Application::setupGlobalHotkeys()
{
    // F12 - AI Boost toggle
    QShortcut* aiBoostShortcut = new QShortcut(QKeySequence("F12"), this);
    connect(aiBoostShortcut, &QShortcut::activated, this, &Application::onAiBoostTriggered);
    
    // Ctrl+Shift+O - Toggle overlay
    QShortcut* overlayShortcut = new QShortcut(QKeySequence("Ctrl+Shift+O"), this);
    connect(overlayShortcut, &QShortcut::activated, this, &Application::onOverlayTriggered);
    
    // Alt+F4 handled by Qt
}

void Application::onFrameReady()
{
    // Frame received from emulator
    // Process through AI upscaling if enabled
    if (m_config.aiEnabled && m_aiBoostActive && m_aiCore) {
        m_aiCore->processFrame();
    }
    
    // Update metrics
    m_currentFps = m_emulator->currentFps();
    if (m_aiCore) {
        m_aiLatencyMs = m_aiCore->lastInferenceLatencyMs();
    }
}

void Application::onAiBoostTriggered()
{
    if (!m_config.aiEnabled || !m_aiCore) {
        return;
    }
    
    m_aiBoostActive = !m_aiBoostActive;
    m_aiCore->setEnabled(m_aiBoostActive);
    
    qDebug() << "[Application] AI Boost:" << (m_aiBoostActive ? "ON" : "OFF");
    emit aiBoostToggled(m_aiBoostActive);
    
    // Update tray icon
    if (m_trayMenu) {
        auto actions = m_trayMenu->actions();
        for (QAction* action : actions) {
            if (action->text() == "AI Boost") {
                action->setChecked(m_aiBoostActive);
                break;
            }
        }
    }
}

void Application::onOverlayTriggered()
{
    if (m_overlay->isVisible()) {
        m_overlay->hide();
    } else {
        m_overlay->show();
        m_overlay->raise();
    }
}

void Application::updateMetrics()
{
    // Update metrics display in overlay
    if (m_overlay) {
        m_overlay->updateFps(m_currentFps);
        m_overlay->updateAiLatency(m_aiLatencyMs);
        m_overlay->updateMemoryUsage(m_memoryManager->currentUsageMb());
    }
}

} // namespace Nexus
