// Qt compatibility stubs for compilation without Qt
// Comprehensive stubs for all Qt classes used in the project
// Version: 0.2.0-redesign

#ifndef QT_STUBS_H
#define QT_STUBS_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <functional>
#include <initializer_list>

// Type definitions
using qint64 = int64_t;
using quint64 = uint64_t;
using qint32 = int32_t;
using quint32 = uint32_t;

// Forward declarations
class QString;
class QByteArray;
class QStringList;
class QObject;
class QTimer;
class QProcess;

// Forward declare template classes
template<typename T> class QVector;
template<typename T> using QList = QVector<T>;
template<typename Key, typename T> class QMap;
template<typename T> class QSet;

// ============================================================================
// QString - String class stub
// ============================================================================
class QString {
public:
    QString() = default;
    QString(const char* s) : m_str(s ? s : "") {}
    QString(const std::string& s) : m_str(s) {}
    QString(int i) : m_str(std::to_string(i)) {}
    QString(double d) : m_str(std::to_string(d)) {}
    
    const char* toUtf8() const { return m_str.c_str(); }
    std::string toStdString() const { return m_str; }
    bool isEmpty() const { return m_str.empty(); }
    bool contains(const QString& s) const { 
        return m_str.find(s.m_str) != std::string::npos; 
    }
    QString toLower() const { 
        std::string lower = m_str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return QString(lower);
    }
    QString trimmed() const {
        auto start = m_str.find_first_not_of(" \t\n\r");
        auto end = m_str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return QString();
        return QString(m_str.substr(start, end - start + 1));
    }
    int toInt() const { return std::stoi(m_str); }
    double toDouble() const { return std::stod(m_str); }
    
    QString arg(int i) const {
        std::string result = m_str;
        size_t pos = result.find("%1");
        if (pos != std::string::npos)
            result.replace(pos, 2, std::to_string(i));
        return QString(result);
    }
    
    QString arg(const QString& s) const {
        std::string result = m_str;
        size_t pos = result.find("%1");
        if (pos != std::string::npos)
            result.replace(pos, 2, s.m_str);
        return QString(result);
    }
    
    // Operators
    QString operator+(const QString& other) const {
        return QString(m_str + other.m_str);
    }
    bool operator==(const QString& other) const {
        return m_str == other.m_str;
    }
    bool operator!=(const QString& other) const {
        return m_str != other.m_str;
    }
    
private:
    std::string m_str;
};

// ============================================================================
// QByteArray - Byte array stub
// ============================================================================
class QByteArray {
public:
    QByteArray() = default;
    QByteArray(const char* s) : m_data(s ? s : "") {}
    QByteArray(const std::string& s) : m_data(s) {}
    
    bool isEmpty() const { return m_data.empty(); }
    const char* constData() const { return m_data.c_str(); }
    int size() const { return static_cast<int>(m_data.size()); }
    QByteArray trimmed() const { return *this; }
    bool contains(const QString& s) const { 
        return m_data.find(s.toStdString()) != std::string::npos; 
    }
    
private:
    std::string m_data;
};

// ============================================================================
// QVector - Vector stub (MUST come before QList usage in QMap)
// ============================================================================
template<typename T>
class QVector : public std::vector<T> {
public:
    using std::vector<T>::vector;
    
    QVector() = default;
    QVector(std::initializer_list<T> list) {
        this->insert(this->end(), list);
    }
    
    void append(const T& value) { this->push_back(value); }
    void prepend(const T& value) { this->insert(this->begin(), value); }
    void removeAt(int index) { this->erase(this->begin() + index); }
    void removeFirst() { this->erase(this->begin()); }
    void removeLast() { this->pop_back(); }
    T takeAt(int index) {
        T value = this->at(index);
        this->erase(this->begin() + index);
        return value;
    }
    T first() const { return this->front(); }
    T last() const { return this->back(); }
    int size() const { return static_cast<int>(this->size()); }
    bool isEmpty() const { return this->empty(); }
    void clear() { this->clear(); }
    T& operator[](int index) { return std::vector<T>::operator[](index); }
    const T& operator[](int index) const { return std::vector<T>::operator[](index); }
    
