/**
 * Nexus AI Emulator - Process Analyzer Implementation
 */

#include "process_analyzer.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

namespace Nexus::Memory {

/**
 * @brief Private implementation for ProcessAnalyzer
 */
struct ProcessAnalyzer::Impl
{
    // Process history for behavior analysis
    struct ProcessHistory {
        QVector<int> memoryHistory;  // Memory usage over time
        QVector<qint64> timestamps;
        static constexpr int MAX_SAMPLES = 60;  // 1 minute at 1 sample/sec
    };
    
    QMap<int, ProcessHistory> processHistory;
    
    // AI model state (stub)
    bool modelLoaded = false;
    QString modelPath;
    
    // Thresholds
    float memoryLeakThreshold = 0.9f;  // 90% consistent growth
    int leakDetectionSamples = 10;
};

ProcessAnalyzer::ProcessAnalyzer(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

ProcessAnalyzer::~ProcessAnalyzer()
{
}

bool ProcessAnalyzer::initialize()
{
    qDebug() << "[ProcessAnalyzer] Initializing...";
    
    // Load AI model for prediction (stub)
    m_impl->modelPath = "models/process_analyzer.onnx";
    
    // In real implementation, would load neural network model
    // that predicts process importance and memory needs
    
    m_impl->modelLoaded = QFile::exists(m_impl->modelPath);
    
    if (!m_impl->modelLoaded) {
        qWarning() << "[ProcessAnalyzer] AI model not found, using heuristics";
    }
    
    m_initialized = true;
    qDebug() << "[ProcessAnalyzer] Initialization complete";
    
    return true;
}

QVector<ProcessInfo> ProcessAnalyzer::analyzeProcesses()
{
    QVector<ProcessInfo> processes;
    
#ifdef Q_OS_WIN
    // Windows: Use Toolhelp32 snapshot
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                ProcessInfo info;
                info.pid = pe32.th32ProcessID;
                info.ppid = pe32.th32ParentProcessID;
                info.processName = QString::fromWCharArray(pe32.szExeFile);
                info.packageName = info.processName;  // Windows doesn't have package names
                
                // Get memory usage
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                              FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        info.memoryUsageKb = static_cast<int>(pmc.WorkingSetSize / 1024);
                    }
                    CloseHandle(hProcess);
                }
                
                // Determine priority
                updateProcessPriority(info);
                
                processes.append(info);
                emit processAnalyzed(info);
                
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
    }
#else
    // Linux: Parse /proc filesystem
    QDir procDir("/proc");
    QFileInfoList entries = procDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& entry : entries) {
        bool ok;
        int pid = entry.fileName().toInt(&ok);
        if (!ok) continue;
        
        ProcessInfo info = readLinuxProcess(pid);
        if (info.pid > 0) {
            processes.append(info);
            emit processAnalyzed(info);
        }
    }
#endif
    
    // Update cache
    for (const ProcessInfo& info : processes) {
        m_processCache[info.pid] = info;
    }
    
    return processes;
}

ProcessInfo ProcessAnalyzer::getProcessInfo(int pid) const
{
    return m_processCache.value(pid);
}

ProcessInfo ProcessAnalyzer::getProcessByPackage(const QString& packageName) const
{
    for (auto it = m_processCache.begin(); it != m_processCache.end(); ++it) {
        if (it->packageName == packageName) {
            return it.value();
        }
    }
    
    return ProcessInfo();
}

float ProcessAnalyzer::calculateImportanceScore(const ProcessInfo& process)
{
    float score = 0.0f;
    
    // Base score from priority
    switch (process.priority) {
        case ProcessPriority::Foreground:
            score = 1.0f;
            break;
        case ProcessPriority::Visible:
            score = 0.8f;
            break;
        case ProcessPriority::Perceptible:
            score = 0.6f;
            break;
        case ProcessPriority::ForegroundService:
            score = 0.7f;
            break;
        case ProcessPriority::Background:
            score = 0.4f;
            break;
        case ProcessPriority::Cached:
            score = 0.2f;
            break;
        case ProcessPriority::Empty:
            score = 0.1f;
            break;
    }
    
    // Bonus for games
    if (process.isGame) {
        score = qMin(1.0f, score + 0.2f);
    }
    
    // Bonus for recent activity
    qint64 timeSinceActivity = QDateTime::currentMSecsSinceEpoch() - process.lastActivityTime;
    if (timeSinceActivity < 5000) {  // Active in last 5 seconds
        score = qMin(1.0f, score + 0.1f);
    }
    
    // Penalty for high memory usage
    if (process.memoryUsageKb > process.memoryLimitKb * 0.9) {
        score = qMax(0.0f, score - 0.1f);
    }
    
    return score;
}

