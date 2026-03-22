/**
 * Nexus AI Emulator - Memory Manager
 * 
 * Intelligent memory management with:
 * - Dynamic RAM allocation
 * - Neural process analysis
 * - Background service killing
 */

#ifndef NEXUS_MEMORY_MANAGER_H
#define NEXUS_MEMORY_MANAGER_H

#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QObject>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QVector>
#endif

#include <memory>

namespace Nexus::Memory {

/**
 * @brief Intelligent memory manager
 * 
 * Uses AI to analyze running processes and optimize
 * memory allocation for the emulated Android system.
 */
class MemoryManager : public QObject
{
    Q_OBJECT
    
public:
    explicit MemoryManager(QObject *parent = nullptr);
    ~MemoryManager();
    
    /**
     * @brief Initialize memory manager
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown memory manager
     */
    void shutdown();
    
    /**
     * @brief Set RAM limit in MB
     */
    void setRamLimit(int limitMb);
    
    /**
     * @brief Get current RAM usage in MB
     */
    int currentUsageMb() const { return m_currentUsageMb; }
    
    /**
     * @brief Get available RAM in MB
     */
    int availableMemory() const;
    
    /**
     * @brief Enable/disable intelligent paging
     */
    void setPagingEnabled(bool enabled);
    
    /**
     * @brief Enable/disable aggressive background killing
     */
    void setAggressiveMode(bool enabled);
    
    /**
     * @brief Add process to whitelist (never kill)
     */
    void addToWhitelist(const QString& packageName);
    
    /**
     * @brief Remove process from whitelist
     */
    void removeFromWhitelist(const QString& packageName);
    
    /**
     * @brief Get memory statistics
     */
    struct MemoryStats {
        int totalMemory = 0;
        int usedMemory = 0;
        int availableMemory = 0;
        int cachedMemory = 0;
        float usagePercent = 0.0f;
    };
    
    MemoryStats getStats() const;

signals:
    void memoryPressureHigh(float usagePercent);
    void processKilled(const QString& packageName);
    void memoryFreed(int freedMb);
    void statsUpdated(MemoryStats stats);
    
private slots:
    void analyzeMemoryUsage();
    void optimizeMemory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Configuration
    int m_ramLimitMb = 8192;
    int m_ramMinMb = 2048;
    bool m_pagingEnabled = true;
    bool m_aggressiveMode = false;
    
    // State
    int m_currentUsageMb = 0;
    
    // Analysis timer
    QTimer* m_analysisTimer;
    
    // Memory analysis
    void analyzeProcesses();
    void killBackgroundProcesses();
    void trimMemory();
    int calculateOptimalAllocation();
};

} // namespace Nexus::Memory

#endif // NEXUS_MEMORY_MANAGER_H
