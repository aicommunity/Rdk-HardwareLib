#ifndef UADCSENSOR_H
#define UADCSENSOR_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"
#include "../../../Rdk/Core/Engine/ModernContainers.h"
#include "../../../Rdk/Core/System/ModernChrono.h"
#include "../../../Rdk/Core/System/ModernMutex.h"

namespace RDK {

    class RDK_LIB_TYPE UADC: public UNet
    {
    public:
        // ����������� �� ���������
        UADC() = default;
        virtual ~UADC() = default;

        // --------------------------
        // Modern C++20 methods
        // --------------------------
        // Move semantics
        UADC(const UADC&) = default;
        UADC(UADC&&) noexcept = default;
        UADC& operator=(const UADC&) = default;
        UADC& operator=(UADC&&) noexcept = default;

        // Modern ADC operations
        void OptimizeADCPerformance();
        bool IsADCSensorValid() const;
        void ReserveADCMemory(size_t samples_count);
        TimePoint GetLastADCUpdateTime() const;
        void SetLastADCUpdateTime(TimePoint time);

        // Modern sensor operations
        template<typename T>
        void ReserveSensorBuffer(std::vector<T>& buffer, size_t buffer_size);

        template<typename T>
        void MoveSensorBuffer(std::vector<T>& target, std::vector<T>&& source);

        // Advanced ADC algorithms
        void UpdateADCParameters(const std::vector<double>& parameters);
        void CalculateADCMetrics(const std::vector<double>& sensor_data);
        void OptimizeADCConfiguration(double performance_target);
        void ProcessADCDataStream(const std::vector<std::vector<double>>& data_stream);
        // --------------------------
    };
}

#endif