int ProcessAnalyzer::predictMemoryNeed(const ProcessInfo& process)
{
    // AI-based prediction (stub)
    // Real implementation would use neural network
    
    // Simple heuristic based on process type
    if (process.isGame) {
        // Games typically need more memory
        return process.memoryUsageKb * 1.5;
    }
    
    if (process.isService) {
        // Services are usually stable
        return process.memoryUsageKb * 1.1;
    }
    
    // Default: slight growth expectation
    return process.memoryUsageKb * 1.05;
}

bool ProcessAnalyzer::isLikelyGame(const QString& packageName) const
{
    QString lowerName = packageName.toLower();
    
    for (const QString& pattern : m_gamePackagePatterns) {
        if (lowerName.startsWith(pattern.toLower())) {
            return true;
        }
    }
    
    // Check for common game-related keywords
    QStringList gameKeywords = {"game", "play", "gaming", "esports"};
    for (const QString& keyword : gameKeywords) {
        if (lowerName.contains(keyword)) {
            return true;
        }
    }
    
    return false;
}

bool ProcessAnalyzer::detectMemoryLeak(int pid) const
{
    auto it = m_impl->processHistory.find(pid);
    if (it == m_impl->processHistory.end()) {
        return false;
    }
    
    const Impl::ProcessHistory& history = it.value();
    
    // Need enough samples
    if (history.memoryHistory.size() < m_impl->leakDetectionSamples) {
        return false;
    }
    
    // Check for consistent growth
    int growthCount = 0;
    for (int i = 1; i < history.memoryHistory.size(); ++i) {
        if (history.memoryHistory[i] > history.memoryHistory[i-1]) {
            growthCount++;
        }
    }
    
    float growthRatio = static_cast<float>(growthCount) / (history.memoryHistory.size() - 1);
    
    if (growthRatio >= m_impl->memoryLeakThreshold) {
        emit memoryLeakDetected(pid);
        return true;
    }
    
    return false;
}

void ProcessAnalyzer::analyzeProcessMemory(int pid)
{
    // Get current memory usage
    ProcessInfo info = getProcessInfo(pid);
    
    // Update history
    Impl::ProcessHistory& history = m_impl->processHistory[pid];
    history.memoryHistory.append(info.memoryUsageKb);
    history.timestamps.append(QDateTime::currentMSecsSinceEpoch());
    
    // Trim history
    if (history.memoryHistory.size() > Impl::ProcessHistory::MAX_SAMPLES) {
        history.memoryHistory.removeFirst();
        history.timestamps.removeFirst();
    }
    
    // Check for memory leak
    detectMemoryLeak(pid);
}

void ProcessAnalyzer::updateProcessPriority(ProcessInfo& process)
{
    // Determine priority based on process characteristics
    // This is a simplified version - real implementation would query Android
    
    if (isLikelyGame(process.packageName)) {
        process.isGame = true;
        process.priority = ProcessPriority::Foreground;  // Assume games are foreground
    }
    
    // Check if it's a service
    if (process.processName.contains("service", Qt::CaseInsensitive) ||
        process.packageName.contains(".service")) {
        process.isService = true;
        if (process.priority == ProcessPriority::Background) {
            process.priority = ProcessPriority::ForegroundService;
        }
    }
    
    // Calculate importance score
    process.importanceScore = calculateImportanceScore(process);
}

ProcessInfo ProcessAnalyzer::readLinuxProcess(int pid)
{
    ProcessInfo info;
    info.pid = pid;
    
    // Read /proc/[pid]/stat
    QFile statFile(QString("/proc/%1/stat").arg(pid));
    if (!statFile.open(QFile::ReadOnly)) {
        return info;
    }
    
    QTextStream statStream(&statFile);
    QString statLine = statStream.readLine();
    
    // Parse stat file (simplified)
    QRegularExpression re("(\\d+)\\s+\\((.+)\\)");
    QRegularExpressionMatch match = re.match(statLine);
    
    if (match.hasMatch()) {
        info.processName = match.captured(2);
        info.packageName = info.processName;
    }
    
    statFile.close();
    
    // Read /proc/[pid]/statm for memory
    QFile statmFile(QString("/proc/%1/statm").arg(pid));
    if (statmFile.open(QFile::ReadOnly)) {
        QTextStream statmStream(&statmFile);
        int size, resident;
        statmStream >> size >> resident;
        info.memoryUsageKb = resident * 4;  // Pages to KB (4KB pages)
    }
    
    // Determine priority
    updateProcessPriority(info);
    
    return info;
}

} // namespace Nexus::Memory
