/**
 * Nexus AI Emulator - Voice Controller
 * 
 * Voice command recognition for hands-free control:
 * - Wake word detection
 * - Command recognition
 * - Text-to-speech feedback
 */

#ifndef NEXUS_VOICE_CONTROLLER_H
#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#define NEXUS_VOICE_CONTROLLER_H

#include <QObject>
#endif
#include <QString>
#endif
#include <memory>

namespace Nexus::UI {

/**
 * @brief Voice commands
 */
enum class VoiceCommand {
    None,
    ToggleOverlay,
    ToggleAiBoost,
    ShowFps,
    ShowMetrics,
    HideUi,
    TakeScreenshot,
    RecordVideo,
    Pause,
    Resume,
    Exit
};

/**
 * @brief Voice command controller
 */
class VoiceController : public QObject
{
    Q_OBJECT
    
public:
    explicit VoiceController(QObject *parent = nullptr);
    ~VoiceController();
    
    /**
     * @brief Initialize voice controller
     * @param language Language code (e.g., "en-US")
     * @return true if initialization successful
     */
    bool initialize(const QString& language = "en-US");
    
    /**
     * @brief Start listening for voice commands
     */
    void startListening();
    
    /**
     * @brief Stop listening
     */
    void stopListening();
    
    /**
     * @brief Check if listening is active
     */
    bool isListening() const { return m_listening; }
    
    /**
     * @brief Set wake word
     */
    void setWakeWord(const QString& wakeWord);
    
    /**
     * @brief Enable/disable wake word requirement
     */
    void setWakeWordRequired(bool required);
    
    /**
     * @brief Set language
     */
    void setLanguage(const QString& language);

signals:
    void commandReceived(VoiceCommand command);
    void listeningStarted();
    void listeningStopped();
    void errorOccurred(const QString& error);
    void speechRecognized(const QString& text);
    
private slots:
    void processSpeech(const QString& text);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    bool m_listening = false;
    bool m_wakeWordRequired = true;
    
    QString m_wakeWord = "Nexus";
    QString m_language = "en-US";
    
    VoiceCommand parseCommand(const QString& text);
    void speak(const QString& text);
};

} // namespace Nexus::UI

#endif // NEXUS_VOICE_CONTROLLER_H
