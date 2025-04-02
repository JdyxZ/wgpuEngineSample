// Headers 
#include "core/core.hpp"
#include "chrono.hpp"

Chrono::Chrono() : total_elapsed(0) {}

void Chrono::start()
{
    if (!start_time.has_value())
        start_time = std::chrono::high_resolution_clock::now();
}

void Chrono::end()
{
    if (start_time.has_value())
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        total_elapsed += end_time - start_time.value();
        start_time.reset();
    }
}

elapsed_time Chrono::elapsed() const
{
    auto elapsed = total_elapsed;

    if (start_time.has_value())
        elapsed += std::chrono::high_resolution_clock::now() - start_time.value();

    auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    auto total_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    elapsed_time et;
    et.hours = int(total_seconds / 3600);
    et.minutes = int((total_seconds % 3600) / 60);
    et.seconds = int(total_seconds % 60);
    et.milliseconds = int(total_milliseconds % 1000);

    return et;
}

int Chrono::elapsed_miliseconds() const
{
    auto elapsed = total_elapsed;

    if (start_time.has_value())
        elapsed += std::chrono::high_resolution_clock::now() - start_time.value();

    return int(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
}

std::string Chrono::elapsed_to_string() const
{
    auto et = elapsed();

    std::ostringstream elapsed_str;
    if (et.hours > 0) elapsed_str << et.hours << "h ";
    if (et.minutes > 0) elapsed_str << et.minutes << "m ";
    if (et.seconds > 0) elapsed_str << et.seconds << "s ";
    elapsed_str << et.milliseconds << "ms";

    return elapsed_str.str();
}

Chrono& Chrono::operator+=(const Chrono& c)
{
    if (c.start_time.has_value())
        Logger::warn("Chrono", "The chrono you're adding has not finished timing. This could lead to inaccurate total elapsed time!");

    this->total_elapsed += c.total_elapsed;
    return *this;
}

Chrono Chrono::operator+(const Chrono& c) const
{
    if (c.start_time.has_value())
        Logger::warn("Chrono", "The chrono you're adding has not finished timing. This could lead to inaccurate total elapsed time!");

    Chrono result = *this;
    result += c;
    return result;
}

