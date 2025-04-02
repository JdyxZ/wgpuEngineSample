#pragma once

// Headers
#include "core/core.hpp"

struct ProjectInfo
{
public:
    static const string version;
    static const string build_configuration;
    static const string compiler;

private:
    static string getBuildConfiguration();
    static string getCompilerInfo();
};
