#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#endif
/**
 * Nexus AI Emulator - Glassmorphism Renderer Implementation
 */

#include "glassmorphism_renderer.h"
#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#else
#include <QDebug>
#endif
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QRandomGenerator>

namespace Nexus::UI {

/**
 * @brief Private implementation for GlassmorphismRenderer
 */
struct GlassmorphismRenderer::Impl
{
    QGraphicsBlurEffect* blurEffect = nullptr;
    
    // Render cache
    QImage cachedBackground;
    bool cacheValid = false;
    
    // Settings
    int defaultBlurRadius = 20;
    float defaultOpacity = 0.85f;
    float noiseIntensity = 0.03f;
};

GlassmorphismRenderer::GlassmorphismRenderer(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

GlassmorphismRenderer::~GlassmorphismRenderer()
{
}

bool GlassmorphismRenderer::initialize()
{
    qDebug() << "[GlassmorphismRenderer] Initializing...";
    
    m_impl->blurEffect = new QGraphicsBlurEffect();
    m_impl->blurEffect->setBlurRadius(m_impl->defaultBlurRadius);
    
    m_initialized = true;
    qDebug() << "[GlassmorphismRenderer] Initialization complete";
    
    return true;
}

void GlassmorphismRenderer::renderGlassEffect(const QRect& target, 
                                               int blurRadius, 
                                               float opacity)
{
    if (!m_initialized) {
        return;
    }
    
    // This would be called during paint events
    // Actual rendering happens in the widget's paintEvent
    
    emit renderComplete();
}

QImage GlassmorphismRenderer::applyFrostedEffect(const QImage& input, int blurRadius)
{
    if (input.isNull()) {
        return QImage();
    }
    
    QImage result = input;
    
    // Step 1: Apply blur
    applyBlur(result, blurRadius);
    
    // Step 2: Add subtle noise for texture
    applyNoise(result, m_impl->noiseIntensity);
    
    // Step 3: Apply gradient overlay
    applyGradient(result);
    
    return result;
}

void GlassmorphismRenderer::setAccentColor(const QColor& color)
{
    m_accentColor = color;
    m_impl->cacheValid = false;
}

void GlassmorphismRenderer::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
    m_impl->cacheValid = false;
}

void GlassmorphismRenderer::applyBlur(QImage& image, int radius)
{
    // Use QGraphicsBlurEffect for high-quality blur
    QGraphicsBlurEffect blurEffect;
    blurEffect.setBlurRadius(radius);
    
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap blurred = pixmap;
    
    // Apply blur via QPainter
    QPainter painter(&blurred);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Simple box blur as fallback (multiple passes for better quality)
    int passes = radius / 4 + 1;
    for (int pass = 0; pass < passes; ++pass) {
        QImage temp = image.copy();
        
        for (int y = 1; y < image.height() - 1; ++y) {
            for (int x = 1; x < image.width() - 1; ++x) {
                int r = 0, g = 0, b = 0, a = 0;
                
                // 3x3 kernel
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        QRgb pixel = image.pixel(x + dx, y + dy);
                        r += qRed(pixel);
                        g += qGreen(pixel);
                        b += qBlue(pixel);
                        a += qAlpha(pixel);
                    }
                }
                
                temp.setPixel(x, y, qRgba(r / 9, g / 9, b / 9, a / 9));
            }
        }
        
        image = temp;
    }
}

void GlassmorphismRenderer::applyNoise(QImage& image, float intensity)
{
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            
            // Add random noise
            int noise = static_cast<int>((QRandomGenerator::global()->bounded(256) - 128) * intensity);
            
            int r = qBound(0, qRed(pixel) + noise, 255);
            int g = qBound(0, qGreen(pixel) + noise, 255);
            int b = qBound(0, qBlue(pixel) + noise, 255);
            
            image.setPixel(x, y, qRgba(r, g, b, qAlpha(pixel)));
        }
    }
}

void GlassmorphismRenderer::applyGradient(QImage& image)
{
    QPainter painter(&image);
    
    // Subtle gradient overlay
    QLinearGradient gradient(0, 0, 0, image.height());
    gradient.setColorAt(0, QColor(255, 255, 255, 10));
    gradient.setColorAt(0.5, QColor(255, 255, 255, 5));
    gradient.setColorAt(1, QColor(0, 0, 0, 10));
    
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.fillRect(image.rect(), gradient);
    
    // Accent glow
    if (m_accentColor.alpha() > 0) {
        QRadialGradient glow(image.width() / 2, image.height() / 2, image.width() / 3);
        glow.setColorAt(0, QColor(m_accentColor.red(), m_accentColor.green(), m_accentColor.blue(), 20));
        glow.setColorAt(1, Qt::transparent);
        
        painter.fillRect(image.rect(), glow);
    }
}

} // namespace Nexus::UI
