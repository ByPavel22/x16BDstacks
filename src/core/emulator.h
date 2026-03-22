/**
 * Nexus AI Emulator - Emulator Core
 * 
 * Main emulator controller that manages:
 * - QEMU virtual machine
 * - Android system
 * - Display output
 */

#ifndef NEXUS_EMULATOR_H
#define NEXUS_EMULATOR_H

#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#include <QObject>
#include <QImage>
#include <QProcess>
#include <memory>
#endif

namespace Nexus::Core {

/**
 * @brief Main emulator controller
 */
class Emulator : public QObject
{
    Q_OBJECT
    
public:
    explicit Emulator(QObject *parent = nullptr);
    ~Emulator();
    
    /**
     * @brief Initialize emulator
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown emulator
     */
    void shutdown();
    
    /**
     * @brief Start Android system
     * @return true if start successful
     */
    bool start();
    
    /**
     * @brief Stop Android system
     */
    void stop();
    
    /**
     * @brief Install APK
     * @param apkPath Path to APK file
     * @return true if installation successful
     */
    bool installApk(const QString& apkPath);
    
    /**
     * @brief Launch app by package name
     * @param packageName Package name to launch
     * @return true if launch successful
     */
    bool launchApp(const QString& packageName);
    
    /**
     * @brief Get current FPS
     */
    int currentFps() const { return m_currentFps; }
    
    /**
     * @brief Check if emulator is running
     */
    bool isRunning() const { return m_running; }

signals:
    void frameReady();
    void started();
    void stopped();
    void errorOccurred(const QString& error);
    void appLaunched(const QString& packageName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    bool m_running = false;
    int m_currentFps = 0;
    
    // QEMU process
    QScopedPointer<QProcess> m_qemuProcess;
    
    // Display
    QImage m_frameBuffer;
    
    // Methods
    bool startQemu();
    void stopQemu();
    void processFrame();
};

} // namespace Nexus::Core

#endif // NEXUS_EMULATOR_H
