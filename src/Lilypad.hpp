#pragma once

#include <cstdint>

struct SGpuInformation {
    int64_t ThermalSensorReading; // in C
    int64_t ThermalSensorReadingSource; // 1 - gpu
    int64_t GpuCoreTemperature; // in C
    int64_t CurrentVoltage;

    int64_t ClockSpeedCurrent;
    int64_t ClockSpeedMin;
    int64_t ClockSpeedMax;
    int64_t MemoryClockSpeed;
    int64_t MemoryClockSpeedMin;
    int64_t MemoryClockSpeedMax;
    int64_t MemoryTransferRate;
    int64_t MemoryTransferRateSpeedMin;
    int64_t MemoryTransferRateSpeedMax;
};

auto LoadLilypad() -> bool;
auto UnloadLilypad() -> void;
auto UpdateGpuInformation(int32_t gpuIndex, SGpuInformation* gpuInformation) -> bool;
