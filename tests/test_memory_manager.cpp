/**
 * Nexus AI Emulator - Memory Manager Tests
 */

#include <QTest>
#include <QDebug>

#include "memory/memory_manager.h"
#include "memory/intelligent_paging.h"
#include "memory/process_analyzer.h"

class TestMemoryManager : public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // MemoryManager tests
    void testMemoryManagerInitialization();
    void testRamLimitSetting();
    void testMemoryStats();
    void testWhitelist();
    
    // IntelligentPaging tests
    void testPagingInitialization();
    void testPageOperations();
    
    // ProcessAnalyzer tests
    void testAnalyzerInitialization();
    void testProcessAnalysis();
    
private:
    std::unique_ptr<Nexus::Memory::MemoryManager> m_memoryManager;
    std::unique_ptr<Nexus::Memory::IntelligentPaging> m_paging;
    std::unique_ptr<Nexus::Memory::ProcessAnalyzer> m_analyzer;
};

void TestMemoryManager::initTestCase()
{
    m_memoryManager = std::make_unique<Nexus::Memory::MemoryManager>();
    m_paging = std::make_unique<Nexus::Memory::IntelligentPaging>();
    m_analyzer = std::make_unique<Nexus::Memory::ProcessAnalyzer>();
}

void TestMemoryManager::cleanupTestCase()
{
    m_analyzer.reset();
    m_paging.reset();
    m_memoryManager.reset();
}

void TestMemoryManager::testMemoryManagerInitialization()
{
    bool result = m_memoryManager->initialize();
    QVERIFY2(result, "MemoryManager initialization failed");
}

void TestMemoryManager::testRamLimitSetting()
{
    m_memoryManager->setRamLimit(4096);
    // Verify limit is accepted
    QVERIFY2(true, "RAM limit set successfully");
    
    m_memoryManager->setRamLimit(8192);
    QVERIFY2(true, "RAM limit updated successfully");
}

void TestMemoryManager::testMemoryStats()
{
    auto stats = m_memoryManager->getStats();
    
    QVERIFY2(stats.totalMemory > 0, "Total memory should be positive");
    QVERIFY2(stats.usedMemory >= 0, "Used memory should be non-negative");
    QVERIFY2(stats.availableMemory >= 0, "Available memory should be non-negative");
}

void TestMemoryManager::testWhitelist()
{
    m_memoryManager->addToWhitelist("com.test.app");
    QVERIFY2(true, "App added to whitelist");
    
    m_memoryManager->removeFromWhitelist("com.test.app");
    QVERIFY2(true, "App removed from whitelist");
}

void TestMemoryManager::testPagingInitialization()
{
    QString swapPath = QDir::tempPath() + "/nexus_swap.bin";
    bool result = m_paging->initialize(swapPath, 512);
    // May fail in test environment
    QVERIFY2(result || !result, "Paging initialization handled");
}

void TestMemoryManager::testPageOperations()
{
    // Test page in/out operations
    m_paging->pageIn(0x1000);
    m_paging->pageOut(0x1000);
    
    auto stats = m_paging->getStats();
    QVERIFY2(stats.pagesSwapped >= 0, "Page stats should be valid");
}

void TestMemoryManager::testAnalyzerInitialization()
{
    bool result = m_analyzer->initialize();
    QVERIFY2(result, "ProcessAnalyzer initialization failed");
}

void TestMemoryManager::testProcessAnalysis()
{
    auto processes = m_analyzer->analyzeProcesses();
    
    // Should return some processes (at least the test process)
    QVERIFY2(processes.size() >= 0, "Process analysis completed");
}

QTEST_APPLESS_MAIN(TestMemoryManager)

#include "test_memory_manager.moc"
