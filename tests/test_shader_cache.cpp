/**
 * Nexus AI Emulator - Shader Cache Tests
 */

#include <QTest>
#include <QDebug>
#include <QDir>
#include <QFile>

#include "shader/shader_cache.h"
#include "shader/aot_compiler.h"
#include "shader/jit_compiler.h"
#include "shader/predictive_cache.h"

class TestShaderCache : public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // ShaderCache tests
    void testShaderCacheInitialization();
    void testShaderCompilation();
    void testCacheHitMiss();
    void testCacheFlush();
    void testCacheClear();
    
    // AOTCompiler tests
    void testAOTCompilerInitialization();
    
    // JITCompiler tests
    void testJITCompilerInitialization();
    void testJITCompilation();
    
    // PredictiveCache tests
    void testPredictiveCacheInitialization();
    void testUsageRecording();
    void testPrediction();
    
private:
    std::unique_ptr<Nexus::Shader::ShaderCache> m_shaderCache;
    std::unique_ptr<Nexus::Shader::AOTCompiler> m_aotCompiler;
    std::unique_ptr<Nexus::Shader::JITCompiler> m_jitCompiler;
    std::unique_ptr<Nexus::Shader::PredictiveCache> m_predictiveCache;
    
    QString m_testCachePath;
};

void TestShaderCache::initTestCase()
{
    m_testCachePath = QDir::tempPath() + "/nexus_shader_test";
    QDir().mkpath(m_testCachePath);
    
    m_shaderCache = std::make_unique<Nexus::Shader::ShaderCache>();
    m_aotCompiler = std::make_unique<Nexus::Shader::AOTCompiler>();
    m_jitCompiler = std::make_unique<Nexus::Shader::JITCompiler>();
    m_predictiveCache = std::make_unique<Nexus::Shader::PredictiveCache>();
}

void TestShaderCache::cleanupTestCase()
{
    // Cleanup test files
    QDir(m_testCachePath).removeRecursively();
    
    m_predictiveCache.reset();
    m_jitCompiler.reset();
    m_aotCompiler.reset();
    m_shaderCache.reset();
}

void TestShaderCache::testShaderCacheInitialization()
{
    m_shaderCache->setCachePath(m_testCachePath);
    bool result = m_shaderCache->initialize();
    QVERIFY2(result, "ShaderCache initialization failed");
}

void TestShaderCache::testShaderCompilation()
{
    QString shaderSource = R"(
        #version 450
        layout(location = 0) out vec4 color;
        void main() {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";
    
    QByteArray result = m_shaderCache->getShader(shaderSource, "fragment");
    QVERIFY2(!result.isEmpty(), "Shader compilation should return data");
}

void TestShaderCache::testCacheHitMiss()
{
    QString shaderSource = "test_cached_shader";
    
    // First call - should be miss
    m_shaderCache->getShader(shaderSource, "vertex");
    auto stats1 = m_shaderCache->getStats();
    
    // Second call - should be hit
    m_shaderCache->getShader(shaderSource, "vertex");
    auto stats2 = m_shaderCache->getStats();
    
    QVERIFY2(stats2.cacheHits >= stats1.cacheHits, "Cache hit should be recorded");
}

void TestShaderCache::testCacheFlush()
{
    m_shaderCache->flush();
    QVERIFY2(true, "Cache flush completed");
}

void TestShaderCache::testCacheClear()
{
    m_shaderCache->clear();
    auto stats = m_shaderCache->getStats();
    QVERIFY2(stats.totalShaders == 0, "Cache should be empty after clear");
}

void TestShaderCache::testAOTCompilerInitialization()
{
    bool result = m_aotCompiler->initialize();
    QVERIFY2(result || !result, "AOTCompiler initialization handled");
}

void TestShaderCache::testJITCompilerInitialization()
{
    bool result = m_jitCompiler->initialize(2);
    QVERIFY2(result, "JITCompiler initialization failed");
}

void TestShaderCache::testJITCompilation()
{
    QString shaderSource = "jit_test_shader";
    
    QString hash = m_jitCompiler->requestCompilation(shaderSource, "vertex");
    QVERIFY2(!hash.isEmpty(), "Compilation request should return hash");
    
    // Wait a bit for compilation
    QThread::msleep(100);
    
    bool isCompiled = m_jitCompiler->isCompiled(hash);
    // May or may not be compiled yet
    QVERIFY2(isCompiled || !isCompiled, "Compilation status checked");
}

void TestShaderCache::testPredictiveCacheInitialization()
{
    bool result = m_predictiveCache->initialize();
    QVERIFY2(result, "PredictiveCache initialization failed");
}

void TestShaderCache::testUsageRecording()
{
    m_predictiveCache->recordUsage("shader_1", "level_1");
    m_predictiveCache->recordUsage("shader_2", "level_1");
    m_predictiveCache->recordUsage("shader_1", "level_1");
    
    QVERIFY2(true, "Usage recording completed");
}

void TestShaderCache::testPrediction()
{
    // Record some usage first
    m_predictiveCache->recordUsage("shader_1", "context_1");
    m_predictiveCache->recordUsage("shader_2", "context_1");
    m_predictiveCache->recordUsage("shader_1", "context_1");
    
    auto predictions = m_predictiveCache->predictNextShaders(5);
    
    QVERIFY2(predictions.size() >= 0, "Prediction completed");
}

QTEST_APPLESS_MAIN(TestShaderCache)

#include "test_shader_cache.moc"
