/**
 * Nexus AI Emulator - Hypervisor
 * 
 * Low-latency hypervisor for Windows:
 * - WHPX (Windows Hypervisor Platform) integration
 * - KVM fallback for Linux
 * - Memory virtualization
 */

#ifndef NEXUS_HYPERVISOR_H
#ifdef USE_QT_STUBS
#include "../qt_stubs.h"
#else
#define NEXUS_HYPERVISOR_H

#include <QObject>
#endif
#include <memory>

namespace Nexus::Core {

/**
 * @brief Hypervisor backend type
 */
enum class HypervisorBackend {
    WHPX,       // Windows Hypervisor Platform
    KVM,        // Linux KVM
    HVF,        // macOS Hypervisor Framework
    None        // No hardware acceleration
};

/**
 * @brief Low-latency hypervisor controller
 */
class Hypervisor : public QObject
{
    Q_OBJECT
    
public:
    explicit Hypervisor(QObject *parent = nullptr);
    ~Hypervisor();
    
    /**
     * @brief Initialize hypervisor
     * @param preferredBackend Preferred backend
     * @return true if initialization successful
     */
    bool initialize(HypervisorBackend preferredBackend = HypervisorBackend::WHPX);
    
    /**
     * @brief Shutdown hypervisor
     */
    void shutdown();
    
    /**
     * @brief Create virtual machine
     * @param ramMb RAM allocation in MB
     * @param cpuCount Number of CPU cores
     * @return VM handle or -1 on failure
     */
    int createVm(int ramMb, int cpuCount);
    
    /**
     * @brief Destroy virtual machine
     * @param vmHandle VM handle
     */
    void destroyVm(int vmHandle);
    
    /**
     * @brief Start VM execution
     * @param vmHandle VM handle
     */
    void startVm(int vmHandle);
    
    /**
     * @brief Stop VM execution
     * @param vmHandle VM handle
     */
    void stopVm(int vmHandle);
    
    /**
     * @brief Get current backend
     */
    HypervisorBackend currentBackend() const { return m_backend; }
    
    /**
     * @brief Check if hypervisor is available
     */
    static bool isAvailable(HypervisorBackend backend);

signals:
    void vmCreated(int vmHandle);
    void vmDestroyed(int vmHandle);
    void vmError(int vmHandle, const QString& error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_initialized = false;
    HypervisorBackend m_backend = HypervisorBackend::None;
    
    // WHPX methods
    bool initializeWhpx();
    
    // KVM methods
    bool initializeKvm();
};

} // namespace Nexus::Core

#endif // NEXUS_HYPERVISOR_H
