

bool isPointInPolygon(const Vector2& point, const std::vector<Vector2>& polygon) {
    bool inside = false;
    std::size_t n = polygon.size();

    for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
            (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }

    return inside;
}
