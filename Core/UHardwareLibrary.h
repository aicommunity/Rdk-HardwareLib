#ifndef UHardwareLibrary_H
#define UHardwareLibrary_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"
#include "../../../Rdk/Core/Engine/ModernContainers.h"
#include "../../../Rdk/Core/System/ModernChrono.h"
#include "../../../Rdk/Core/System/ModernMutex.h"

#include "UArduinoConnect.h"
#include "UArduinoControl.h"
#include "UAdcSensor.h"
#include "UDcControlDemo.h"

namespace RDK {

class RDK_LIB_TYPE UHardwareLibrary: public ULibrary
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UHardwareLibrary(void);
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);

// --------------------------

// --------------------------
// Modern C++20 methods
// --------------------------
// Move semantics
UHardwareLibrary(const UHardwareLibrary&) = default;
UHardwareLibrary(UHardwareLibrary&&) noexcept = default;
UHardwareLibrary& operator=(const UHardwareLibrary&) = default;
UHardwareLibrary& operator=(UHardwareLibrary&&) noexcept = default;

// Modern hardware operations
void OptimizeHardwarePerformance();
bool IsHardwareLibraryValid() const;
void ReserveHardwareMemory(size_t devices_count);
TimePoint GetLastHardwareUpdateTime() const;
void SetLastHardwareUpdateTime(TimePoint time);

// Modern hardware device operations
template<typename T>
void ReserveDeviceBuffer(std::vector<T>& buffer, size_t device_count);

template<typename T>
void MoveDeviceBuffer(std::vector<T>& target, std::vector<T>&& source);

// Advanced hardware algorithms
void UpdateHardwareParameters(const std::vector<double>& parameters);
void CalculateHardwareMetrics(const std::vector<double>& sensor_data);
void OptimizeHardwareConfiguration(double performance_target);
void ProcessHardwareDataStream(const std::vector<std::vector<double>>& data_stream);
// --------------------------
};

extern RDK_LIB_TYPE UHardwareLibrary HardwareLibrary;

}
#endif

