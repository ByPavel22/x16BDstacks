/**
 * Nexus AI Emulator - Memory Manager Implementation
 */

#include "memory_manager.h"
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#endif

namespace Nexus::Memory {

/**
 * @brief Private implementation for MemoryManager
 */
struct MemoryManager::Impl
{
    // Process information
    struct ProcessInfo {
        QString packageName;
        QString processName;
        int pid = 0;
        int memoryUsageKb = 0;
        qint64 lastActivityTime = 0;
        bool isForeground = false;
        bool isService = false;
    };
    
    // Active processes
    QVector<ProcessInfo> processes;
    
    // Whitelist (never kill)
    QSet<QString> whitelist = {
        "com.android.systemui",
        "com.google.android.gms",
        "com.android.phone",
        "com.android.settings"
    };
    
    // Memory thresholds
    float highPressureThreshold = 0.85f;   // 85%
    float criticalThreshold = 0.95f;        // 95%
    
    // AI analysis state
    bool neuralAnalysisEnabled = true;
    float activityWeight = 0.4f;
    float memoryWeight = 0.3f;
    float foregroundWeight = 0.3f;
};

MemoryManager::MemoryManager(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
    m_analysisTimer = new QTimer(this);
    connect(m_analysisTimer, &QTimer::timeout, this, &MemoryManager::analyzeMemoryUsage);
}

MemoryManager::~MemoryManager()
{
    shutdown();
}

bool MemoryManager::initialize()
{
    qDebug() << "[MemoryManager] Initializing...";
    qDebug() << "[MemoryManager] RAM Limit:" << m_ramLimitMb << "MB";
    
    // Start periodic analysis
    m_analysisTimer->setInterval(5000);  // Analyze every 5 seconds
    m_analysisTimer->start();
    
    m_initialized = true;
    qDebug() << "[MemoryManager] Initialization complete";
    
    return true;
}

void MemoryManager::shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    qDebug() << "[MemoryManager] Shutting down...";
    
    m_analysisTimer->stop();
    
    m_initialized = false;
}

void MemoryManager::setRamLimit(int limitMb)
{
    m_ramLimitMb = limitMb;
    qDebug() << "[MemoryManager] RAM limit set to:" << limitMb << "MB";
}

int MemoryManager::availableMemory() const
{
    return m_ramLimitMb - m_currentUsageMb;
}

void MemoryManager::setPagingEnabled(bool enabled)
{
    m_pagingEnabled = enabled;
    qDebug() << "[MemoryManager] Intelligent paging:" << (enabled ? "enabled" : "disabled");
}

void MemoryManager::setAggressiveMode(bool enabled)
{
    m_aggressiveMode = enabled;
    qDebug() << "[MemoryManager] Aggressive mode:" << (enabled ? "enabled" : "disabled");
}

void MemoryManager::addToWhitelist(const QString& packageName)
{
    m_impl->whitelist.insert(packageName);
    qDebug() << "[MemoryManager] Added to whitelist:" << packageName;
}

void MemoryManager::removeFromWhitelist(const QString& packageName)
{
    m_impl->whitelist.remove(packageName);
    qDebug() << "[MemoryManager] Removed from whitelist:" << packageName;
}

MemoryManager::MemoryStats MemoryManager::getStats() const
{
    MemoryStats stats;
    
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);
    
    stats.totalMemory = static_cast<int>(memStatus.ullTotalPhys / (1024 * 1024));
    stats.usedMemory = static_cast<int>((memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024));
    stats.availableMemory = static_cast<int>(memStatus.ullAvailPhys / (1024 * 1024));
    stats.usagePercent = (static_cast<float>(stats.usedMemory) / stats.totalMemory) * 100.0f;
#else
    stats.totalMemory = m_ramLimitMb;
    stats.usedMemory = m_currentUsageMb;
    stats.availableMemory = availableMemory();
    stats.usagePercent = (static_cast<float>(stats.usedMemory) / stats.totalMemory) * 100.0f;
#endif
    
    return stats;
}

void MemoryManager::analyzeMemoryUsage()
{
    if (!m_initialized) {
        return;
    }
    
    // Analyze running processes
    analyzeProcesses();
    
    // Check memory pressure
    MemoryStats stats = getStats();
    emit statsUpdated(stats);
    
    if (stats.usagePercent >= m_impl->criticalThreshold * 100.0f) {
        qWarning() << "[MemoryManager] Critical memory pressure:" << stats.usagePercent << "%";
        emit memoryPressureHigh(stats.usagePercent);
        optimizeMemory();
    } else if (stats.usagePercent >= m_impl->highPressureThreshold * 100.0f) {
        qDebug() << "[MemoryManager] High memory pressure:" << stats.usagePercent << "%";
        emit memoryPressureHigh(stats.usagePercent);
    }
    
    // Update current usage
    m_currentUsageMb = stats.usedMemory;
}

