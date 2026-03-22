/**
 * Nexus AI Emulator - QEMU Wrapper
 * 
 * Low-level QEMU process management:
 * - Process lifecycle
 * - IPC communication
 * - Performance monitoring
 */

#ifndef NEXUS_QEMU_WRAPPER_H
#define NEXUS_QEMU_WRAPPER_H

#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#include <QObject>
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QProcess>
#endif
#include <QByteArray>
#endif
#include <memory>

namespace Nexus::Core {

/**
 * @brief QEMU process wrapper
 */
class QemuWrapper : public QObject
{
    Q_OBJECT
    
public:
    explicit QemuWrapper(QObject *parent = nullptr);
    ~QemuWrapper();
    
    /**
     * @brief Initialize QEMU wrapper
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Start QEMU process
     * @param args Command line arguments
     * @return true if start successful
     */
    bool start(const QStringList& args);
    
    /**
     * @brief Stop QEMU process
     */
    void stop();
    
    /**
     * @brief Check if QEMU is running
     */
    bool isRunning() const { return m_running; }
    
    /**
     * @brief Send command to QEMU monitor
     * @param command Command to send
     * @return Command response
     */
    QByteArray sendCommand(const QString& command);
    
    /**
     * @brief Get QEMU PID
     */
    qint64 processId() const;
    
    /**
     * @brief Get CPU usage percentage
     */
    float cpuUsage() const { return m_cpuUsage; }
    
    /**
     * @brief Get memory usage in MB
     */
    int memoryUsage() const { return m_memoryUsageMb; }

signals:
    void started();
    void stopped();
    void outputReceived(const QByteArray& output);
    void errorOccurred(const QString& error);
    
private slots:
    void readOutput();
    void handleError(QProcess::ProcessError error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    bool m_running = false;
    
    QScopedPointer<QProcess> m_process;
    
    float m_cpuUsage = 0.0f;
    int m_memoryUsageMb = 0;
    
    // QEMU monitor socket
    void connectToMonitor();
};

} // namespace Nexus::Core

#endif // NEXUS_QEMU_WRAPPER_H
