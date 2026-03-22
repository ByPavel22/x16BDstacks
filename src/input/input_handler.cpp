/**
 * Nexus AI Emulator - Input Handler Implementation
 */

#include "input_handler.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Nexus::Input {

/**
 * @brief Private implementation for InputHandler
 */
struct InputHandler::Impl
{
    // Default key mappings
    QMap<QString, QKeySequence> defaultMappings = {
        {"overlay_toggle", QKeySequence("Ctrl+Shift+O")},
        {"ai_boost", QKeySequence("F12")},
        {"fullscreen", QKeySequence("Alt+Return")},
        {"screenshot", QKeySequence("F11")},
        {"pause", QKeySequence("Pause")},
        {"volume_up", QKeySequence("F3")},
        {"volume_down", QKeySequence("F2")},
        {"home", QKeySequence("F4")}
    };
    
    // Gesture to action mapping
    QMap<UI::GestureType, QString> gestureMappings = {
        {UI::GestureType::EdgeSwipe, "overlay_toggle"},
        {UI::GestureType::SwipeUp, "volume_up"},
        {UI::GestureType::SwipeDown, "volume_down"},
        {UI::GestureType::TwoFingerSwipe, "ai_boost"}
    };
};

InputHandler::InputHandler(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

InputHandler::~InputHandler()
{
    shutdown();
}

bool InputHandler::initialize()
{
    qDebug() << "[InputHandler] Initializing...";
    
    // Setup default key mappings
    setupDefaultKeyMappings();
    
    // Initialize gesture handler
    m_gestureHandler = std::make_unique<UI::GestureHandler>();
    if (!m_gestureHandler->initialize()) {
        qWarning() << "[InputHandler] Gesture handler initialization failed";
    } else {
        connect(m_gestureHandler.get(), &UI::GestureHandler::gestureRecognized,
                this, &InputHandler::onGestureRecognized);
        connect(m_gestureHandler.get(), &UI::GestureHandler::edgeSwipeDetected,
                this, &InputHandler::overlayTriggered);
    }
    
    // Install event filter
    qApp->installEventFilter(this);
    
    m_initialized = true;
    qDebug() << "[InputHandler] Initialization complete";
    
    return true;
}

void InputHandler::shutdown()
{
    if (!m_initialized) {
        return;
    }
    
    qDebug() << "[InputHandler] Shutting down...";
    
    qApp->removeEventFilter(this);
    m_gestureHandler.reset();
    
    m_initialized = false;
}

void InputHandler::setKeyMapping(const QString& action, const QKeySequence& key)
{
    m_keyMappings[action] = key;
    emit keyMappingChanged();
    
    qDebug() << "[InputHandler] Key mapping:" << action << "->" << key.toString();
}

QKeySequence InputHandler::getKeyMapping(const QString& action) const
{
    return m_keyMappings.value(action, QKeySequence());
}

bool InputHandler::loadKeymapProfile(const QString& profilePath)
{
    QFile file(profilePath);
    
    if (!file.open(QFile::ReadOnly)) {
        qCritical() << "[InputHandler] Failed to open keymap profile:" << profilePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();
    
    QJsonObject mappings = root["mappings"].toObject();
    
    for (auto it = mappings.begin(); it != mappings.end(); ++it) {
        QString action = it.key();
        QString keyString = it.value().toString();
        QKeySequence key(keyString);
        
        if (!key.isEmpty()) {
            m_keyMappings[action] = key;
        }
    }
    
    emit keyMappingChanged();
    
    qDebug() << "[InputHandler] Keymap profile loaded:" << profilePath;
    return true;
}

bool InputHandler::saveKeymapProfile(const QString& profilePath)
{
    QFile file(profilePath);
    
    if (!file.open(QFile::WriteOnly)) {
        qCritical() << "[InputHandler] Failed to save keymap profile:" << profilePath;
        return false;
    }
    
    QJsonObject root;
    QJsonObject mappings;
    
    for (auto it = m_keyMappings.begin(); it != m_keyMappings.end(); ++it) {
        mappings[it.key()] = it.value().toString();
    }
    
    root["mappings"] = mappings;
    
    file.write(QJsonDocument(root).toJson());
    
    qDebug() << "[InputHandler] Keymap profile saved:" << profilePath;
    return true;
}

void InputHandler::setGesturesEnabled(bool enabled)
{
    m_gesturesEnabled = enabled;
    
    if (m_gestureHandler) {
        m_gestureHandler->setEdgeSwipeEnabled(enabled);
    }
    
    qDebug() << "[InputHandler] Gestures:" << (enabled ? "enabled" : "disabled");
}

bool InputHandler::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    
    if (!m_initialized) {
        return false;
    }
    
    switch (event->type()) {
        case QEvent::KeyPress:
            processKeyEvent(static_cast<QKeyEvent*>(event));
            return false;  // Don't block
            
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            processMouseEvent(static_cast<QMouseEvent*>(event));
            return false;
            
        case QEvent::Gesture:
            if (m_gesturesEnabled && m_gestureHandler) {
                return m_gestureHandler->processEvent(event);
            }
            return false;
            
        default:
            return false;
    }
}

void InputHandler::setupDefaultKeyMappings()
{
    m_keyMappings = m_impl->defaultMappings;
    
    qDebug() << "[InputHandler] Default key mappings setup";
}

void InputHandler::processKeyEvent(QKeyEvent* event)
{
    if (event->type() != QEvent::KeyPress || event->isAutoRepeat()) {
        return;
    }
    
    QString keyString = QKeySequence(event->key() | event->modifiers()).toString();
    
    // Find matching action
    for (auto it = m_keyMappings.begin(); it != m_keyMappings.end(); ++it) {
        if (it.value().toString() == keyString) {
            emit actionTriggered(it.key());
            
            if (it.key() == "overlay_toggle") {
                emit overlayTriggered();
            } else if (it.key() == "ai_boost") {
                emit aiBoostTriggered();
            }
            
            break;
        }
    }
}

void InputHandler::processMouseEvent(QMouseEvent* event)
{
    if (m_gesturesEnabled && m_gestureHandler) {
        m_gestureHandler->processEvent(event);
    }
}

void InputHandler::onGestureRecognized(UI::GestureType gesture)
{
    qDebug() << "[InputHandler] Gesture recognized:" << static_cast<int>(gesture);
    
    // Map gesture to action
    QString action = m_impl->gestureMappings.value(gesture);
    
    if (!action.isEmpty()) {
        emit actionTriggered(action);
        
        if (action == "overlay_toggle") {
            emit overlayTriggered();
        } else if (action == "ai_boost") {
            emit aiBoostTriggered();
        }
    }
}

} // namespace Nexus::Input
