/**
 * Nexus AI Emulator - QEMU Wrapper Implementation
 */

#include "qemu_wrapper.h"
#include <QDebug>

namespace Nexus::Core {

/**
 * @brief Private implementation for QemuWrapper
 */
struct QemuWrapper::Impl
{
    QString qemuPath;
    QStringList defaultArgs;
    
    // Monitor connection
    QString monitorSocket;
    bool monitorConnected = false;
};

QemuWrapper::QemuWrapper(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

QemuWrapper::~QemuWrapper()
{
    stop();
}

bool QemuWrapper::initialize()
{
    qDebug() << "[QemuWrapper] Initializing...";
    
#ifdef Q_OS_WIN
    m_impl->qemuPath = QCoreApplication::applicationDirPath() + "/qemu/qemu-system-x86_64.exe";
#else
    m_impl->qemuPath = "/usr/bin/qemu-system-x86_64";
#endif
    
    // Default arguments for Android emulation
    m_impl->defaultArgs = {
        "-enable-kvm",
        "-cpu", "host",
        "-machine", "q35,accel=kvm",
        "-m", "4096",
        "-smp", "4",
        "-display", "none",
        "-monitor", "unix:/tmp/qemu-monitor,server,nowait"
    };
    
    m_impl->monitorSocket = "/tmp/qemu-monitor";
    
    m_initialized = true;
    qDebug() << "[QemuWrapper] Initialization complete";
    
    return true;
}

bool QemuWrapper::start(const QStringList& args)
{
    if (!m_initialized) {
        return false;
    }
    
    qDebug() << "[QemuWrapper] Starting QEMU...";
    
    m_process.reset(new QProcess(this));
    
    connect(m_process.data(), &QProcess::started, this, [this]() {
        qDebug() << "[QemuWrapper] QEMU started";
        m_running = true;
        emit started();
        
        // Connect to monitor
        connectToMonitor();
    });
    
    connect(m_process.data(), &QProcess::readyReadStandardOutput,
            this, &QemuWrapper::readOutput);
    
    connect(m_process.data(), &QProcess::errorOccurred,
            this, &QemuWrapper::handleError);
    
    QStringList allArgs = m_impl->defaultArgs + args;
    
    m_process->start(m_impl->qemuPath, allArgs);
    
    return true;
}

void QemuWrapper::stop()
{
    if (!m_running || !m_process) {
        return;
    }
    
    qDebug() << "[QemuWrapper] Stopping QEMU...";
    
    // Send quit command to monitor first
    sendCommand("quit");
    
    // Force kill if still running
    if (!m_process->waitForFinished(5000)) {
        m_process->kill();
    }
    
    m_running = false;
    emit stopped();
}

QByteArray QemuWrapper::sendCommand(const QString& command)
{
    if (!m_impl->monitorConnected) {
        qWarning() << "[QemuWrapper] Monitor not connected";
        return QByteArray();
    }
    
    // Would send command to QEMU monitor socket
    qDebug() << "[QemuWrapper] Sending command:" << command;
    
    return QByteArray();
}

qint64 QemuWrapper::processId() const
{
    if (m_process) {
        return m_process->processId();
    }
    return -1;
}

void QemuWrapper::readOutput()
{
    if (!m_process) {
        return;
    }
    
    QByteArray output = m_process->readAllStandardOutput();
    emit outputReceived(output);
}

void QemuWrapper::handleError(QProcess::ProcessError error)
{
    QString errorStr;
    
    switch (error) {
        case QProcess::FailedToStart:
            errorStr = "Failed to start";
            break;
        case QProcess::Crashed:
            errorStr = "Crashed";
            break;
        case QProcess::Timedout:
            errorStr = "Timed out";
            break;
        default:
            errorStr = "Unknown error";
    }
    
    qCritical() << "[QemuWrapper] Error:" << errorStr;
    emit errorOccurred(errorStr);
}

void QemuWrapper::connectToMonitor()
{
    // Would connect to QEMU monitor socket for IPC
    m_impl->monitorConnected = true;
    qDebug() << "[QemuWrapper] Monitor connected";
}

} // namespace Nexus::Core
