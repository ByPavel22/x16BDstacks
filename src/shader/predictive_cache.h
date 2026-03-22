/**
 * Nexus AI Emulator - Predictive Shader Cache
 * 
 * AI-driven shader preloading:
 * - Analyzes gameplay patterns
 * - Predicts which shaders will be needed
 * - Pre-compiles during loading screens
 */

#ifndef NEXUS_PREDICTIVE_CACHE_H
#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#define NEXUS_PREDICTIVE_CACHE_H

#include <QObject>
#endif
#include <QMap>
#endif
#include <QVector>
#endif
#include <memory>

namespace Nexus::Shader {

/**
 * @brief Shader usage pattern
 */
struct UsagePattern {
    QString shaderHash;
    QVector<qint64> usageTimestamps;
    QVector<QString> gameContexts;  // Level, scene, etc.
    int totalUses = 0;
    float averageInterval = 0.0f;  // Average time between uses
};

/**
 * @brief Prediction result
 */
struct Prediction {
    QString shaderHash;
    float probability = 0.0f;  // 0.0-1.0
    QString reason;  // Why this shader was predicted
};

/**
 * @brief Predictive shader cache
 */
class PredictiveCache : public QObject
{
    Q_OBJECT
    
public:
    explicit PredictiveCache(QObject *parent = nullptr);
    ~PredictiveCache();
    
    /**
     * @brief Initialize predictive cache
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Record shader usage
     * @param shaderHash Shader hash
     * @param context Current game context (level, scene, etc.)
     */
    void recordUsage(const QString& shaderHash, const QString& context = QString());
    
    /**
     * @brief Predict shaders needed soon
     * @param count Maximum number of predictions
     * @return List of predicted shaders with probabilities
     */
    QVector<Prediction> predictNextShaders(int count = 10);
    
    /**
     * @brief Set current game context
     * @param context Context identifier
     */
    void setCurrentContext(const QString& context);
    
    /**
     * @brief Get context-specific predictions
     * @param context Context identifier
     * @param count Maximum predictions
     * @return Predicted shaders for context
     */
    QVector<Prediction> getPredictionsForContext(const QString& context, int count = 10);
    
    /**
     * @brief Enable/disable AI prediction
     */
    void setAiPredictionEnabled(bool enabled);
    
    /**
     * @brief Clear usage history
     */
    void clearHistory();
    
    /**
     * @brief Export usage data for training
     * @param outputPath Path to export file
     * @return true if export successful
     */
    bool exportTrainingData(const QString& outputPath);

signals:
    void predictionReady(const QVector<Prediction>& predictions);
    void patternLearned(const QString& shaderHash);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    bool m_aiPredictionEnabled = true;
    
    QString m_currentContext;
    
    // Usage tracking
    QMap<QString, UsagePattern> m_usagePatterns;
    
    // Context mapping
    QMap<QString, QVector<QString>> m_contextShaders;
    
    // Prediction
    void analyzePatterns();
    float calculateUsageProbability(const UsagePattern& pattern);
    float calculateContextProbability(const QString& shaderHash, 
                                       const QString& context);
    
    // ML model (stub)
    float runMlPrediction(const UsagePattern& pattern, const QString& context);
};

} // namespace Nexus::Shader

#endif // NEXUS_PREDICTIVE_CACHE_H
