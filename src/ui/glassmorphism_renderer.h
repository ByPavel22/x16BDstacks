/**
 * Nexus AI Emulator - Glassmorphism Renderer
 * 
 * Handles the visual rendering of glassmorphism effects:
 * - Background blur
 * - Semi-transparent layers
 * - Frosted glass effect
 */

#ifndef NEXUS_GLASSMORPHISM_RENDERER_H
#define NEXUS_GLASSMORPHISM_RENDERER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <memory>

namespace Nexus::UI {

/**
 * @brief Glassmorphism visual effects renderer
 */
class GlassmorphismRenderer : public QObject
{
    Q_OBJECT
    
public:
    explicit GlassmorphismRenderer(QObject *parent = nullptr);
    ~GlassmorphismRenderer();
    
    /**
     * @brief Initialize renderer
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Render glassmorphism effect on target area
     * @param target Rect to apply effect to
     * @param blurRadius Blur radius in pixels
     * @param opacity Opacity of background (0.0-1.0)
     */
    void renderGlassEffect(const QRect& target, 
                           int blurRadius = 20, 
                           float opacity = 0.85f);
    
    /**
     * @brief Apply frosted glass effect to image
     * @param input Input image
     * @param blurRadius Blur radius
     * @return Processed image with frosted effect
     */
    QImage applyFrostedEffect(const QImage& input, int blurRadius = 20);
    
    /**
     * @brief Set accent color for highlights
     */
    void setAccentColor(const QColor& color);
    
    /**
     * @brief Set background color
     */
    void setBackgroundColor(const QColor& color);

signals:
    void renderComplete();
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    
    // Colors
    QColor m_accentColor = QColor(0, 150, 255);
    QColor m_backgroundColor = QColor(20, 20, 30, 180);
    
    // Rendering helpers
    void applyBlur(QImage& image, int radius);
    void applyNoise(QImage& image, float intensity = 0.05f);
    void applyGradient(QImage& image);
};

} // namespace Nexus::UI

#endif // NEXUS_GLASSMORPHISM_RENDERER_H
