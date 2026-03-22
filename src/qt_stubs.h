// Qt compatibility stubs for compilation without Qt
// This file provides minimal stubs for compilation checking

#ifndef QT_STUBS_H
#define QT_STUBS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

// Minimal QObject stub
class QObject {
public:
    virtual ~QObject() = default;
    void* parent() const { return nullptr; }
};

// Minimal QString stub
class QString {
public:
    QString() = default;
    QString(const char* s) : m_str(s ? s : "") {}
    QString(const std::string& s) : m_str(s) {}
    const char* toUtf8() const { return m_str.c_str(); }
    const char* toStdString() const { return m_str.c_str(); }
    bool isEmpty() const { return m_str.empty(); }
    bool contains(const QString& s) const { return m_str.find(s.m_str) != std::string::npos; }
    QString toLower() const { 
        std::string lower = m_str;
        for (auto& c : lower) c = tolower(c);
        return QString(lower);
    }
private:
    std::string m_str;
};

// Minimal QByteArray stub  
class QByteArray {
public:
    QByteArray() = default;
    QByteArray(const char* s) : m_data(s ? s : "") {}
    bool isEmpty() const { return m_data.empty(); }
    const char* constData() const { return m_data.c_str(); }
    int size() const { return static_cast<int>(m_data.size()); }
private:
    std::string m_data;
};

// Minimal QImage stub
class QImage {
public:
    enum Format { Format_ARGB32, Format_RGB32, Format_Invalid };
    QImage() : m_width(0), m_height(0), m_format(Format_Invalid) {}
    QImage(int w, int h, Format f) : m_width(w), m_height(h), m_format(f) {}
    bool isNull() const { return m_format == Format_Invalid; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    void fill(int color) {}
private:
    int m_width, m_height;
    Format m_format;
};

// Minimal QColor stub
class QColor {
public:
    QColor() : r(0), g(0), b(0), a(255) {}
    QColor(int _r, int _g, int _b, int _a = 255) : r(_r), g(_g), b(_b), a(_a) {}
private:
    int r, g, b, a;
};

// Minimal QPoint stub
class QPoint {
public:
    QPoint() : x(0), y(0) {}
    QPoint(int _x, int _y) : x(_x), y(_y) {}
    int x, y;
};

// Minimal QRect stub
class QRect {
public:
    QRect() : x(0), y(0), w(0), h(0) {}
    QRect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
    int x, y, w, h;
};

// Minimal QTimer stub
class QTimer : public QObject {
public:
    QTimer(QObject* parent = nullptr) {}
    void setInterval(int ms) {}
    void setSingleShot(bool single) {}
    void start() {}
    void stop() {}
};

// Minimal QScopedPointer stub
template<typename T>
class QScopedPointer {
public:
    QScopedPointer() : ptr(nullptr) {}
    QScopedPointer(T* p) : ptr(p) {}
    ~QScopedPointer() { delete ptr; }
    T* get() const { return ptr; }
    T* data() const { return ptr; }
    void reset(T* p = nullptr) { delete ptr; ptr = p; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
private:
    T* ptr;
};

// Minimal QElapsedTimer stub
class QElapsedTimer {
public:
    void start() {}
    qint64 elapsed() const { return 0; }
    qint64 nsecsElapsed() const { return 0; }
};

// Minimal Qt namespace
namespace Qt {
    enum GlobalColor { white, black, red, green, blue, transparent };
    enum TransformationMode { IgnoreAspectRatio, KeepAspectRatio };
    enum TextureCoordinateMode {};
};

// Debug macros
#define qDebug() std::cerr << "[DEBUG] "
#define qInfo() std::cerr << "[INFO] "
#define qWarning() std::cerr << "[WARNING] "
#define qCritical() std::cerr << "[ERROR] "

// Signal/Slot macros (no-op for stub)
#define signals public:
#define slots:
#define emit
#define Q_OBJECT
#define Q_PROPERTY(...)

#endif // QT_STUBS_H