    QVector& operator<<(const T& value) {
        this->push_back(value);
        return *this;
    }
};

// ============================================================================
// QList - List stub (alias for QVector)
// ============================================================================
template<typename T>
using QList = QVector<T>;

// ============================================================================
// QMap - Map stub
// ============================================================================
template<typename Key, typename T>
class QMap {
public:
    using value_type = std::pair<Key, T>;
    using iterator = typename std::map<Key, T>::iterator;
    using const_iterator = typename std::map<Key, T>::const_iterator;
    
    QMap() = default;
    
    T& operator[](const Key& key) { return m_map[key]; }
    const T& operator[](const Key& key) const { return m_map.at(key); }
    
    void insert(const Key& key, const T& value) { m_map[key] = value; }
    bool contains(const Key& key) const { return m_map.find(key) != m_map.end(); }
    int size() const { return static_cast<int>(m_map.size()); }
    bool isEmpty() const { return m_map.empty(); }
    void remove(const Key& key) { m_map.erase(key); }
    void clear() { m_map.clear(); }
    
    iterator begin() { return m_map.begin(); }
    iterator end() { return m_map.end(); }
    const_iterator begin() const { return m_map.begin(); }
    const_iterator end() const { return m_map.end(); }
    const_iterator constBegin() const { return m_map.begin(); }
    const_iterator constEnd() const { return m_map.end(); }
    
    T value(const Key& key, const T& defaultValue = T()) const {
        auto it = m_map.find(key);
        return (it != m_map.end()) ? it->second : defaultValue;
    }
    
    QList<T> values() const {
        QList<T> result;
        for (const auto& pair : m_map) {
            result.append(pair.second);
        }
        return result;
    }
    
private:
    std::map<Key, T> m_map;
};

// ============================================================================
// QSet - Set stub
// ============================================================================
template<typename T>
class QSet : public std::unordered_set<T> {
public:
    QSet() = default;
    QSet(std::initializer_list<T> list) {
        this->insert(list.begin(), list.end());
    }
    
    bool contains(const T& value) const {
        return this->find(value) != this->end();
    }
    int size() const { return static_cast<int>(this->size()); }
    bool isEmpty() const { return this->empty(); }
    void insert(const T& value) { this->emplace(value); }
    void remove(const T& value) { this->erase(value); }
    void clear() { this->clear(); }
    
    QSet& operator<<(const T& value) {
        this->insert(value);
        return *this;
    }
};

// ============================================================================
// QStringList - String list stub
// ============================================================================
class QStringList : public QVector<QString> {
public:
    QStringList() = default;
    QStringList(std::initializer_list<QString> list) {
        insert(end(), list);
    }
    
    QStringList& operator<<(const QString& s) {
        push_back(s);
        return *this;
    }
    QStringList& operator<<(const QStringList& list) {
        insert(end(), list.begin(), list.end());
        return *this;
    }
    QStringList operator+(const QStringList& other) const {
        QStringList result = *this;
        result.insert(result.end(), other.begin(), other.end());
        return result;
    }
    QStringList& operator=(std::initializer_list<QString> list) {
        clear();
        insert(end(), list);
        return *this;
    }
};

// ============================================================================
// QObject - Base object stub
// ============================================================================
class QObject {
public:
    QObject() : m_parent(nullptr) {}
    QObject(QObject* parent) : m_parent(parent) {}
    virtual ~QObject() = default;
    
    QObject* parent() const { return m_parent; }
    void setParent(QObject* parent) { m_parent = parent; }
    
protected:
    QObject* m_parent;
};

// ============================================================================
// QTimer - Timer stub
// ============================================================================
class QTimer : public QObject {
public:
    QTimer(QObject* parent = nullptr) : QObject(parent), m_interval(1000), m_singleShot(false) {}
    
