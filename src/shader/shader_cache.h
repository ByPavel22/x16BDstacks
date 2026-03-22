/**
 * Nexus AI Emulator - Shader Cache
 * 
 * Hybrid shader caching system with:
 * - AOT (Ahead-of-Time) compilation
 * - JIT (Just-in-Time) compilation
 * - Predictive caching based on usage patterns
 */

#ifndef NEXUS_SHADER_CACHE_H
#define NEXUS_SHADER_CACHE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <memory>

namespace Nexus::Shader {

/**
 * @brief Shader compilation mode
 */
enum class CompilationMode {
    AOT,    // Pre-compile before use
    JIT,    // Compile on first use
    Hybrid  // AOT for known shaders, JIT for new
};

/**
 * @brief Shader cache entry
 */
struct ShaderEntry {
    QString shaderHash;
    QByteArray compiledData;
    quint64 createTime = 0;
    quint64 lastUseTime = 0;
    int useCount = 0;
    bool isPrecompiled = false;
    QString sourceLanguage;  // glsl, hlsl, msl
    QString targetLanguage;  // spirv, dxil, air
};

/**
 * @brief Main shader cache controller
 */
class ShaderCache : public QObject
{
    Q_OBJECT
    
public:
    explicit ShaderCache(QObject *parent = nullptr);
    ~ShaderCache();
    
    /**
     * @brief Initialize shader cache
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown shader cache
     */
    void shutdown();
    
    /**
     * @brief Set cache path
     */
    void setCachePath(const QString& path);
    
    /**
     * @brief Get compiled shader or compile if needed
     * @param shaderSource Shader source code
     * @param shaderType Type of shader (vertex, fragment, compute)
     * @return Compiled shader data
     */
    QByteArray getShader(const QString& shaderSource, 
                         const QString& shaderType);
    
    /**
     * @brief Pre-compile shader for AOT
     * @param shaderSource Shader source code
     * @param shaderType Type of shader
     * @return true if compilation successful
     */
    bool precompileShader(const QString& shaderSource, 
                          const QString& shaderType);
    
    /**
     * @brief Clear shader cache
     */
    void clear();
    
    /**
     * @brief Flush cache to disk
     */
    void flush();
    
    /**
     * @brief Set cache size limit in MB
     */
    void setCacheSizeLimit(int limitMb);
    
    /**
     * @brief Enable/disable predictive caching
     */
    void setPredictiveCachingEnabled(bool enabled);
    
    /**
     * @brief Get cache statistics
     */
    struct CacheStats {
        int totalShaders = 0;
        int compiledShaders = 0;
        int cacheHits = 0;
        int cacheMisses = 0;
        float hitRate = 0.0f;
        int cacheSizeMb = 0;
    };
    
    CacheStats getStats() const { return m_stats; }

signals:
    void shaderCompiled(const QString& shaderHash);
    void cacheFlushed();
    void cacheCleared();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    CacheStats m_stats;
    
    // Cache management
    QString m_cachePath;
    int m_cacheSizeLimitMb = 2048;
    bool m_predictiveCaching = true;
    
    // Shader compilation
    QByteArray compileShader(const QString& source, 
                             const QString& type,
                             CompilationMode mode);
    
    QString calculateHash(const QString& source);
    bool loadFromCache(const QString& hash);
    void saveToCache(const QString& hash, const QByteArray& data);
    
    // Predictive caching
    void analyzeUsagePatterns();
    void preloadLikelyShaders();
};

} // namespace Nexus::Shader

#endif // NEXUS_SHADER_CACHE_H
