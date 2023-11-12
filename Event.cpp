// Event.cpp

#include "Event.hh"
#include "Vector2.hh"
#include "Arc.hh"
#include "VoronoiDiagram.hh"

// Site event constructor implementation
Event::Event(VoronoiDiagram::Site* site) : type(Type::SITE), y(site->point.y), index(-1), site(site) {
    // Additional implementation if needed...
}

// Circle event constructor implementation
Event::Event(double y, Vector2 point, Arc* arc) : type(Type::CIRCLE), y(y), index(-1), point(point), arc(arc) {
    // Additional implementation if needed...
}

// Less-than operator implementation
bool operator<(const Event& lhs, const Event& rhs) {
    return lhs.y < rhs.y;
}

// Output stream operator implementation
std::ostream& operator<<(std::ostream& os, const Event& event) {
    if (event.type == Event::Type::SITE)
        os << "S(" << event.site->index << ", " << event.y << ")";
    else
        os << "C(" << event.arc << ", " << event.y << ", " << event.point << ")";
    return os;
}
