/**
 * Nexus AI Emulator - Input Handler
 * 
 * Central input management:
 * - Keyboard mapping
 * - Mouse input
 * - Gamepad support
 * - Gesture integration
 */

#ifndef NEXUS_INPUT_HANDLER_H
#define NEXUS_INPUT_HANDLER_H

#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <memory>

#include "ui/gesture_handler.h"

namespace Nexus::Input {

/**
 * @brief Input device types
 */
enum class InputDevice {
    Keyboard,
    Mouse,
    Touch,
    Gamepad
};

/**
 * @brief Main input handler
 */
class InputHandler : public QObject
{
    Q_OBJECT
    
public:
    explicit InputHandler(QObject *parent = nullptr);
    ~InputHandler();
    
    /**
     * @brief Initialize input handler
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown input handler
     */
    void shutdown();
    
    /**
     * @brief Set key mapping for action
     */
    void setKeyMapping(const QString& action, const QKeySequence& key);
    
    /**
     * @brief Get key mapping for action
     */
    QKeySequence getKeyMapping(const QString& action) const;
    
    /**
     * @brief Load keymap profile
     */
    bool loadKeymapProfile(const QString& profilePath);
    
    /**
     * @brief Save keymap profile
     */
    bool saveKeymapProfile(const QString& profilePath);
    
    /**
     * @brief Enable/disable gesture recognition
     */
    void setGesturesEnabled(bool enabled);
    
    /**
     * @brief Check if gestures are enabled
     */
    bool gesturesEnabled() const { return m_gesturesEnabled; }
    
    /**
     * @brief Get gesture handler
     */
    UI::GestureHandler* gestureHandler() const { return m_gestureHandler.get(); }

signals:
    void actionTriggered(const QString& action);
    void overlayTriggered();
    void aiBoostTriggered();
    void keyMappingChanged();
    
private slots:
    void onGestureRecognized(UI::GestureType gesture);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    bool m_gesturesEnabled = true;
    
    // Input mapping
    QMap<QString, QKeySequence> m_keyMappings;
    
    // Sub-handlers
    std::unique_ptr<UI::GestureHandler> m_gestureHandler;
    
    // Event filter
    bool eventFilter(QObject* obj, QEvent* event) override;
    
    void setupDefaultKeyMappings();
    void processKeyEvent(QKeyEvent* event);
    void processMouseEvent(QMouseEvent* event);
};

} // namespace Nexus::Input

#endif // NEXUS_INPUT_HANDLER_H