void MemoryManager::optimizeMemory()
{
    qDebug() << "[MemoryManager] Optimizing memory...";
    
    if (m_pagingEnabled) {
        trimMemory();
    }
    
    if (m_aggressiveMode) {
        killBackgroundProcesses();
    }
    
    // Recalculate optimal allocation
    int optimalAlloc = calculateOptimalAllocation();
    qDebug() << "[MemoryManager] Optimal allocation:" << optimalAlloc << "MB";
}

void MemoryManager::analyzeProcesses()
{
    m_impl->processes.clear();
    
#ifdef Q_OS_WIN
    // Get process list from Windows
    DWORD processIds[1024];
    DWORD bytesReturned;
    
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        int numProcesses = bytesReturned / sizeof(DWORD);
        
        for (int i = 0; i < numProcesses && i < 100; ++i) {
            DWORD pid = processIds[i];
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (hProcess) {
                HMODULE hMod;
                DWORD cbNeeded;
                char processName[MAX_PATH];
                
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
                    
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        Impl::ProcessInfo info;
                        info.processName = QString(processName);
                        info.pid = pid;
                        info.memoryUsageKb = static_cast<int>(pmc.WorkingSetSize / 1024);
                        
                        m_impl->processes.append(info);
                    }
                }
                
                CloseHandle(hProcess);
            }
        }
    }
#else
    // Linux/macOS: Parse /proc filesystem
    QDir procDir("/proc");
    QFileInfoList entries = procDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& entry : entries) {
        bool ok;
        int pid = entry.fileName().toInt(&ok);
        if (!ok) continue;
        
        // Read process memory info
        QFile statmFile(QString("/proc/%1/statm").arg(pid));
        if (statmFile.open(QFile::ReadOnly)) {
            QTextStream stream(&statmFile);
            int size, resident;
            stream >> size >> resident;
            
            Impl::ProcessInfo info;
            info.pid = pid;
            info.memoryUsageKb = resident * 4;  // Pages to KB (assuming 4KB pages)
            
            m_impl->processes.append(info);
        }
    }
#endif
}

void MemoryManager::killBackgroundProcesses()
{
    qDebug() << "[MemoryManager] Killing background processes...";
    
    // Sort processes by priority (lowest priority first)
    std::sort(m_impl->processes.begin(), m_impl->processes.end(),
              [this](const Impl::ProcessInfo& a, const Impl::ProcessInfo& b) {
                  // Whitelist processes have highest priority
                  if (m_impl->whitelist.contains(a.packageName)) return false;
                  if (m_impl->whitelist.contains(b.packageName)) return true;
                  
                  // Foreground processes have higher priority
                  if (a.isForeground && !b.isForeground) return false;
                  if (!a.isForeground && b.isForeground) return true;
                  
                  // Recent activity has higher priority
                  return a.lastActivityTime < b.lastActivityTime;
              });
    
    // Kill lowest priority processes until we're under threshold
    MemoryStats stats = getStats();
    int targetUsage = static_cast<int>(m_ramLimitMb * 0.7f);  // Target 70% usage
    
    for (const Impl::ProcessInfo& process : m_impl->processes) {
        if (stats.usedMemory <= targetUsage) {
            break;
        }
        
        // Skip whitelisted processes
        if (m_impl->whitelist.contains(process.packageName)) {
            continue;
        }
        
        // Skip foreground processes
        if (process.isForeground) {
            continue;
        }
        
        // Kill process (stub - would use ADB or similar)
        qDebug() << "[MemoryManager] Would kill:" << process.packageName 
                 << "(" << process.memoryUsageKb << "KB)";
        
        stats.usedMemory -= process.memoryUsageKb / 1024;
        emit processKilled(process.packageName);
    }
}

void MemoryManager::trimMemory()
{
    qDebug() << "[MemoryManager] Trimming memory caches...";
    
    // On Linux, we can drop caches
#ifdef Q_OS_LINUX
    QFile dropCaches("/proc/sys/vm/drop_caches");
    if (dropCaches.open(QFile::WriteOnly)) {
        dropCaches.write("1");
        dropCaches.close();
    }
#endif
    
    // Trigger garbage collection in emulated Android system
    // This would send a broadcast to Android to trim memory
    
    emit memoryFreed(100);  // Stub value
}

int MemoryManager::calculateOptimalAllocation()
{
    // AI-based calculation considering:
    // - Active game/app requirements
    // - Background service needs
    // - System overhead
    
    int foregroundNeed = 0;
    int backgroundNeed = 0;
    int systemReserve = 512;  // Reserve 512MB for system
    
    for (const Impl::ProcessInfo& process : m_impl->processes) {
        if (process.isForeground) {
            foregroundNeed += process.memoryUsageKb;
        } else if (!m_impl->whitelist.contains(process.packageName)) {
            backgroundNeed += process.memoryUsageKb / 2;  // Count background at 50%
        }
    }
    
    int optimal = (foregroundNeed + backgroundNeed) / 1024 + systemReserve;
    return qBound(m_ramMinMb, optimal, m_ramLimitMb);
}

} // namespace Nexus::Memory
