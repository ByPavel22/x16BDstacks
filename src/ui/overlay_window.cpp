/**
 * Nexus AI Emulator - Overlay Window Implementation
 */

#include "overlay_window.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QMouseEvent>
#include <QWindow>
#include <QtWidgets>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

namespace Nexus::UI {

/**
 * @brief Private implementation for OverlayWindow
 */
struct OverlayWindow::Impl
{
    // UI Components
    QWidget* mainWidget = nullptr;
    QWidget* metricsPanel = nullptr;
    QWidget* aiBoostButton = nullptr;
    
    QLabel* fpsLabel = nullptr;
    QLabel* aiLatencyLabel = nullptr;
    QLabel* memoryLabel = nullptr;
    QLabel* resolutionLabel = nullptr;
    
    // Graphics effects
    QGraphicsBlurEffect* blurEffect = nullptr;
    QGraphicsDropShadowEffect* shadowEffect = nullptr;
    
    // Styling
    QColor backgroundColor = QColor(20, 20, 30, 180);
    QColor accentColor = QColor(0, 150, 255);
    QColor textColor = QColor(255, 255, 255);
    
    // Fonts
    QFont metricsFont;
    QFont boostFont;
};

OverlayWindow::OverlayWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_impl(std::make_unique<Impl>())
{
    // Set window flags for overlay behavior
    setWindowFlags(Qt::FramelessWindowHint | 
                   Qt::WindowStaysOnTopHint | 
                   Qt::Tool |
                   Qt::WindowTransparentForInput);
    
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    
    // Default size
    resize(400, 100);
    
    // Move to top center
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->geometry();
        move(geo.center().x() - width() / 2, 10);
    }
    
    // Edge detection timer
    m_edgeCheckTimer = new QTimer(this);
    connect(m_edgeCheckTimer, &QTimer::timeout, this, &OverlayWindow::checkEdgeTrigger);
}

OverlayWindow::~OverlayWindow()
{
}

bool OverlayWindow::initialize()
{
    qDebug() << "[OverlayWindow] Initializing...";
    
    setupUI();
    setupGlassmorphism();
    setupMetricsPanel();
    setupAiBoostButton();
    setupEdgeDetection();
    
    // Setup animations
    m_opacityAnimation.reset(new QPropertyAnimation(this, "overlayOpacity"));
    m_opacityAnimation->setDuration(200);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    m_initialized = true;
    
    qDebug() << "[OverlayWindow] Initialization complete";
    return true;
}

void OverlayWindow::setOverlayOpacity(float opacity)
{
    m_overlayOpacity = qBound(0.0f, opacity, 1.0f);
    update();
}

void OverlayWindow::updateFps(int fps)
{
    m_currentFps = fps;
    if (m_impl->fpsLabel && isVisible()) {
        m_impl->fpsLabel->setText(QString("FPS: %1").arg(fps));
    }
}

void OverlayWindow::updateAiLatency(float latencyMs)
{
    m_aiLatencyMs = latencyMs;
    if (m_impl->aiLatencyLabel && isVisible()) {
        m_impl->aiLatencyLabel->setText(QString("AI: %1 ms").arg(latencyMs, 0, 'f', 1));
    }
}

void OverlayWindow::updateMemoryUsage(int usageMb)
{
    m_memoryUsageMb = usageMb;
    if (m_impl->memoryLabel && isVisible()) {
        m_impl->memoryLabel->setText(QString("RAM: %1 MB").arg(usageMb));
    }
}

void OverlayWindow::updateAiBoostState(bool active)
{
    m_aiBoostActive = active;
    
    if (m_impl->aiBoostButton) {
        m_impl->aiBoostButton->setProperty("active", active);
        m_impl->aiBoostButton->style()->unpolish(m_impl->aiBoostButton);
        m_impl->aiBoostButton->style()->polish(m_impl->aiBoostButton);
    }
}

