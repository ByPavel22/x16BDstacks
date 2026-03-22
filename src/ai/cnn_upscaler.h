/**
 * Nexus AI Emulator - CNN Frame Generator
 * 
 * Temporal upscaling neural network for frame generation.
 * Takes low-resolution frames + motion vectors + frame history
 * and generates high-resolution frames with interpolated motion.
 */

#ifndef NEXUS_CNN_UPSCALER_H
#define NEXUS_CNN_UPSCALER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <memory>

namespace Nexus::AI {

/**
 * @brief Temporal CNN upscaler for frame generation
 * 
 * Implements DLSS/FSR-like functionality:
 * - Takes multiple low-res frames as input
 * - Uses motion vectors for temporal stability
 * - Generates high-res output with super-resolution
 * - Creates intermediate frames for higher FPS
 */
class CNNUpscaler : public QObject
{
    Q_OBJECT
    
public:
    explicit CNNUpscaler(QObject *parent = nullptr);
    ~CNNUpscaler();
    
    /**
     * @brief Initialize the upscaler with model path
     * @param modelPath Path to ONNX model file
     * @param useCUDA Use CUDA backend if available
     * @return true if initialization successful
     */
    bool initialize(const QString& modelPath, bool useCUDA = true);
    
    /**
     * @brief Process a frame through the upscaler
     * @param inputFrame Low-resolution input frame
     * @param motionVectors Optional motion vectors for temporal stability
     * @return Upscaled high-resolution frame
     */
    QImage upscale(const QImage& inputFrame, 
                   const QVector<QPoint>& motionVectors = QVector<QPoint>());
    
    /**
     * @brief Generate intermediate frames for FPS boost
     * @param frameA Previous frame
     * @param frameB Current frame
     * @param t Interpolation factor (0.0-1.0)
     * @return Interpolated frame
     */
    QImage generateIntermediateFrame(const QImage& frameA, 
                                     const QImage& frameB, 
                                     float t);
    
    /**
     * @brief Set output resolution
     */
    void setOutputResolution(int width, int height);
    
    /**
     * @brief Set input resolution
     */
    void setInputResolution(int width, int height);
    
    /**
     * @brief Get inference latency in milliseconds
     */
    float lastLatencyMs() const { return m_lastLatencyMs; }
    
    /**
     * @brief Check if upscaler is ready
     */
    bool isReady() const { return m_initialized; }

signals:
    void processingComplete(const QImage& output);
    void errorOccurred(const QString& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    float m_lastLatencyMs = 0.0f;
    
    int m_inputWidth = 1280;
    int m_inputHeight = 720;
    int m_outputWidth = 2560;
    int m_outputHeight = 1440;
    
    // Frame history for temporal processing
    QVector<QImage> m_frameHistory;
    static constexpr int MAX_HISTORY = 4;
};

} // namespace Nexus::AI

#endif // NEXUS_CNN_UPSCALER_H
