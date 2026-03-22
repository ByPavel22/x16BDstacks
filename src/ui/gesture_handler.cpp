#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#endif
/**
 * Nexus AI Emulator - Gesture Handler Implementation
 */

#include "gesture_handler.h"
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QDebug>
#endif
#include <QMouseEvent>
#include <QTouchEvent>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QSwipeGesture>
#include <QApplication>

namespace Nexus::UI {

/**
 * @brief Private implementation for GestureHandler
 */
struct GestureHandler::Impl
{
    // Touch points tracking
    QMap<int, QPoint> activeTouchPoints;
    
    // Gesture history
    QVector<QPoint> movementHistory;
    static constexpr int MAX_HISTORY = 10;
    
    // Timing
    QTimer* tapTimer = nullptr;
    QPoint lastTapPos;
    quint64 lastTapTime = 0;
    
    // Edge detection
    bool edgeDetected = false;
    int edgeThreshold = 20;  // pixels from edge
};

GestureHandler::GestureHandler(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

GestureHandler::~GestureHandler()
{
}

bool GestureHandler::initialize()
{
    qDebug() << "[GestureHandler] Initializing...";
    
    // Install event filter on application
    qApp->installEventFilter(this);
    
    // Setup tap timer for double-tap detection
    m_impl->tapTimer = new QTimer(this);
    m_impl->tapTimer->setSingleShot(true);
    m_impl->tapTimer->setInterval(300);  // Double-tap window
    
    connect(m_impl->tapTimer, &QTimer::timeout, this, [this]() {
        m_impl->lastTapTime = 0;
    });
    
    m_initialized = true;
    qDebug() << "[GestureHandler] Initialization complete";
    
    return true;
}

bool GestureHandler::processEvent(QEvent* event)
{
    if (!m_initialized) {
        return false;
    }
    
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            return handleMousePress(static_cast<QMouseEvent*>(event));
            
        case QEvent::MouseMove:
            return handleMouseMove(static_cast<QMouseEvent*>(event));
            
        case QEvent::MouseButtonRelease:
            return handleMouseRelease(static_cast<QMouseEvent*>(event));
            
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            return handleTouchEvent(static_cast<QTouchEvent*>(event));
            
        case QEvent::Gesture:
            return handleGestureEvent(static_cast<QGestureEvent*>(event));
            
        default:
            return false;
    }
}

void GestureHandler::setSensitivity(float sensitivity)
{
    m_sensitivity = qBound(0.0f, sensitivity, 1.0f);
    m_minSwipeDistance = static_cast<int>(100 * (1.0f - m_sensitivity));
}

void GestureHandler::setEdgeSwipeEnabled(bool enabled)
{
    m_edgeSwipeEnabled = enabled;
}

void GestureHandler::setEdgeSwipeDelay(int delayMs)
{
    m_edgeSwipeDelayMs = delayMs;
}

bool GestureHandler::handleMousePress(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return false;
    }
    
    m_touching = true;
    m_startPos = event->pos();
    m_currentPos = event->pos();
    m_touchStartTime = QDateTime::currentMSecsSinceEpoch();
    
    emit touchStarted(m_startPos);
    
    // Check for edge swipe start
    if (m_edgeSwipeEnabled && isEdgeSwipe()) {
        m_impl->edgeDetected = true;
    }
    
    return true;
}

bool GestureHandler::handleMouseMove(QMouseEvent* event)
{
    if (!m_touching) {
        return false;
    }
    
    m_currentPos = event->pos();
    
    // Add to movement history
    m_impl->movementHistory.append(m_currentPos);
    if (m_impl->movementHistory.size() > Impl::MAX_HISTORY) {
        m_impl->movementHistory.removeFirst();
    }
    
    emit touchMoved(m_currentPos);
    
    return true;
}

bool GestureHandler::handleMouseRelease(QMouseEvent* event)
{
    if (!m_touching || event->button() != Qt::LeftButton) {
        return false;
    }
    
    m_touching = false;
    
    emit touchEnded(m_currentPos);
    
    // Recognize gesture
    GestureType gesture = recognizeGesture();
    
    if (gesture != GestureType::None) {
        m_lastGesture = gesture;
        emit gestureRecognized(gesture);
        
        if (gesture == GestureType::EdgeSwipe) {
            emit edgeSwipeDetected();
        }
    }
    
    // Check for tap/double-tap
    if (swipeDistance() < 5) {  // Very small movement = tap
        quint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        
        if (currentTime - m_impl->lastTapTime < 300) {
            // Double tap
            m_lastGesture = GestureType::DoubleTap;
            emit gestureRecognized(GestureType::DoubleTap);
            m_impl->tapTimer->stop();
        } else {
            // Single tap
            m_impl->lastTapTime = currentTime;
            m_impl->lastTapPos = m_currentPos;
            m_impl->tapTimer->start();
        }
    }
    
    // Reset state
    m_impl->movementHistory.clear();
    m_impl->edgeDetected = false;
    
    return true;
}