void OverlayWindow::setZeroUiMode(bool enabled)
{
    m_zeroUiMode = enabled;
    
    if (enabled) {
        hideOverlay();
        m_edgeCheckTimer->start(100);  // Check for edge trigger
    } else {
        m_edgeCheckTimer->stop();
        showOverlay();
    }
}

bool OverlayWindow::isOverlayVisible() const
{
    return m_overlayVisible && isVisible();
}

void OverlayWindow::showOverlay()
{
    if (m_overlayVisible) {
        return;
    }
    
    setWindowAttribute(Qt::WA_TransparentForMouseEvents, false);
    show();
    raise();
    animateIn();
    
    m_overlayVisible = true;
    emit overlayShown();
}

void OverlayWindow::hideOverlay()
{
    if (!m_overlayVisible) {
        return;
    }
    
    animateOut();
    
    m_overlayVisible = false;
}

void OverlayWindow::toggleOverlay()
{
    if (m_overlayVisible) {
        hideOverlay();
    } else {
        showOverlay();
    }
}

void OverlayWindow::checkEdgeTrigger()
{
    if (!m_zeroUiMode || m_overlayVisible) {
        return;
    }
    
    QPoint cursorPos = QCursor::pos();
    QScreen* screen = QApplication::screenAt(cursorPos);
    
    if (!screen) {
        return;
    }
    
    QRect screenGeo = screen->geometry();
    
    // Check if cursor is at top edge
    if (cursorPos.y() - screenGeo.top() <= m_edgeThresholdPx) {
        showOverlay();
    }
}

