#pragma once

// Headers
#include "core/core.hpp"

struct SystemInfo
{
public:
    static const string platform;
    static const int cpu_threads;

private:
    static string GetCPUModel();
    static string GetGPUModel();
    static string getPlatform();
    static int getActiveThreads();
};