    void setInterval(int ms) { m_interval = ms; }
    int interval() const { return m_interval; }
    void setSingleShot(bool single) { m_singleShot = single; }
    bool isSingleShot() const { return m_singleShot; }
    void start() {}
    void stop() {}
    bool isActive() const { return false; }
    
private:
    int m_interval;
    bool m_singleShot;
};

// ============================================================================
// QProcess - Process stub
// ============================================================================
class QProcess : public QObject {
public:
    enum ProcessError { 
        FailedToStart, 
        Crashed, 
        Timedout, 
        UnknownError 
    };
    
    enum ExitStatus { 
        NormalExit, 
        CrashExit 
    };
    
    QProcess(QObject* parent = nullptr) : QObject(parent) {}
    
    void start(const QString& program, const QStringList& arguments = QStringList()) {}
    bool waitForStarted(int msecs = 30000) { return true; }
    bool waitForFinished(int msecs = 30000) { return true; }
    void kill() {}
    void terminate() {}
    QByteArray readAllStandardOutput() { return QByteArray(); }
    QByteArray readAllStandardError() { return QByteArray(); }
    qint64 processId() const { return 0; }
    int exitCode() const { return 0; }
    ExitStatus exitStatus() const { return NormalExit; }
    bool isOpen() const { return false; }
    
    // Signals (stubs)
    void connect_started(std::function<void()> callback) {}
    void connect_finished(std::function<void(int)> callback) {}
    void connect_error(std::function<void(ProcessError)> callback) {}
};

// ============================================================================
// QDir - Directory stub
// ============================================================================
class QDir {
public:
    QDir() = default;
    QDir(const QString& path) : m_path(path) {}
    
    QString path() const { return m_path; }
    bool exists() const { return true; }
    bool mkdir(const QString& dirName) const { return true; }
    bool rmdir(const QString& dirName) const { return true; }
    bool mkpath(const QString& path) const { return true; }
    
    bool cd(const QString& dirName) { return true; }
    QString dirName() const { return m_path; }
    QString absolutePath() const { return m_path; }
    
    QStringList entryList() const { return QStringList(); }
    QStringList entryList(const QStringList& filters) const { return QStringList(); }
    
    static QString currentPath() { return "."; }
    static QDir current() { return QDir("."); }
    static QDir home() { return QDir("~"); }
    static QDir root() { return QDir("/"); }
    
    bool remove(const QString& fileName) const { return true; }
    
private:
    QString m_path;
};

// ============================================================================
// QFile - File stub
// ============================================================================
class QFile {
public:
    enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Binary = 0x0020
    };
    
    QFile() = default;
    QFile(const QString& name) : m_fileName(name) {}
    QFile(const QString& name, QObject* parent) : m_fileName(name) {}
    
    void setFileName(const QString& name) { m_fileName = name; }
    QString fileName() const { return m_fileName; }
    
    bool open(OpenMode mode) { return true; }
    bool open(OpenMode mode, const QString& deviceName) { return true; }
    void close() {}
    bool isOpen() const { return true; }
    
    qint64 read(char* data, qint64 maxSize) { return 0; }
    QByteArray readAll() { return QByteArray(); }
    qint64 write(const char* data, qint64 maxSize) { return maxSize; }
    qint64 write(const QByteArray& data) { return data.size(); }
    
    bool flush() { return true; }
    bool remove() { return true; }
    bool rename(const QString& newName) { return true; }
    bool copy(const QString& newName) { return true; }
    
    bool exists() const { return true; }
    qint64 size() const { return 0; }
    
    static bool exists(const QString& fileName) { return true; }
    static bool remove(const QString& fileName) { return true; }
    static bool rename(const QString& oldName, const QString& newName) { return true; }
    static bool copy(const QString& oldName, const QString& newName) { return true; }
    
private:
    QString m_fileName;
};

