// Directives
#pragma execution_character_set("utf-8")

// Headers
#include "core/core.hpp"
#include "utilities.hpp"

// ************** TIMESTAMP UTILITIES ************** //

bool is_valid_timestamp_format(const char* fmt)
{
    // Nullptr check
    if (!fmt) return false;

    // Valid chars inside the format string
    constexpr const char* valid_specifiers = "aAbBcCdDeFgGhHIjmMnOpPrRsSuUVwWxXyYzZ";
    constexpr const char* valid_separators = "-_:/.· ";

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++; // Move to the next character after '%'

            if (!*fmt || !strchr("aAbBcCdDeFgGhHIjmMnOpPrRsSuUVwWxXyYzZ", *fmt))
                return false;
        }
        else if (!strchr(valid_separators, *fmt))
        {
            return false;
        }

        fmt++;
    }
    return true;
}

string get_current_timestamp(const char* fmt, bool miliseconds)
{
    // Validate timestamp format
    if (!is_valid_timestamp_format(fmt))
    {
        Logger::error("CORE", "Invalid format time!");
        return string("ERROR get_current_timestamp");
    }

    // Get the current local date and time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Use localtime_s for safer local time conversion
    std::tm localTime;
    if (localtime_s(&localTime, &now_time_t) != 0)
    {
        Logger::error("CORE", "Could not retrieve local time!");
        return string("ERROR get_current_timestamp");
    }

    // Format the time using the provided format
    std::ostringstream oss;
    oss << std::put_time(&localTime, fmt);

    // Format miliseconds if requested
    if (miliseconds)
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

// ************** FILE UTILITIES ************** //

file_size get_file_size(const string& file_path)
{
    if (!fs::exists(file_path))
    {
        Logger::error("CORE", "File does not exist:" + file_path);
        return { 0.0, "B" };
    }

    const uintmax_t bytes = fs::file_size(file_path);

    constexpr double kb_factor = 1024.0;
    constexpr double mb_factor = kb_factor * 1024.0;
    constexpr double gb_factor = mb_factor * 1024.0;

    // Check GB first
    if (double gb_size = static_cast<double>(bytes) / gb_factor; gb_size >= 1.0)
        return { std::floor(gb_size * 100) / 100, "GB" };
    // Then MB
    if (double mb_size = static_cast<double>(bytes) / mb_factor; mb_size >= 1.0)
        return { std::floor(mb_size * 100) / 100, "MB" };
    // Then KB
    if (double kb_size = static_cast<double>(bytes) / kb_factor; kb_size >= 1.0)
        return { std::floor(kb_size * 100) / 100, "KB" };

    // Default to bytes
    return { static_cast<double>(bytes), "B" };
}

// ************** STRING UTILITIES ************** //

string to_list(const vector<string>& vec)
{
    std::ostringstream oss;

    for (size_t i = 0; i < vec.size(); ++i)
    {
        oss << vec[i];
        if (i != vec.size() - 1)
            oss << ", ";
    }

    return oss.str();
}

string trim(const string& str)
{
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    return (start < end) ? string(start, end) : string();
}

string trim_trailing_zeros(const double number, const bool remove_point) 
{
    string str_number = std::to_string(number);

    while (!str_number.empty() && str_number.back() == '0')
        str_number.pop_back();

    if (remove_point && str_number.back() == '.')
        str_number.pop_back();

    return str_number;
}



