/**
 * Nexus AI Emulator - Process Analyzer
 * 
 * AI-based process analysis for memory optimization:
 * - Identifies foreground vs background processes
 * - Predicts process memory needs
 * - Detects memory leaks
 */

#ifndef NEXUS_PROCESS_ANALYZER_H
#define NEXUS_PROCESS_ANALYZER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <memory>

namespace Nexus::Memory {

/**
 * @brief Process priority levels
 */
enum class ProcessPriority {
    Foreground,     // User-interactive app
    Visible,        // Visible but not focused
    Perceptible,    // User can perceive (playing audio)
    ForegroundService,  // Running foreground service
    Background,     // Background process
    Cached,         // Cached process (can be killed)
    Empty           // Empty process (no components)
};

/**
 * @brief Process information
 */
struct ProcessInfo {
    QString packageName;
    QString processName;
    int pid = 0;
    int uid = 0;
    ProcessPriority priority = ProcessPriority::Cached;
    int memoryUsageKb = 0;
    int memoryLimitKb = 0;
    qint64 lastActivityTime = 0;
    qint64 startTime = 0;
    bool isService = false;
    bool isGame = false;
    float importanceScore = 0.0f;  // 0.0-1.0, higher = more important
};

/**
 * @brief AI process analyzer
 */
class ProcessAnalyzer : public QObject
{
    Q_OBJECT
    
public:
    explicit ProcessAnalyzer(QObject *parent = nullptr);
    ~ProcessAnalyzer();
    
    /**
     * @brief Initialize analyzer
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Analyze all running processes
     * @return List of process information
     */
    QVector<ProcessInfo> analyzeProcesses();
    
    /**
     * @brief Get detailed info for specific process
     */
    ProcessInfo getProcessInfo(int pid) const;
    
    /**
     * @brief Get process by package name
     */
    ProcessInfo getProcessByPackage(const QString& packageName) const;
    
    /**
     * @brief Calculate importance score for process
     */
    float calculateImportanceScore(const ProcessInfo& process);
    
    /**
     * @brief Predict memory needs for process
     */
    int predictMemoryNeed(const ProcessInfo& process);
    
    /**
     * @brief Check if process is likely a game
     */
    bool isLikelyGame(const QString& packageName) const;
    
    /**
     * @brief Detect memory leaks in process
     */
    bool detectMemoryLeak(int pid) const;

signals:
    void processAnalyzed(const ProcessInfo& info);
    void memoryLeakDetected(int pid);
    void processImportanceChanged(int pid, float newScore);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Cached process info
    QMap<int, ProcessInfo> m_processCache;
    
    // Game package patterns
    QStringList m_gamePackagePatterns = {
        "com.miHoYo.",      // Genshin Impact, Honkai
        "com.tencent.",     // PUBG, COD Mobile
        "com.epicgames.",   // Fortnite
        "com.riotgames.",  // League/Wild Rift
        "com.netease.",     // Various games
        "com.supercell.",   // Clash of Clans, etc.
        "com.king.",        // Candy Crush
        "com.garena."       // Free Fire
    };
    
    // Analysis methods
    void analyzeProcessMemory(int pid);
    void updateProcessPriority(ProcessInfo& process);
    void analyzeProcessBehavior();
};

} // namespace Nexus::Memory

#endif // NEXUS_PROCESS_ANALYZER_H
