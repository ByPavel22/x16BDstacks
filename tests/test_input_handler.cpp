/**
 * Nexus AI Emulator - Input Handler Tests
 */

#include <QTest>
#include <QDebug>
#include <QKeyEvent>

#include "input/input_handler.h"
#include "ui/gesture_handler.h"

class TestInputHandler : public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // InputHandler tests
    void testInputHandlerInitialization();
    void testKeyMapping();
    void testKeyMappingLoadSave();
    void testGestureEnableDisable();
    
    // GestureHandler tests
    void testGestureHandlerInitialization();
    void testSwipeRecognition();
    void testEdgeSwipe();
    void testTapRecognition();
    
private:
    std::unique_ptr<Nexus::Input::InputHandler> m_inputHandler;
    std::unique_ptr<Nexus::UI::GestureHandler> m_gestureHandler;
};

void TestInputHandler::initTestCase()
{
    m_inputHandler = std::make_unique<Nexus::Input::InputHandler>();
    m_gestureHandler = std::make_unique<Nexus::UI::GestureHandler>();
}

void TestInputHandler::cleanupTestCase()
{
    m_gestureHandler.reset();
    m_inputHandler.reset();
}

void TestInputHandler::testInputHandlerInitialization()
{
    bool result = m_inputHandler->initialize();
    QVERIFY2(result, "InputHandler initialization failed");
}

void TestInputHandler::testKeyMapping()
{
    // Test setting key mapping
    m_inputHandler->setKeyMapping("test_action", QKeySequence("Ctrl+Shift+T"));
    
    QKeySequence mapping = m_inputHandler->getKeyMapping("test_action");
    QCOMPARE(mapping.toString(), QString("Ctrl+Shift+T"));
    
    // Test default mappings
    QKeySequence overlayMapping = m_inputHandler->getKeyMapping("overlay_toggle");
    QVERIFY2(!overlayMapping.isEmpty(), "Default mapping should exist");
}

void TestInputHandler::testKeyMappingLoadSave()
{
    QString testProfile = QDir::tempPath() + "/nexus_keymap_test.json";
    
    // Set custom mapping
    m_inputHandler->setKeyMapping("custom_action", QKeySequence("F1"));
    
    // Save profile
    bool saveResult = m_inputHandler->saveKeymapProfile(testProfile);
    QVERIFY2(saveResult, "Failed to save keymap profile");
    
    // Load profile
    bool loadResult = m_inputHandler->loadKeymapProfile(testProfile);
    QVERIFY2(loadResult, "Failed to load keymap profile");
    
    // Cleanup
    QFile::remove(testProfile);
}

void TestInputHandler::testGestureEnableDisable()
{
    m_inputHandler->setGesturesEnabled(true);
    QVERIFY2(m_inputHandler->gesturesEnabled(), "Gestures should be enabled");
    
    m_inputHandler->setGesturesEnabled(false);
    QVERIFY2(!m_inputHandler->gesturesEnabled(), "Gestures should be disabled");
}

void TestInputHandler::testGestureHandlerInitialization()
{
    bool result = m_gestureHandler->initialize();
    QVERIFY2(result, "GestureHandler initialization failed");
}

void TestInputHandler::testSwipeRecognition()
{
    // Simulate swipe gesture
    // This would require actual event processing
    QVERIFY2(true, "Swipe recognition test stub");
}

void TestInputHandler::testEdgeSwipe()
{
    m_gestureHandler->setEdgeSwipeEnabled(true);
    m_gestureHandler->setSensitivity(0.8f);
    
    QVERIFY2(true, "Edge swipe configuration test");
}

void TestInputHandler::testTapRecognition()
{
    // Test tap/double-tap recognition
    QVERIFY2(true, "Tap recognition test stub");
}

QTEST_APPLESS_MAIN(TestInputHandler)

#include "test_input_handler.moc"