// ============================================================================
// QTextStream - Text stream stub
// ============================================================================
class QTextStream {
public:
    QTextStream() = default;
    QTextStream(QIODevice* device) : m_device(device) {}
    QTextStream(QIODevice* device, QObject* parent) : m_device(device) {}
    
    void setDevice(QIODevice* device) { m_device = device; }
    QIODevice* device() const { return m_device; }
    
    QTextStream& operator<<(const QString& s) { return *this; }
    QTextStream& operator<<(const char* s) { return *this; }
    QTextStream& operator<<(int i) { return *this; }
    QTextStream& operator<<(double d) { return *this; }
    
    QString readAll() { return QString(); }
    QString readLine() { return QString(); }
    
private:
    QIODevice* m_device = nullptr;
};

// ============================================================================
// QIODevice - IO Device stub
// ============================================================================
class QIODevice : public QObject {
public:
    enum OpenModeFlag {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Binary = 0x0020,
        Unbuffered = 0x0040
    };
    using OpenMode = OpenModeFlag;
    
    QIODevice(QObject* parent = nullptr) : QObject(parent) {}
    
    virtual bool open(OpenMode mode) { return true; }
    virtual void close() {}
    virtual bool isOpen() const { return true; }
    virtual bool isReadable() const { return true; }
    virtual bool isWritable() const { return true; }
    
    virtual qint64 read(char* data, qint64 maxSize) { return 0; }
    virtual QByteArray readAll() { return QByteArray(); }
    virtual qint64 write(const char* data, qint64 maxSize) { return maxSize; }
    
protected:
    OpenMode m_openMode = NotOpen;
};

// ============================================================================
// QImage - Image stub
// ============================================================================
class QImage {
public:
    enum Format { 
        Format_Invalid,
        Format_ARGB32,
        Format_ARGB32_Premultiplied,
        Format_RGB32,
        Format_RGB888,
        Format_Grayscale8
    };
    
    QImage() : m_width(0), m_height(0), m_format(Format_Invalid) {}
    QImage(int w, int h, Format f) : m_width(w), m_height(h), m_format(f) {}
    QImage(const QString& fileName) : m_width(0), m_height(0), m_format(Format_Invalid) {}
    
    bool isNull() const { return m_format == Format_Invalid; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    Format format() const { return m_format; }
    
    void fill(int color) {}
    void fill(unsigned int color) {}
    
    bool load(const QString& fileName) { return false; }
    bool save(const QString& fileName, const char* format = nullptr, int quality = -1) const { 
        return false; 
    }
    
    QImage scaled(int w, int h, int aspectRatioMode = 0, int transformMode = 0) const {
        return QImage(w, h, m_format);
    }
    
    bool convertToFormat(Format f) { 
        m_format = f; 
        return true; 
    }
    
private:
    int m_width;
    int m_height;
    Format m_format;
};

// ============================================================================
// QColor - Color stub
// ============================================================================
class QColor {
public:
    QColor() : m_r(0), m_g(0), m_b(0), m_a(255) {}
    QColor(int _r, int _g, int _b, int _a = 255) : m_r(_r), m_g(_g), m_b(_b), m_a(_a) {}
    QColor(const QString& name) : m_r(0), m_g(0), m_b(0), m_a(255) {}
    
    void setRgb(int _r, int _g, int _b, int _a = 255) { m_r = _r; m_g = _g; m_b = _b; m_a = _a; }
    int red() const { return m_r; }
    int green() const { return m_g; }
    int blue() const { return m_b; }
    int alpha() const { return m_a; }
    
    QColor lighter(int factor = 100) const { return *this; }
    QColor darker(int factor = 100) const { return *this; }
    
    static QColor fromRgb(int r, int g, int b, int a = 255) {
        return QColor(r, g, b, a);
    }
    
