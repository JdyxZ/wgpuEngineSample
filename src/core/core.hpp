#pragma once

// Macros
#define NOMINMAX             // Prevent min/max macros
#define WIN32_LEAN_AND_MEAN  // Reduce Windows.h bloat

// External Headers
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <optional>
#include <tuple>
#include <algorithm>
#include <vector>
#include <array>
#include <format>
#include <list>
#include <map>
#include <type_traits>
#include <utility>
#include <windows.h>

// C++ std usings
using std::make_shared;
using std::make_pair;
using std::shared_ptr;
using std::unique_ptr;
using std::nullopt;
using std::optional;
using std::string;
using std::vector;
using std::tuple;
using std::pair;
using std::floor;
using std::array;

// Namespaces
namespace fs = std::filesystem;

// Internal headers
#include "utils/logger.hpp"
#include "paths.hpp"
#include "constants.hpp"

