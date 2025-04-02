#pragma once

// Headers
#include "core.hpp"

// Directory names
const string textures_directory = "images";
const string models_directory = "models";
const string output_directory = "render";
const string logs_directory = "logs";

// Paths
const string cwd = fs::current_path().string();
const string textures_path = cwd + "\\" + textures_directory + "\\";
const string models_path = cwd + "\\" + models_directory + "\\";
const string output_path = cwd + "\\" + output_directory + "\\";
const string logs_path = cwd + "\\" + logs_directory + "\\";