    static QColor fromHsv(int h, int s, int v, int a = 255) {
        // Simple HSV to RGB conversion (stub)
        return QColor(128, 128, 128, a);
    }
    
private:
    int m_r, m_g, m_b, m_a;
};

// ============================================================================
// QPoint - Point stub
// ============================================================================
class QPoint {
public:
    QPoint() : m_x(0), m_y(0) {}
    QPoint(int _x, int _y) : m_x(_x), m_y(_y) {}
    
    int x() const { return m_x; }
    int y() const { return m_y; }
    void setX(int _x) { m_x = _x; }
    void setY(int _y) { m_y = _y; }
    
    QPoint operator+(const QPoint& other) const {
        return QPoint(m_x + other.m_x, m_y + other.m_y);
    }
    QPoint operator-(const QPoint& other) const {
        return QPoint(m_x - other.m_x, m_y - other.m_y);
    }
    
    int m_x, m_y;
};

// ============================================================================
// QRect - Rectangle stub
// ============================================================================
class QRect {
public:
    QRect() : m_x(0), m_y(0), m_w(0), m_h(0) {}
    QRect(int _x, int _y, int _w, int _h) : m_x(_x), m_y(_y), m_w(_w), m_h(_h) {}
    QRect(const QPoint& topLeft, const QPoint& bottomRight) 
        : m_x(topLeft.x()), m_y(topLeft.y()), 
          m_w(bottomRight.x() - topLeft.x()), m_h(bottomRight.y() - topLeft.y()) {}
    
    int x() const { return m_x; }
    int y() const { return m_y; }
    int width() const { return m_w; }
    int height() const { return m_h; }
    int left() const { return m_x; }
    int right() const { return m_x + m_w; }
    int top() const { return m_y; }
    int bottom() const { return m_y + m_h; }
    
    QPoint topLeft() const { return QPoint(m_x, m_y); }
    QPoint bottomRight() const { return QPoint(m_x + m_w, m_y + m_h); }
    
    void moveLeft(int _x) { m_x = _x; }
    void moveTop(int _y) { m_y = _y; }
    void moveTo(const QPoint& pos) { m_x = pos.x(); m_y = pos.y(); }
    
    bool contains(const QPoint& point) const {
        return point.x() >= m_x && point.x() <= m_x + m_w && 
               point.y() >= m_y && point.y() <= m_y + m_h;
    }
    
    QRect adjusted(int dx1, int dy1, int dx2, int dy2) const {
        return QRect(m_x + dx1, m_y + dy1, m_w - dx1 - dx2, m_h - dy1 - dy2);
    }
    
private:
    int m_x, m_y, m_w, m_h;
};

// ============================================================================
// QSize - Size stub
// ============================================================================
class QSize {
public:
    QSize() : m_w(0), m_h(0) {}
    QSize(int _w, int _h) : m_w(_w), m_h(_h) {}
    
    int width() const { return m_w; }
    int height() const { return m_h; }
    void setWidth(int _w) { m_w = _w; }
    void setHeight(int _h) { m_h = _h; }
    
    QSize expanded(const QSize& other) const {
        return QSize(std::max(m_w, other.m_w), std::max(m_h, other.m_h));
    }
    
private:
    int m_w, m_h;
};

// ============================================================================
// QVariant - Variant stub
// ============================================================================
class QVariant {
public:
    enum Type {
        Invalid,
        Int,
        Double,
        String,
        Bool
    };
    
    QVariant() : m_type(Invalid) {}
    QVariant(int i) : m_int(i), m_type(Int) {}
    QVariant(double d) : m_double(d), m_type(Double) {}
    QVariant(const QString& s) : m_string(s), m_type(String) {}
    QVariant(bool b) : m_bool(b), m_type(Bool) {}
    
    int toInt() const { return m_int; }
    double toDouble() const { return m_double; }
    QString toString() const { return m_string; }
    bool toBool() const { return m_bool; }
    
private:
    union {
        int m_int;
        double m_double;
        bool m_bool;
    };
    QString m_string;
    Type m_type;
};

// ============================================================================
// QScopedPointer - Smart pointer stub
// ============================================================================
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
    bool isNull() const { return ptr == nullptr; }
    operator bool() const { return ptr != nullptr; }
    
private:
    T* ptr;
};