bool GestureHandler::handleTouchEvent(QTouchEvent* event)
{
    // Store active touch points
    const QList<QTouchEvent::TouchPoint>& touchPoints = event->touchPoints();
    
    for (const QTouchEvent::TouchPoint& point : touchPoints) {
        int id = point.id();
        
        switch (point.state()) {
            case Qt::TouchPointPressed:
                m_impl->activeTouchPoints.insert(id, point.pos().toPoint());
                if (m_impl->activeTouchPoints.size() == 1) {
                    m_startPos = point.pos().toPoint();
                    m_touchStartTime = QDateTime::currentMSecsSinceEpoch();
                    emit touchStarted(m_startPos);
                }
                break;
                
            case Qt::TouchPointMoved:
                m_impl->activeTouchPoints.insert(id, point.pos().toPoint());
                emit touchMoved(point.pos().toPoint());
                break;
                
            case Qt::TouchPointReleased:
                m_impl->activeTouchPoints.remove(id);
                if (m_impl->activeTouchPoints.isEmpty()) {
                    m_touching = false;
                    emit touchEnded(point.pos().toPoint());
                    
                    // Recognize gesture
                    GestureType gesture = recognizeGesture();
                    if (gesture != GestureType::None) {
                        m_lastGesture = gesture;
                        emit gestureRecognized(gesture);
                    }
                }
                break;
                
            default:
                break;
        }
    }
    
    return true;
}

bool GestureHandler::handleGestureEvent(QGestureEvent* event)
{
    // Handle Qt gesture events
    if (QGesture* gesture = event->gesture(Qt::SwipeGesture)) {
        QSwipeGesture* swipe = static_cast<QSwipeGesture*>(gesture);
        
        switch (swipe->state()) {
            case Qt::GestureFinished:
                if (swipe->horizontalDirection() == QSwipeGesture::Left) {
                    emit gestureRecognized(GestureType::SwipeLeft);
                } else if (swipe->horizontalDirection() == QSwipeGesture::Right) {
                    emit gestureRecognized(GestureType::SwipeRight);
                } else if (swipe->verticalDirection() == QSwipeGesture::Up) {
                    emit gestureRecognized(GestureType::SwipeUp);
                } else if (swipe->verticalDirection() == QSwipeGesture::Down) {
                    emit gestureRecognized(GestureType::SwipeDown);
                }
                break;
                
            default:
                break;
        }
    }
    
    if (QGesture* gesture = event->gesture(Qt::PinchGesture)) {
        QPinchGesture* pinch = static_cast<QPinchGesture*>(gesture);
        
        switch (pinch->state()) {
            case Qt::GestureFinished:
                if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
                    if (pinch->scaleFactor() > 1.0) {
                        emit gestureRecognized(GestureType::Pinch);
                    }
                }
                break;
                
            default:
                break;
        }
    }
    
    return event->wasAccepted();
}

GestureType GestureHandler::recognizeGesture()
{
    // Check for edge swipe first
    if (m_edgeSwipeEnabled && m_impl->edgeDetected) {
        return GestureType::EdgeSwipe;
    }
    
    float distance = swipeDistance();
    
    // Too small to be a swipe
    if (distance < m_minSwipeDistance) {
        return GestureType::None;
    }
    
    float angle = swipeAngle();
    
    // Determine direction based on angle
    if (angle >= -45 && angle <= 45) {
        return GestureType::SwipeRight;
    } else if (angle >= 45 && angle <= 135) {
        return GestureType::SwipeDown;
    } else if (angle >= 135 || angle <= -135) {
        return GestureType::SwipeLeft;
    } else if (angle >= -135 && angle <= -45) {
        return GestureType::SwipeUp;
    }
    
    return GestureType::None;
}

bool GestureHandler::isEdgeSwipe() const
{
    // Check if starting position is near screen edge
    int edgeThreshold = m_impl->edgeThreshold;
    
    return (m_startPos.x() < edgeThreshold ||
            m_startPos.x() > QApplication::primaryScreen()->size().width() - edgeThreshold ||
            m_startPos.y() < edgeThreshold ||
            m_startPos.y() > QApplication::primaryScreen()->size().height() - edgeThreshold);
}

float GestureHandler::swipeDistance() const
{
    QPointF delta = m_currentPos - m_startPos;
    return std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
}

float GestureHandler::swipeAngle() const
{
    QPointF delta = m_currentPos - m_startPos;
    
    if (delta.isNull()) {
        return 0.0f;
    }
    
    return qRadiansToDegrees(std::atan2(delta.y(), delta.x()));
}

} // namespace Nexus::UI