void OverlayWindow::animateIn()
{
    // Fade in
    m_opacityAnimation->setStartValue(0.0f);
    m_opacityAnimation->setEndValue(m_overlayOpacity);
    m_opacityAnimation->start();
    
    // Slide down
    QPropertyAnimation* slideAnim = new QPropertyAnimation(this, "pos");
    slideAnim->setDuration(200);
    slideAnim->setStartValue(pos() - QPoint(0, -20));
    slideAnim->setEndValue(pos());
    slideAnim->setEasingCurve(QEasingCurve::OutCubic);
    slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void OverlayWindow::animateOut()
{
    // Fade out
    m_opacityAnimation->setStartValue(m_overlayOpacity);
    m_opacityAnimation->setEndValue(0.0f);
    m_opacityAnimation->start();
    
    // Slide up
    QPropertyAnimation* slideAnim = new QPropertyAnimation(this, "pos");
    slideAnim->setDuration(200);
    slideAnim->setStartValue(pos());
    slideAnim->setEndValue(pos() - QPoint(0, -20));
    slideAnim->setEasingCurve(QEasingCurve::InCubic);
    slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
    
    // Hide after animation
    QTimer::singleShot(200, this, [this]() {
        hide();
        setWindowAttribute(Qt::WA_TransparentForMouseEvents, true);
        emit overlayHidden();
    });
}

void OverlayWindow::updateMetricsDisplay()
{
    // Update all metrics labels
    updateFps(m_currentFps);
    updateAiLatency(m_aiLatencyMs);
    updateMemoryUsage(m_memoryUsageMb);
}

void OverlayWindow::setupUI()
{
    m_impl->mainWidget = new QWidget(this);
    setCentralWidget(m_impl->mainWidget);
    
    auto* layout = new QHBoxLayout(m_impl->mainWidget);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setSpacing(30);
    
    // Metrics panel
    setupMetricsPanel();
    layout->addWidget(m_impl->metricsPanel);
    
    // AI Boost button
    setupAiBoostButton();
    layout->addWidget(m_impl->aiBoostButton);
    
    // Spacer
    layout->addStretch();
}

void OverlayWindow::setupGlassmorphism()
{
    // Apply blur effect (Windows DWM or QGraphicsBlurEffect)
#ifdef Q_OS_WIN
    // Use DWM for native blur on Windows
    HWND hwnd = reinterpret_cast<HWND>(winId());
    
    MARGINS margins = {0, 0, 0, 100};  // Blur behind bottom 100px
    DwmExtendFrameIntoClientArea(hwnd, &margins);
#endif
    
    // Fallback QGraphicsBlurEffect
    m_impl->blurEffect = new QGraphicsBlurEffect(this);
    m_impl->blurEffect->setBlurRadius(20);
    m_impl->mainWidget->setGraphicsEffect(m_impl->blurEffect);
    
    // Shadow effect
    m_impl->shadowEffect = new QGraphicsDropShadowEffect(this);
    m_impl->shadowEffect->setBlurRadius(30);
    m_impl->shadowEffect->setColor(QColor(0, 0, 0, 100));
    m_impl->shadowEffect->setOffset(0, 5);
}

void OverlayWindow::setupMetricsPanel()
{
    m_impl->metricsPanel = new QWidget(m_impl->mainWidget);
    auto* layout = new QHBoxLayout(m_impl->metricsPanel);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(20);
    
    // FPS label
    m_impl->fpsLabel = new QLabel("FPS: 0", m_impl->metricsPanel);
    m_impl->fpsLabel->setStyleSheet(R"(
        QLabel {
            color: #FFFFFF;
            font-size: 16px;
            font-weight: bold;
            font-family: 'Segoe UI', sans-serif;
        }
    )");
    layout->addWidget(m_impl->fpsLabel);
    
    // AI Latency label
    m_impl->aiLatencyLabel = new QLabel("AI: 0.0 ms", m_impl->metricsPanel);
    m_impl->aiLatencyLabel->setStyleSheet(R"(
        QLabel {
            color: #00D4AA;
            font-size: 16px;
            font-weight: bold;
            font-family: 'Segoe UI', sans-serif;
        }
    )");
    layout->addWidget(m_impl->aiLatencyLabel);
    
    // Memory label
    m_impl->memoryLabel = new QLabel("RAM: 0 MB", m_impl->metricsPanel);
    m_impl->memoryLabel->setStyleSheet(R"(
        QLabel {
            color: #FFB800;
            font-size: 16px;
            font-weight: bold;
            font-family: 'Segoe UI', sans-serif;
        }
    )");
    layout->addWidget(m_impl->memoryLabel);
    
    // Resolution label
    m_impl->resolutionLabel = new QLabel("1920x1080", m_impl->metricsPanel);
    m_impl->resolutionLabel->setStyleSheet(R"(
        QLabel {
            color: #888888;
            font-size: 14px;
            font-family: 'Segoe UI', sans-serif;
        }
    )");
    layout->addWidget(m_impl->resolutionLabel);
    
    // Panel styling
    m_impl->metricsPanel->setStyleSheet(R"(
        QWidget {
            background-color: rgba(30, 30, 40, 180);
            border-radius: 12px;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
    )");
}

void OverlayWindow::setupAiBoostButton()
{
    m_impl->aiBoostButton = new QPushButton("AI BOOST", m_impl->mainWidget);
    m_impl->aiBoostButton->setFixedSize(120, 40);
    m_impl->aiBoostButton->setCheckable(true);
    
    m_impl->aiBoostButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(0, 150, 255, 0.3);
            color: #00D4AA;
            border: 2px solid #00D4AA;
            border-radius: 20px;
            font-size: 14px;
            font-weight: bold;
            font-family: 'Segoe UI', sans-serif;
            padding: 8px 20px;
        }
        QPushButton:hover {
            background-color: rgba(0, 150, 255, 0.5);
            border-color: #00FFFF;
            color: #00FFFF;
        }
        QPushButton:checked {
            background-color: rgba(0, 212, 170, 0.4);
            color: #FFFFFF;
            border-color: #00D4AA;
            box-shadow: 0 0 20px rgba(0, 212, 170, 0.5);
        }
    )");
    
    connect(m_impl->aiBoostButton, &QPushButton::clicked, this, [this]() {
        emit aiBoostToggled();
    });
}

void OverlayWindow::setupEdgeDetection()
{
    // Edge detection is handled in checkEdgeTrigger()
    m_edgeThresholdPx = 10;
}

} // namespace Nexus::UI