// ============================================================================
// QSharedPointer - Shared pointer stub
// ============================================================================
template<typename T>
class QSharedPointer {
public:
    QSharedPointer() : ptr(nullptr), refCount(nullptr) {}
    QSharedPointer(T* p) : ptr(p), refCount(p ? new int(1) : nullptr) {}
    
    ~QSharedPointer() { release(); }
    
    QSharedPointer(const QSharedPointer& other) 
        : ptr(other.ptr), refCount(other.refCount) {
        if (refCount) ++(*refCount);
    }
    
    QSharedPointer& operator=(const QSharedPointer& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            if (refCount) ++(*refCount);
        }
        return *this;
    }
    
    T* get() const { return ptr; }
    T* data() const { return ptr; }
    void reset() { release(); }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    bool isNull() const { return ptr == nullptr; }
    int use_count() const { return refCount ? *refCount : 0; }
    
private:
    void release() {
        if (refCount) {
            --(*refCount);
            if (*refCount == 0) {
                delete ptr;
                delete refCount;
            }
        }
        ptr = nullptr;
        refCount = nullptr;
    }
    
    T* ptr;
    int* refCount;
};

// ============================================================================
// QElapsedTimer - Timer stub
// ============================================================================
class QElapsedTimer {
public:
    QElapsedTimer() : m_running(false) {}
    
    void start() { m_running = true; }
    qint64 elapsed() const { return 0; }
    qint64 nsecsElapsed() const { return 0; }
    bool isValid() const { return m_running; }
    void invalidate() { m_running = false; }
    
private:
    bool m_running;
};

// ============================================================================
// QDebug - Debug output stub
// ============================================================================
class QDebug {
public:
    QDebug() {}
    
    QDebug& operator<<(const QString& s) { 
        std::cerr << s.toStdString(); 
        return *this; 
    }
    QDebug& operator<<(const char* s) { 
        std::cerr << s; 
        return *this; 
    }
    QDebug& operator<<(int i) { 
        std::cerr << i; 
        return *this; 
    }
    QDebug& operator<<(double d) { 
        std::cerr << d; 
        return *this; 
    }
};

// ============================================================================
// Qt namespace
// ============================================================================
namespace Qt {
    enum GlobalColor { 
        white, black, red, green, blue, yellow, magenta, cyan, 
        darkGray, gray, lightGray, transparent 
    };
    
    enum TransformationMode { 
        FastTransformation, 
        SmoothTransformation 
    };
    
    enum AspectRatioMode { 
        IgnoreAspectRatio, 
        KeepAspectRatio, 
        KeepAspectRatioByExpanding 
    };
    
    enum AlignmentFlag {
        AlignLeft = 0x0001,
        AlignRight = 0x0002,
        AlignHCenter = 0x0004,
        AlignTop = 0x0020,
        AlignBottom = 0x0040,
        AlignVCenter = 0x0080,
        AlignCenter = AlignTop | AlignBottom | AlignHCenter
    };
    
    using Alignment = int;
};

// ============================================================================
// Macros
// ============================================================================

// Debug macros
#define qDebug() QDebug()
#define qInfo() QDebug()
#define qWarning() QDebug() << "[WARNING] "
#define qCritical() QDebug() << "[ERROR] "

// Signal/Slot macros (no-op for stubs)
#ifdef USE_QT_STUBS
#define signals public:
#define slots public:
#define emit
#define Q_OBJECT
#define Q_PROPERTY(...)
#define Q_ENUM(...)
#else
// Let Qt handle these
#endif

// Connect macro (no-op for stubs)
#ifdef USE_QT_STUBS
#define connect(sender, signal, receiver, slot)
#endif

#endif // QT_STUBS_H
