#pragma once

class vec {
public:
    virtual ~vec() = default; 

    virtual double length() const = 0;
    virtual double length_squared() const = 0;
    virtual vec& normalize() = 0;
    virtual bool near_zero() const = 0;
};

