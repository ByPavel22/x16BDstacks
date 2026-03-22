/**
 * Nexus AI Emulator - Metrics Display
 * 
 * Real-time performance metrics visualization:
 * - FPS graph
 * - AI latency graph
 * - Memory usage
 * - Frame timing
 */

#ifndef NEXUS_METRICS_DISPLAY_H
#define NEXUS_METRICS_DISPLAY_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QVector>
#include <memory>

namespace Nexus::UI {

/**
 * @brief Metrics visualization widget
 * 
 * Displays real-time performance graphs instead of raw numbers.
 */
class MetricsDisplay : public QWidget
{
    Q_OBJECT
    
public:
    explicit MetricsDisplay(QWidget *parent = nullptr);
    ~MetricsDisplay();
    
    /**
     * @brief Initialize metrics display
     */
    bool initialize();
    
    /**
     * @brief Update FPS value
     */
    void updateFps(int fps);
    
    /**
     * @brief Update AI latency value
     */
    void updateAiLatency(float latencyMs);
    
    /**
     * @brief Update memory usage value
     */
    void updateMemoryUsage(int usageMb);
    
    /**
     * @brief Update frame time value
     */
    void updateFrameTime(float frameTimeMs);
    
    /**
     * @brief Set display style
     */
    enum class Style {
        Graph,      // Minimalist graphs
        Numbers,    // Raw numbers
        Minimal     // Compact view
    };
    
    void setStyle(Style style);
    
    /**
     * @brief Set graph color
     */
    void setGraphColor(const QColor& color);
    
    /**
     * @brief Clear all metrics
     */
    void clear();

signals:
    void metricsUpdated();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    void drawGraph(QPainter& painter);
    void drawNumbers(QPainter& painter);
    void drawMinimal(QPainter& painter);
    void updateHistory();
    
    // Current values
    int m_currentFps = 0;
    float m_currentAiLatency = 0.0f;
    int m_currentMemory = 0;
    float m_currentFrameTime = 0.0f;
    
    // History for graphs
    QVector<int> m_fpsHistory;
    QVector<float> m_aiLatencyHistory;
    QVector<int> m_memoryHistory;
    
    static constexpr int MAX_HISTORY = 120;  // 2 seconds at 60 FPS
    
    // Styling
    Style m_style = Style::Graph;
    QColor m_fpsColor = QColor(0, 212, 170);      // Teal
    QColor m_aiColor = QColor(0, 150, 255);       // Blue
    QColor m_memoryColor = QColor(255, 184, 0);   // Amber
};

} // namespace Nexus::UI

#endif // NEXUS_METRICS_DISPLAY_H
