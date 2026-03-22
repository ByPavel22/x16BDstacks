/**
 * Nexus AI Emulator - Inference Engine
 * 
 * Hardware-accelerated inference backend abstraction:
 * - CUDA (NVIDIA)
 * - DirectML (AMD/Intel)
 * - Metal (Apple)
 */

#ifndef NEXUS_INFERENCE_ENGINE_H
#define NEXUS_INFERENCE_ENGINE_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

namespace Nexus::AI {

/**
 * @brief Inference backend type
 */
enum class InferenceBackend {
    CPU,
    CUDA,       // NVIDIA
    DirectML,   // AMD/Intel on Windows
    Metal,      // Apple
    OpenVINO    // Intel
};

/**
 * @brief Hardware inference engine
 * 
 * Provides unified interface for running neural network
 * inference on different hardware backends.
 */
class InferenceEngine : public QObject
{
    Q_OBJECT
    
public:
    explicit InferenceEngine(QObject *parent = nullptr);
    ~InferenceEngine();
    
    /**
     * @brief Initialize inference engine
     * @param backend Preferred backend
     * @return true if initialization successful
     */
    bool initialize(InferenceBackend backend = InferenceBackend::CUDA);
    
    /**
     * @brief Load ONNX model
     * @param modelPath Path to .onnx model file
     * @return true if model loaded successfully
     */
    bool loadModel(const QString& modelPath);
    
    /**
     * @brief Run inference
     * @param inputData Input tensor data (flattened)
     * @param inputShape Input tensor shape [N, C, H, W]
     * @param outputData Output tensor data (flattened)
     * @param outputShape Output tensor shape
     * @return Inference time in milliseconds
     */
    float runInference(const std::vector<float>& inputData,
                       const std::vector<int64_t>& inputShape,
                       std::vector<float>& outputData,
                       const std::vector<int64_t>& outputShape);
    
    /**
     * @brief Get current backend
     */
    InferenceBackend currentBackend() const { return m_backend; }
    
    /**
     * @brief Check if backend is available
     */
    static bool isBackendAvailable(InferenceBackend backend);
    
    /**
     * @brief Get available backends
     */
    static QList<InferenceBackend> availableBackends();
    
    /**
     * @brief Get GPU memory usage in MB
     */
    int gpuMemoryUsage() const;
    
    /**
     * @brief Set precision mode
     */
    void setPrecision(bool fp16 = true);

signals:
    void modelLoaded(const QString& path);
    void inferenceComplete(float latencyMs);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    InferenceBackend m_backend = InferenceBackend::CPU;
    bool m_initialized = false;
    bool m_fp16 = true;
    
    // Backend-specific initialization
    bool initializeCUDA();
    bool initializeDirectML();
    bool initializeMetal();
};

} // namespace Nexus::AI

#endif // NEXUS_INFERENCE_ENGINE_H
