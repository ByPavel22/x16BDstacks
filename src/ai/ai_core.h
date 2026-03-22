/**
 * Nexus AI Emulator - AI Core Module
 * 
 * Central AI processing unit that coordinates:
 * - Frame generation (temporal upscaling)
 * - Texture enhancement
 * - Inference engine management
 */

#ifndef NEXUS_AI_CORE_H
#define NEXUS_AI_CORE_H

#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#include <QObject>
#include <QImage>
#include <QScopedPointer>
#include <QElapsedTimer>
#include <memory>
#endif

namespace Nexus::AI {

/**
 * @brief Main AI Core controller
 * 
 * Orchestrates all AI-powered image enhancement features.
 */
class AICore : public QObject
{
    Q_OBJECT
    
public:
    explicit AICore(QObject *parent = nullptr);
    ~AICore();
    
    /**
     * @brief Initialize AI subsystem
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown AI subsystem
     */
    void shutdown();
    
    /**
     * @brief Enable/disable AI processing
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if AI is enabled
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Process current frame through AI pipeline
     */
    void processFrame();
    
    /**
     * @brief Get last inference latency in milliseconds
     */
    float lastInferenceLatencyMs() const { return m_lastLatencyMs; }
    
    /**
     * @brief Set target resolution for upscaling
     */
    void setTargetResolution(int width, int height);
    
    /**
     * @brief Set internal (render) resolution
     */
    void setInternalResolution(int width, int height);
    
    /**
     * @brief Set target FPS for frame generation
     */
    void setTargetFps(int fps);

signals:
    void frameProcessed(const QImage& output);
    void boostToggled(bool enabled);
    void errorOccurred(const QString& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_enabled = false;
    bool m_initialized = false;
    float m_lastLatencyMs = 0.0f;
    QElapsedTimer m_timer;
    
    // Resolutions
    int m_internalWidth = 1280;
    int m_internalHeight = 720;
    int m_targetWidth = 2560;
    int m_targetHeight = 1440;
    int m_targetFps = 120;
};

} // namespace Nexus::AI

#endif // NEXUS_AI_CORE_H
