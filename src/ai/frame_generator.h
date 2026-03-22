/**
 * Nexus AI Emulator - Frame Generator
 * 
 * High-level frame generation system that combines:
 * - CNN upscaling
 * - Motion vector estimation
 * - Frame interpolation
 */

#ifndef NEXUS_FRAME_GENERATOR_H
#define NEXUS_FRAME_GENERATOR_H

#include <QObject>
#include <QImage>
#include <QQueue>
#include <memory>

#include "cnn_upscaler.h"

namespace Nexus::AI {

/**
 * @brief Frame generation controller
 * 
 * Manages the complete frame generation pipeline:
 * 1. Receives low-res frames from emulator
 * 2. Estimates motion vectors
 * 3. Runs CNN upscaling
 * 4. Generates intermediate frames
 * 5. Outputs high-res frames at target FPS
 */
class FrameGenerator : public QObject
{
    Q_OBJECT
    
public:
    explicit FrameGenerator(QObject *parent = nullptr);
    ~FrameGenerator();
    
    /**
     * @brief Initialize frame generator
     * @param modelPath Path to frame generation model
     * @param targetFps Target output FPS
     * @return true if initialization successful
     */
    bool initialize(const QString& modelPath, int targetFps = 120);
    
    /**
     * @brief Submit a frame for processing
     * @param frame Low-resolution frame from emulator
     */
    void submitFrame(const QImage& frame);
    
    /**
     * @brief Get the next generated frame (if available)
     * @param frame Output frame
     * @return true if frame was available
     */
    bool getNextFrame(QImage& frame);
    
    /**
     * @brief Set target FPS for frame generation
     */
    void setTargetFps(int fps);
    
    /**
     * @brief Get current FPS boost ratio
     */
    float fpsBoostRatio() const { return m_fpsBoostRatio; }
    
    /**
     * @brief Clear frame buffers
     */
    void clear();

signals:
    void frameGenerated(const QImage& frame);
    void latencyMeasured(float ms);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Frame queues
    QQueue<QImage> m_inputQueue;
    QQueue<QImage> m_outputQueue;
    
    // Motion estimation
    QVector<QPoint> estimateMotionVectors(const QImage& frameA, const QImage& frameB);
    
    // Configuration
    int m_targetFps = 120;
    int m_sourceFps = 30;
    float m_fpsBoostRatio = 4.0f;
    
    // Timing
    quint64 m_lastFrameTime = 0;
    float m_frameIntervalMs = 8.33f;  // 120 FPS
};

} // namespace Nexus::AI

#endif // NEXUS_FRAME_GENERATOR_H
