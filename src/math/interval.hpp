#pragma once

class Interval 
{
public:
    double min, max;
    static const Interval empty, universe, unitary;

    Interval(); // Default interval is empty
    Interval(double min, double max);
    Interval(const Interval& a, const Interval& b); // Creates the interval tightly enclosing the two input intervals.

    double size() const;
    bool is_empty() const;
    bool contains(double x) const;
    bool surrounds(double x) const;
    double clamp(double x) const;
    Interval expand(double delta) const;
};

// Operator overlaods
inline Interval operator+(const Interval& ival, double displacement) 
{
    return Interval(ival.min + displacement, ival.max + displacement);
}

inline Interval operator+(double displacement, const Interval& ival) 
{
    return ival + displacement;
}

inline Interval operator*(const Interval& ival, double displacement) 
{
    return Interval(ival.min * displacement, ival.max * displacement);
}

inline Interval operator*(double displacement, const Interval& ival) 
{
    return ival * displacement;
}


