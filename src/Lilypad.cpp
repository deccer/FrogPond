#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <NVCtrl/NVCtrlLib.h>

#include <iostream>
#include "Lilypad.hpp"

#include <map>

static Display* g_x11Display = nullptr;

auto LoadLilypad() -> bool {

    g_x11Display = glfwGetX11Display();
    int32_t eventBase = 0;
    int32_t errorBase = 0;
    auto nvResult = XNVCTRLQueryExtension(g_x11Display, &eventBase, &errorBase);
    if (nvResult != True) {
        return false;
    }

    return true;
}

auto UnloadLilypad() -> void {
    g_x11Display = nullptr;
}

auto ToDictionary(std::string s, char delim = ':', const std::string& white = " \n\t\v\r\f") -> std::map<std::string, std::string> {

    std::map<std::string, std::string> m;
    if (white.empty()) {
        return m;
    }

    s += white[0];// necessary if s doesn't contain trailing spaces
    size_t pos = 0;
    auto removeLeading = [&](){ 
        if ((pos = s.find_first_not_of(white)) != std::string::npos) {
        s.erase(0, pos);
    }};
    auto maxInitWord = [&]() -> std::string {

        std::string word;
        if ((pos = s.find_first_of(white + delim)) != std::string::npos) {
            word = s.substr(0, pos);
            if (word.back() == ',') {
                word.pop_back();
            }
            s.erase(0, pos);
        }
        return word;
    };

    while (true)
    {
        std::string key;
        removeLeading();
        if ((key = maxInitWord()).empty()) {
            break;
        }
        removeLeading();
        if (s.empty() or s[0] != delim) {
            break;
        }
        s.erase(0, 1);
        removeLeading();
        if ((m[key] = maxInitWord()).empty()) {
            break;
        }
    }

    return m;
}

auto UpdateGpuInformation(int32_t gpuIndex, SGpuInformation* gpuInformation) -> bool {

    if (g_x11Display == nullptr) {
        return false;
    }

    auto result = XNVCTRLQueryTargetAttribute64(
        g_x11Display,
        NV_CTRL_TARGET_TYPE_THERMAL_SENSOR,
        gpuIndex,
        0,
        NV_CTRL_THERMAL_SENSOR_TARGET,
        &gpuInformation->ThermalSensorReadingSource);
    if (!result) {
        std::cout << "Failed to query temperature sensor source\n";
        return false;
    }

    result = XNVCTRLQueryTargetAttribute64(
        g_x11Display,
        NV_CTRL_TARGET_TYPE_THERMAL_SENSOR,
        gpuIndex,
        0,
        NV_CTRL_THERMAL_SENSOR_READING,
        &gpuInformation->ThermalSensorReading);
    if (!result) {
        std::cout << "Failed to query temperature sensor\n";
        return false;
    }

    result = XNVCTRLQueryTargetAttribute64(
        g_x11Display,
        NV_CTRL_TARGET_TYPE_GPU,
        gpuIndex,
        0,
        NV_CTRL_GPU_CORE_TEMPERATURE,
        &gpuInformation->GpuCoreTemperature);
    if (!result) {
        std::cout << "Failed to query gpu core temperature\n";
        return false;
    }

    int* data = nullptr;
    int dataLength = 0;
    char* stringData = nullptr;

    result = XNVCTRLQueryTargetStringAttribute(
        g_x11Display,
        NV_CTRL_TARGET_TYPE_GPU,
        gpuIndex,
        0,
        NV_CTRL_STRING_GPU_CURRENT_CLOCK_FREQS,
        &stringData);
    if (!result) {
        std::cout << "Failed to query current voltage\n";
        return false;
    }

    auto dictionary = ToDictionary(stringData, '=');
    free(stringData);

    gpuInformation->ClockSpeedCurrent = std::stoi(dictionary["nvclock"]);
    gpuInformation->ClockSpeedMin = std::stoi(dictionary["nvclockmin"]);
    gpuInformation->ClockSpeedMax = std::stoi(dictionary["nvclockmax"]);
    gpuInformation->MemoryClockSpeed = std::stoi(dictionary["memclock"]);
    gpuInformation->MemoryClockSpeedMin = std::stoi(dictionary["memclockmin"]);
    gpuInformation->MemoryClockSpeedMax = std::stoi(dictionary["memclockmax"]);
    gpuInformation->MemoryTransferRate = std::stoi(dictionary["memTransferRate"]);
    gpuInformation->MemoryTransferRateSpeedMin = std::stoi(dictionary["memTransferRatemin"]);
    gpuInformation->MemoryTransferRateSpeedMax = std::stoi(dictionary["memTransferRatemax"]);

    return true;
}
