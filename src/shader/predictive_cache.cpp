#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#endif
/**
 * Nexus AI Emulator - Predictive Cache Implementation
 */

#include "predictive_cache.h"
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QDebug>
#endif
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

namespace Nexus::Shader {

/**
 * @brief Private implementation for PredictiveCache
 */
struct PredictiveCache::Impl
{
    // ML model state (stub)
    bool modelLoaded = false;
    QString modelPath;
    
    // Pattern analysis settings
    int minSamplesForPrediction = 5;
    float timeDecayFactor = 0.9f;  // Older usage counts less
    int maxHistoryPerShader = 100;
    
    // Statistics
    int totalPredictions = 0;
    int correctPredictions = 0;
    float predictionAccuracy = 0.0f;
    
    // Export data
    QVector<QVariantMap> trainingData;
};

PredictiveCache::PredictiveCache(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

PredictiveCache::~PredictiveCache()
{
}

bool PredictiveCache::initialize()
{
    qDebug() << "[PredictiveCache] Initializing...";
    
    m_impl->modelPath = "models/shader_predictor.onnx";
    
    // Try to load ML model
    m_impl->modelLoaded = QFile::exists(m_impl->modelPath);
    
    if (!m_impl->modelLoaded) {
        qWarning() << "[PredictiveCache] ML model not found, using heuristics";
    } else {
        qDebug() << "[PredictiveCache] ML model loaded";
    }
    
    m_initialized = true;
    qDebug() << "[PredictiveCache] Initialization complete";
    
    return true;
}

void PredictiveCache::recordUsage(const QString& shaderHash, const QString& context)
{
    if (!m_initialized) {
        return;
    }
    
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    
    // Get or create pattern
    UsagePattern& pattern = m_usagePatterns[shaderHash];
    pattern.shaderHash = shaderHash;
    pattern.totalUses++;
    
    // Add timestamp
    pattern.usageTimestamps.append(timestamp);
    
    // Trim history
    if (pattern.usageTimestamps.size() > m_impl->maxHistoryPerShader) {
        pattern.usageTimestamps.removeFirst();
    }
    
    // Calculate average interval
    if (pattern.usageTimestamps.size() >= 2) {
        qint64 totalInterval = 0;
        for (int i = 1; i < pattern.usageTimestamps.size(); ++i) {
            totalInterval += pattern.usageTimestamps[i] - pattern.usageTimestamps[i-1];
        }
        pattern.averageInterval = static_cast<float>(totalInterval) / 
                                   (pattern.usageTimestamps.size() - 1);
    }
    
    // Add context
    if (!context.isEmpty()) {
        pattern.gameContexts.append(context);
        m_contextShaders[context].append(shaderHash);
        
        if (context != m_currentContext) {
            m_currentContext = context;
        }
    }
    
    // Add to training data
    if (m_impl->trainingData.size() < 100000) {
        QVariantMap record;
        record["shader"] = shaderHash;
        record["timestamp"] = timestamp;
        record["context"] = context;
        m_impl->trainingData.append(record);
    }
    
    emit patternLearned(shaderHash);
}

QVector<Prediction> PredictiveCache::predictNextShaders(int count)
{
    if (!m_initialized) {
        return QVector<Prediction>();
    }
    
    QVector<Prediction> predictions;
    
    // Analyze patterns first
    analyzePatterns();
    
    // Score each shader
    QList<QPair<QString, float>> scored;
    
    for (auto it = m_usagePatterns.begin(); it != m_usagePatterns.end(); ++it) {
        float probability = calculateUsageProbability(it.value());
        
        // Boost probability for current context
        if (!m_currentContext.isEmpty()) {
            float contextProb = calculateContextProbability(it.key(), m_currentContext);
            probability = (probability + contextProb) / 2.0f;
        }
        
        // Use ML model if available
        if (m_impl->modelLoaded && m_aiPredictionEnabled) {
            float mlProb = runMlPrediction(it.value(), m_currentContext);
            probability = (probability + mlProb) / 2.0f;
        }
        
        Prediction pred;
        pred.shaderHash = it.key();
        pred.probability = probability;
        scored.append(qMakePair(it.key(), probability));
    }
    
    // Sort by probability
    std::sort(scored.begin(), scored.end(),
              [](const QPair<QString, float>& a, const QPair<QString, float>& b) {
                  return a.second > b.second;
              });
    
    // Take top predictions
    for (int i = 0; i < qMin(count, scored.size()); ++i) {
        Prediction pred;
        pred.shaderHash = scored[i].first;
        pred.probability = scored[i].second;
        pred.reason = "Based on usage pattern analysis";
        
        predictions.append(pred);
    }
    
    m_impl->totalPredictions++;
    
    emit predictionReady(predictions);
    
    return predictions;
}

void PredictiveCache::setCurrentContext(const QString& context)
{
    m_currentContext = context;
    qDebug() << "[PredictiveCache] Context changed to:" << context;
}

QVector<Prediction> PredictiveCache::getPredictionsForContext(const QString& context, 
                                                               int count)
{
    QString previousContext = m_currentContext;
    m_currentContext = context;
    
    QVector<Prediction> predictions = predictNextShaders(count);
    
    m_currentContext = previousContext;
    
    return predictions;
}

void PredictiveCache::setAiPredictionEnabled(bool enabled)
{
    m_aiPredictionEnabled = enabled;
    qDebug() << "[PredictiveCache] AI prediction:" << (enabled ? "enabled" : "disabled");
}

void PredictiveCache::clearHistory()
{
    m_usagePatterns.clear();
    m_contextShaders.clear();
    m_impl->trainingData.clear();
    
    qDebug() << "[PredictiveCache] History cleared";
}

bool PredictiveCache::exportTrainingData(const QString& outputPath)
{
    qDebug() << "[PredictiveCache] Exporting training data to:" << outputPath;
    
    QJsonArray dataArray;
    
    for (const QVariantMap& record : m_impl->trainingData) {
        QJsonObject obj;
        obj["shader"] = record["shader"].toString();
        obj["timestamp"] = record["timestamp"].toLongLong();
        obj["context"] = record["context"].toString();
        dataArray.append(obj);
    }
    
    QJsonDocument doc(dataArray);
    
    QFile file(outputPath);
    if (!file.open(QFile::WriteOnly)) {
        qCritical() << "[PredictiveCache] Failed to open output file";
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    qDebug() << "[PredictiveCache] Exported" << dataArray.size() << "records";
    
    return true;
}

void PredictiveCache::analyzePatterns()
{
    // Analyze usage patterns for all shaders
    // This is called before making predictions
    
    for (auto it = m_usagePatterns.begin(); it != m_usagePatterns.end(); ++it) {
        UsagePattern& pattern = it.value();
        
        // Apply time decay to older usage
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        float decayMultiplier = 1.0f;
        
        for (int i = pattern.usageTimestamps.size() - 1; i >= 0; --i) {
            qint64 age = currentTime - pattern.usageTimestamps[i];
            float ageFactor = qPow(m_impl->timeDecayFactor, age / 60000.0f);  // Per minute
            decayMultiplier *= ageFactor;
        }
    }
}

float PredictiveCache::calculateUsageProbability(const UsagePattern& pattern)
{
    if (pattern.totalUses < m_impl->minSamplesForPrediction) {
        return 0.0f;  // Not enough data
    }
    
    float probability = 0.0f;
    
    // Factor 1: Total usage count (more uses = more likely)
    float countFactor = qMin(1.0f, pattern.totalUses / 100.0f);
    
    // Factor 2: Recency (recently used = more likely)
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeSinceLastUse = currentTime - pattern.usageTimestamps.last();
    float recencyFactor = qPow(0.999f, timeSinceLastUse / 1000.0f);  // Decay per second
    
    // Factor 3: Frequency (frequent use = more likely)
    float frequencyFactor = 0.0f;
    if (pattern.averageInterval > 0) {
        // Shorter interval = higher frequency = more likely
        frequencyFactor = qMin(1.0f, 60000.0f / pattern.averageInterval);
    }
    
    // Combine factors
    probability = (countFactor * 0.3f + recencyFactor * 0.4f + frequencyFactor * 0.3f);
    
    return qBound(0.0f, probability, 1.0f);
}

float PredictiveCache::calculateContextProbability(const QString& shaderHash, 
                                                    const QString& context)
{
    auto it = m_contextShaders.find(context);
    if (it == m_contextShaders.end()) {
        return 0.0f;
    }
    
    const QVector<QString>& contextShaders = it.value();
    
    // Count occurrences of this shader in this context
    int occurrences = std::count(contextShaders.begin(), 
                                  contextShaders.end(), 
                                  shaderHash);
    
    if (occurrences == 0) {
        return 0.0f;
    }
    
    // More occurrences = higher probability
    float probability = qMin(1.0f, occurrences / 10.0f);
    
    return probability;
}

float PredictiveCache::runMlPrediction(const UsagePattern& pattern, 
                                        const QString& context)
{
    if (!m_impl->modelLoaded) {
        return 0.0f;
    }
    
    // Stub ML prediction
    // Real implementation would:
    // 1. Prepare input features (usage timestamps, intervals, context)
    // 2. Run ONNX model inference
    // 3. Return probability
    
    Q_UNUSED(pattern);
    Q_UNUSED(context);
    
    // Return random probability for stub
    return 0.5f;
}

} // namespace Nexus::Shader
