// Facility.hh
#ifndef FACILITY_HH
#define FACILITY_HH

#include "Vector2.hh"

class Facility {
public:
    Vector2 position;
    // Add any other relevant attributes for facilities

    Facility(Vector2 pos) : position(pos) {}
};

#endif // FACILITY_HH
