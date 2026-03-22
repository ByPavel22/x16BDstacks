/**
 * Nexus AI Emulator - JIT Shader Compiler
 * 
 * Just-in-Time shader compilation:
 * - Compiles shaders on first use
 * - Async compilation to avoid stutter
 * - Fallback shaders while compiling
 */

#ifndef NEXUS_JIT_COMPILER_H
#define NEXUS_JIT_COMPILER_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QFuture>
#include <memory>

namespace Nexus::Shader {

/**
 * @brief JIT compilation state
 */
enum class CompilationState {
    Pending,      // Waiting to compile
    Compiling,    // Currently compiling
    Complete,     // Compilation finished
    Failed        // Compilation failed
};

/**
 * @brief JIT shader compilation request
 */
struct JITRequest {
    QString shaderHash;
    QString shaderSource;
    QString shaderType;
    CompilationState state = CompilationState::Pending;
    QFuture<void> compileFuture;
    qint64 requestTime = 0;
};

/**
 * @brief JIT shader compiler
 */
class JITCompiler : public QObject
{
    Q_OBJECT
    
public:
    explicit JITCompiler(QObject *parent = nullptr);
    ~JITCompiler();
    
    /**
     * @brief Initialize JIT compiler
     * @param workerThreads Number of compilation threads
     * @return true if initialization successful
     */
    bool initialize(int workerThreads = 4);
    
    /**
     * @brief Request shader compilation
     * @param source Shader source code
     * @param type Shader type (vertex, fragment, compute)
     * @param priority Compilation priority (0-10, higher = more urgent)
     * @return Shader hash for tracking
     */
    QString requestCompilation(const QString& source,
                               const QString& type,
                               int priority = 5);
    
    /**
     * @brief Check if shader is compiled
     * @param shaderHash Shader hash
     * @return true if compilation complete
     */
    bool isCompiled(const QString& shaderHash) const;
    
    /**
     * @brief Get compiled shader data
     * @param shaderHash Shader hash
     * @return Compiled shader data (empty if not ready)
     */
    QByteArray getCompiledShader(const QString& shaderHash) const;
    
    /**
     * @brief Get compilation state
     * @param shaderHash Shader hash
     * @return Current compilation state
     */
    CompilationState getCompilationState(const QString& shaderHash) const;
    
    /**
     * @brief Set compilation priority
     * @param shaderHash Shader hash
     * @param priority New priority (0-10)
     */
    void setCompilationPriority(const QString& shaderHash, int priority);
    
    /**
     * @brief Cancel pending compilation
     * @param shaderHash Shader hash
     * @return true if cancelled
     */
    bool cancelCompilation(const QString& shaderHash);
    
    /**
     * @brief Get pending compilation count
     */
    int pendingCount() const { return m_pendingQueue.size(); }
    
    /**
     * @brief Get compiling count
     */
    int compilingCount() const { return m_compiling.size(); }

signals:
    void compilationStarted(const QString& shaderHash);
    void compilationFinished(const QString& shaderHash, bool success);
    void compilationFailed(const QString& shaderHash, const QString& error);
    
private slots:
    void processCompilationQueue();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Compilation queues
    QQueue<JITRequest> m_pendingQueue;
    QMap<QString, JITRequest> m_compiling;
    QMap<QString, QByteArray> m_completed;
    
    // Worker threads
    int m_workerThreads = 4;
    
    // Compilation
    void startCompilation(JITRequest& request);
    QByteArray compileShaderSync(const QString& source, const QString& type);
    
    // Fallback shaders
    QByteArray getFallbackShader(const QString& type) const;
};

} // namespace Nexus::Shader

#endif // NEXUS_JIT_COMPILER_H
