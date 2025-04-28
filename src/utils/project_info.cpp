// Headers
#include "core/core.hpp" 
#include "project_info.hpp"

string ProjectInfo::getBuildConfiguration()
{
    #ifdef _DEBUG
        return "Debug";
    #elif defined(NDEBUG)
        return "Release";
    #else
        return "Unknown";
    #endif
}

string ProjectInfo::getCompilerInfo()
{
    string info;

    // Compiler
    #if defined(_MSC_VER)
        info = "MSVC " + std::to_string(_MSC_VER);
    #elif defined(__clang__)
        info = "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__);
    #elif defined(__GNUC__)
        info = "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
    #elif defined(__INTEL_COMPILER)
        info = "ICC " + std::to_string(__INTEL_COMPILER);
    #else
        info = "Unknown Compiler";
    #endif

    return info;
}

// Static attributes
const string ProjectInfo::version = "1.0";
const string ProjectInfo::build_configuration = getBuildConfiguration();
const string ProjectInfo::compiler = getCompilerInfo();
