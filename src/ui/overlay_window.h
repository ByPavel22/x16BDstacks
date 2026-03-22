/**
 * Nexus AI Emulator - UI Overlay Window
 * 
 * Glassmorphism-style overlay with:
 * - Edge-triggered appearance
 * - Minimalist metrics display
 * - AI Boost toggle
 * - Zero UI mode support
 */

#ifndef NEXUS_OVERLAY_WINDOW_H
#define NEXUS_OVERLAY_WINDOW_H

#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#include <QMainWindow>
#include <QScopedPointer>
#include <QPropertyAnimation>
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QTimer>
#endif
#endif
#include <memory>

namespace Nexus::UI {

/**
 * @brief Glassmorphism overlay window
 * 
 * Semi-transparent overlay that appears on edge trigger
 * or when user interaction is detected.
 */
class OverlayWindow : public QMainWindow
{
    Q_OBJECT
    
    Q_PROPERTY(float overlayOpacity READ overlayOpacity WRITE setOverlayOpacity)
    
public:
    explicit OverlayWindow(QWidget *parent = nullptr);
    ~OverlayWindow();
    
    /**
     * @brief Initialize overlay
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Get current overlay opacity (0.0-1.0)
     */
    float overlayOpacity() const { return m_overlayOpacity; }
    
    /**
     * @brief Set overlay opacity
     */
    void setOverlayOpacity(float opacity);
    
    /**
     * @brief Update FPS display
     */
    void updateFps(int fps);
    
    /**
     * @brief Update AI latency display
     */
    void updateAiLatency(float latencyMs);
    
    /**
     * @brief Update memory usage display
     */
    void updateMemoryUsage(int usageMb);
    
    /**
     * @brief Update AI Boost indicator
     */
    void updateAiBoostState(bool active);
    
    /**
     * @brief Set Zero UI mode
     */
    void setZeroUiMode(bool enabled);
    
    /**
     * @brief Check if overlay is visible
     */
    bool isOverlayVisible() const;

signals:
    void aiBoostToggled();
    void overlayHidden();
    void overlayShown();
    
public slots:
    void showOverlay();
    void hideOverlay();
    void toggleOverlay();
    
private slots:
    void checkEdgeTrigger();
    void animateIn();
    void animateOut();
    void updateMetricsDisplay();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    void setupUI();
    void setupGlassmorphism();
    void setupMetricsPanel();
    void setupAiBoostButton();
    void setupEdgeDetection();
    
    float m_overlayOpacity = 0.85f;
    bool m_zeroUiMode = true;
    bool m_overlayVisible = false;
    
    // Animations
    QScopedPointer<QPropertyAnimation> m_opacityAnimation;
    QScopedPointer<QPropertyAnimation> m_slideAnimation;
    
    // Edge detection
    QTimer* m_edgeCheckTimer;
    int m_edgeThresholdPx = 10;
    
    // Metrics state
    int m_currentFps = 0;
    float m_aiLatencyMs = 0.0f;
    int m_memoryUsageMb = 0;
    
    // AI Boost state
    bool m_aiBoostActive = false;
};

} // namespace Nexus::UI

#endif // NEXUS_OVERLAY_WINDOW_H
