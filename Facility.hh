// Facility.hh
#ifndef FACILITY_HH
#define FACILITY_HH

#include "Vector2.hh"

class Facility
{
public:
    Vector2 position;

    // Add a default constructor
    Facility() : position(Vector2()) {}

    // Add a constructor that takes a Vector2
    Facility(const Vector2& pos) : position(pos) {}

    // Add an equality operator
    bool operator==(const Facility& other) const
    {
        return position == other.position;
    }
};

#endif // FACILITY_HH
