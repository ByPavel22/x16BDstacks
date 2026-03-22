/**
 * Nexus AI Emulator - Gesture Handler
 * 
 * Handles touch and mouse gestures for Zero UI mode:
 * - Edge swipes
 * - Multi-finger gestures
 * - Custom gesture recognition
 */

#ifndef NEXUS_GESTURE_HANDLER_H
#define NEXUS_GESTURE_HANDLER_H

#include <QObject>
#include <QPoint>
#include <QEvent>
#include <QTimer>
#include <memory>

namespace Nexus::UI {

/**
 * @brief Gesture types
 */
enum class GestureType {
    None,
    SwipeUp,
    SwipeDown,
    SwipeLeft,
    SwipeRight,
    EdgeSwipe,
    TwoFingerSwipe,
    Pinch,
    Tap,
    DoubleTap
};

/**
 * @brief Gesture recognition and handling
 */
class GestureHandler : public QObject
{
    Q_OBJECT
    
public:
    explicit GestureHandler(QObject *parent = nullptr);
    ~GestureHandler();
    
    /**
     * @brief Initialize gesture handler
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Process mouse/touch event
     * @param event Input event
     * @return true if event was handled
     */
    bool processEvent(QEvent* event);
    
    /**
     * @brief Set gesture sensitivity (0.0-1.0)
     */
    void setSensitivity(float sensitivity);
    
    /**
     * @brief Enable/disable edge swipe detection
     */
    void setEdgeSwipeEnabled(bool enabled);
    
    /**
     * @brief Set edge swipe delay in milliseconds
     */
    void setEdgeSwipeDelay(int delayMs);
    
    /**
     * @brief Get last recognized gesture
     */
    GestureType lastGesture() const { return m_lastGesture; }

signals:
    void gestureRecognized(GestureType gesture);
    void edgeSwipeDetected();
    void touchStarted(QPoint pos);
    void touchMoved(QPoint pos);
    void touchEnded(QPoint pos);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    GestureType m_lastGesture = GestureType::None;
    
    // Gesture state
    QPoint m_startPos;
    QPoint m_currentPos;
    bool m_touching = false;
    quint64 m_touchStartTime = 0;
    
    // Settings
    float m_sensitivity = 0.8f;
    bool m_edgeSwipeEnabled = true;
    int m_edgeSwipeDelayMs = 100;
    int m_minSwipeDistance = 50;  // pixels
    
    // Recognition
    GestureType recognizeGesture();
    bool isEdgeSwipe() const;
    float swipeDistance() const;
    float swipeAngle() const;
};

} // namespace Nexus::UI

#endif // NEXUS_GESTURE_HANDLER_H
