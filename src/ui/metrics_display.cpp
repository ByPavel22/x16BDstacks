/**
 * Nexus AI Emulator - Metrics Display Implementation
 */

#include "metrics_display.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QDebug>

namespace Nexus::UI {

/**
 * @brief Private implementation for MetricsDisplay
 */
struct MetricsDisplay::Impl
{
    bool initialized = false;
    
    // Graph settings
    int graphHeight = 40;
    int graphSpacing = 5;
    int lineThickness = 2;
    
    // Grid settings
    int gridLines = 4;
    QColor gridColor = QColor(255, 255, 255, 30);
    
    // Value ranges for scaling
    float maxFps = 144.0f;
    float maxAiLatency = 10.0f;  // ms
    float maxMemory = 16384.0f;  // MB
};

MetricsDisplay::MetricsDisplay(QWidget *parent)
    : QWidget(parent)
    , m_impl(std::make_unique<Impl>())
{
    setMinimumSize(300, 80);
    setMaximumHeight(120);
    
    // Initialize history buffers
    m_fpsHistory.reserve(MAX_HISTORY);
    m_aiLatencyHistory.reserve(MAX_HISTORY);
    m_memoryHistory.reserve(MAX_HISTORY);
}

MetricsDisplay::~MetricsDisplay()
{
}

bool MetricsDisplay::initialize()
{
    m_impl->initialized = true;
    return true;
}

void MetricsDisplay::updateFps(int fps)
{
    m_currentFps = fps;
    updateHistory();
    update();
    emit metricsUpdated();
}

void MetricsDisplay::updateAiLatency(float latencyMs)
{
    m_currentAiLatency = latencyMs;
    updateHistory();
    update();
    emit metricsUpdated();
}

void MetricsDisplay::updateMemoryUsage(int usageMb)
{
    m_currentMemory = usageMb;
    updateHistory();
    update();
    emit metricsUpdated();
}

void MetricsDisplay::updateFrameTime(float frameTimeMs)
{
    m_currentFrameTime = frameTimeMs;
    update();
}

void MetricsDisplay::setStyle(Style style)
{
    m_style = style;
    update();
}

void MetricsDisplay::setGraphColor(const QColor& color)
{
    m_fpsColor = color;
    update();
}

void MetricsDisplay::clear()
{
    m_fpsHistory.clear();
    m_aiLatencyHistory.clear();
    m_memoryHistory.clear();
    
    m_currentFps = 0;
    m_currentAiLatency = 0.0f;
    m_currentMemory = 0;
    
    update();
}

void MetricsDisplay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    // Clear background
    painter.fillRect(rect(), QColor(20, 20, 30, 180));
    
    // Draw based on style
    switch (m_style) {
        case Style::Graph:
            drawGraph(painter);
            break;
        case Style::Numbers:
            drawNumbers(painter);
            break;
        case Style::Minimal:
            drawMinimal(painter);
            break;
    }
}

void MetricsDisplay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void MetricsDisplay::updateHistory()
{
    // Add current values to history
    m_fpsHistory.append(m_currentFps);
    m_aiLatencyHistory.append(m_currentAiLatency);
    m_memoryHistory.append(m_currentMemory);
    
    // Trim to max size
    while (m_fpsHistory.size() > MAX_HISTORY) {
        m_fpsHistory.removeFirst();
    }
    while (m_aiLatencyHistory.size() > MAX_HISTORY) {
        m_aiLatencyHistory.removeFirst();
    }
    while (m_memoryHistory.size() > MAX_HISTORY) {
        m_memoryHistory.removeFirst();
    }
}

void MetricsDisplay::drawGraph(QPainter& painter)
{
    const int labelWidth = 60;
    const int graphWidth = width() - labelWidth - 20;
    const int yPos_FPS = 25;
    const int yPos_AI = 55;
    const int yPos_MEM = 85;
    
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);
    
    // Draw labels
    painter.setPen(m_fpsColor);
    painter.drawText(5, yPos_FPS + 12, "FPS");
    
    painter.setPen(m_aiColor);
    painter.drawText(5, yPos_AI + 12, "AI ms");
    
    painter.setPen(m_memoryColor);
    painter.drawText(5, yPos_MEM + 12, "RAM");
    
    // Draw FPS graph
    drawSingleGraph(painter, labelWidth, yPos_FPS, graphWidth, 
                    m_fpsHistory, m_fpsColor, m_impl->maxFps);
    
    // Draw AI Latency graph
    drawSingleGraph(painter, labelWidth, yPos_AI, graphWidth,
                    m_aiLatencyHistory, m_aiColor, m_impl->maxAiLatency);
    
    // Draw Memory graph
    drawSingleGraph(painter, labelWidth, yPos_MEM, graphWidth,
                    m_memoryHistory, m_memoryColor, m_impl->maxMemory);
}

void MetricsDisplay::drawSingleGraph(QPainter& painter, 
                                      int xOffset, 
                                      int yOffset, 
                                      int width,
                                      const QVector<float>& data,
                                      const QColor& color,
                                      float maxValue)
{
    if (data.isEmpty()) {
        return;
    }
    
    painter.setPen(QPen(color, m_impl->lineThickness, Qt::SolidLine, Qt::RoundCap));
    
    QPainterPath path;
    
    float xStep = static_cast<float>(width) / (MAX_HISTORY - 1);
    float graphHeight = m_impl->graphHeight;
    
    // Start path
    float firstY = yOffset - (data.first() / maxValue) * graphHeight;
    path.moveTo(xOffset, firstY);
    
    // Add points
    for (int i = 1; i < data.size(); ++i) {
        float x = xOffset + i * xStep;
        float y = yOffset - (data.at(i) / maxValue) * graphHeight;
        y = qBound(static_cast<float>(yOffset - graphHeight), y, static_cast<float>(yOffset));
        path.lineTo(x, y);
    }
    
    painter.drawPath(path);
    
    // Draw current value
    QFont valueFont = painter.font();
    valueFont.setPointSize(12);
    valueFont.setBold(true);
    painter.setFont(valueFont);
    painter.setPen(color);
    
    QString valueText = QString::number(data.last(), 'f', data == m_aiLatencyHistory ? 1 : 0);
    painter.drawText(xOffset + width - 40, yOffset + 12, valueText);
}

void MetricsDisplay::drawNumbers(QPainter& painter)
{
    QFont font = painter.font();
    font.setPointSize(14);
    font.setBold(true);
    painter.setFont(font);
    
    const int lineHeight = 25;
    int y = 20;
    
    // FPS
    painter.setPen(m_fpsColor);
    painter.drawText(10, y, QString("FPS: %1").arg(m_currentFps));
    y += lineHeight;
    
    // AI Latency
    painter.setPen(m_aiColor);
    painter.drawText(10, y, QString("AI: %1 ms").arg(m_currentAiLatency, 0, 'f', 1));
    y += lineHeight;
    
    // Memory
    painter.setPen(m_memoryColor);
    painter.drawText(10, y, QString("RAM: %1 MB").arg(m_currentMemory));
}

void MetricsDisplay::drawMinimal(QPainter& painter)
{
    QFont font = painter.font();
    font.setPointSize(11);
    font.setBold(true);
    painter.setFont(font);
    
    // Compact single-line display
    QString text = QString("FPS: %1 | AI: %2ms | RAM: %3MB")
        .arg(m_currentFps)
        .arg(m_currentAiLatency, 0, 'f', 1)
        .arg(m_currentMemory);
    
    painter.setPen(Qt::white);
    painter.drawText(rect().adjusted(10, 0, -10, 0), Qt::AlignVCenter, text);
}

} // namespace Nexus::UI
