// Customer.hh
#ifndef CUSTOMER_HH
#define CUSTOMER_HH

#include "Vector2.hh"

class Customer {
public:
    Vector2 position;
    // Add any other relevant attributes for customers

    Customer(Vector2 pos) : position(pos) {}
};

#endif // CUSTOMER_HH
