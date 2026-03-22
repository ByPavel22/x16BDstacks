/**
 * Nexus AI Emulator - AOT Shader Compiler
 * 
 * Ahead-of-Time shader compilation:
 * - Pre-compiles known shaders before game starts
 * - Uses shader database from game files
 * - Reduces stutter during gameplay
 */

#ifndef NEXUS_AOT_COMPILER_H
#define NEXUS_AOT_COMPILER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <memory>

namespace Nexus::Shader {

/**
 * @brief AOT compilation result
 */
struct AOTResult {
    bool success = false;
    QString error;
    QByteArray compiledData;
    quint64 compileTimeMs = 0;
};

/**
 * @brief AOT shader compiler
 */
class AOTCompiler : public QObject
{
    Q_OBJECT
    
public:
    explicit AOTCompiler(QObject *parent = nullptr);
    ~AOTCompiler();
    
    /**
     * @brief Initialize AOT compiler
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Extract shaders from game APK
     * @param apkPath Path to APK file
     * @return List of shader source codes
     */
    QVector<QString> extractShadersFromApk(const QString& apkPath);
    
    /**
     * @brief Compile shader to target format
     * @param source Shader source code
     * @param sourceLanguage Source language (glsl, hlsl)
     * @param targetLanguage Target language (spirv, dxil, air)
     * @return Compilation result
     */
    AOTResult compileShader(const QString& source,
                            const QString& sourceLanguage,
                            const QString& targetLanguage);
    
    /**
     * @brief Batch compile multiple shaders
     * @param shaders List of shader sources
     * @param workerCount Number of parallel workers
     * @return Results for each shader
     */
    QVector<AOTResult> batchCompile(const QVector<QString>& shaders,
                                     int workerCount = 4);
    
    /**
     * @brief Load shader database from file
     * @param databasePath Path to shader database
     * @return true if loaded successfully
     */
    bool loadShaderDatabase(const QString& databasePath);
    
    /**
     * @brief Get known shaders for game
     * @param gamePackage Game package name
     * @return List of known shader hashes
     */
    QVector<QString> getKnownShadersForGame(const QString& gamePackage);

signals:
    void compilationProgress(int current, int total);
    void compilationComplete();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Compilation
    AOTResult compileGlslToSpirv(const QString& source);
    AOTResult compileHlslToDxil(const QString& source);
    AOTResult compileToMetal(const QString& source);
    
    // Database
    void buildShaderDatabase(const QString& gamePackage);
};

} // namespace Nexus::Shader

#endif // NEXUS_AOT_COMPILER_H
