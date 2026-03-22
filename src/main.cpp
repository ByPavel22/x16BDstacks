/**
 * Nexus AI Emulator - Main Entry Point
 * 
 * Copyright (c) 2024-2026 Nexus Team. MIT License.
 * 
 * Next-Generation Android 13 Emulator with AI-Powered Enhancement
 */

#ifdef USE_QT_STUBS
#include "qt_stubs.h"
#include <iostream>
#else
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QMessageBox>
#endif

#include "application.h"

/**
 * @brief Application entry point
 * 
 * Initializes the Nexus AI Emulator with the following steps:
 * 1. Parse command-line arguments
 * 2. Initialize application
 * 3. Load configuration
 * 4. Start emulator core
 * 5. Show UI overlay
 * 
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 = success)
 */
int main(int argc, char* argv[])
{
#ifdef USE_QT_STUBS
    // Stub mode - minimal initialization
    std::cout << "========================================\n";
    std::cout << "  Nexus AI Emulator v0.2.0-redesign\n";
    std::cout << "  Android 13 Emulator with AI Upscaling\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Running in stub mode (Qt not available)\n";
    std::cout << "For full functionality, install Qt 6.5+\n\n";
    
    // Initialize application in stub mode
    Nexus::Application nexusApp;
    
    if (!nexusApp.initialize("")) {
        std::cerr << "Failed to initialize application\n";
        return -1;
    }
    
    std::cout << "Application initialized successfully\n";
    std::cout << "Press Enter to exit...\n";
    
    std::cin.get();
    
    nexusApp.shutdown();
    
    return 0;
#else
    // Full Qt mode
    // Set high DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    app.setApplicationName("Nexus AI Emulator");
    app.setApplicationVersion("0.2.0-redesign");
    app.setOrganizationName("Nexus");
    app.setWindowIcon(QIcon(":/icons/nexus_icon.png"));
    
    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Android 13 Emulator with AI-powered upscaling and frame generation"
    );
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Custom options
    QCommandLineOption configOption(
        QStringList() << "c" << "config",
        "Path to configuration file",
        "config_path",
        "configs/nexus-config.yaml"
    );
    parser.addOption(configOption);
    
    QCommandLineOption noAiOption(
        QStringList() << "no-ai",
        "Disable AI upscaling features"
    );
    parser.addOption(noAiOption);
    
    QCommandLineOption verboseOption(
        QStringList() << "v" << "verbose",
        "Enable verbose logging"
    );
    parser.addOption(verboseOption);
    
    QCommandLineOption apkOption(
        QStringList() << "apk",
        "APK file to launch on startup",
        "apk_path"
    );
    parser.addOption(apkOption);
    
    parser.process(app);
    
    // Get configuration path
    QString configPath = parser.value(configOption);
    if (!QFile::exists(configPath)) {
        configPath = QCoreApplication::applicationDirPath() + "/" + configPath;
    }
    
    qDebug() << "Nexus AI Emulator v0.2.0-redesign";
    qDebug() << "Configuration:" << configPath;
    
    // Create and initialize application
    try {
        Nexus::Application nexusApp;
        
        // Initialize with configuration
        if (!nexusApp.initialize(configPath)) {
            qCritical() << "Failed to initialize application";
            return -1;
        }
        
        // Apply command-line overrides
        if (parser.isSet(noAiOption)) {
            nexusApp.disableAI();
        }
        
        if (parser.isSet(verboseOption)) {
            nexusApp.setVerboseLogging(true);
        }
        
        // Launch APK if specified
        if (parser.isSet(apkOption)) {
            QString apkPath = parser.value(apkOption);
            nexusApp.queueApkLaunch(apkPath);
        }
        
        // Show main window and run
        nexusApp.show();
        
        int result = app.exec();
        
        // Cleanup
        nexusApp.shutdown();
        
        return result;
        
    } catch (const std::exception& e) {
        QMessageBox::critical(
            nullptr, 
            "Critical Error", 
            QString("Fatal error: %1").arg(e.what())
        );
        return -1;
    }
#endif
}
