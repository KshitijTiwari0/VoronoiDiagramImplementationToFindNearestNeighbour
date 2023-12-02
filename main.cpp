
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
// SFML
#include <SFML/Graphics.hpp>
// My includes
#include "FortuneAlgorithm.hh"

#include "Customer.hh"

#include "Facility.hh"
#include "GeometryUtils.hh"

constexpr float WINDOW_WIDTH = 600.0f;
constexpr float WINDOW_HEIGHT = 600.0f;
constexpr float POINT_RADIUS = 0.005f;
constexpr float OFFSET = 1.0f;

Vector2 calculateMean(const std::vector<Vector2>& vertices);


Facility findRegionAndFacility(const Vector2& mean, const std::vector<std::vector<Vector2>>& facilityVertices, const std::vector<Facility>& facilities)
{
    for (std::size_t i = 0; i < facilityVertices.size(); ++i)
    {
        const std::vector<Vector2>& cellVertices = facilityVertices[i];

        // Calculate the mean of Voronoi cell vertices
        Vector2 cellMean = calculateMean(cellVertices);

        // Check if the overall mean lies within the Voronoi cell
        if (isPointInPolygon(mean, cellVertices))
        {
            // If found, return the corresponding facility
            return facilities[i];
        }
    }

    // If no matching region is found, return an "invalid" facility
    return Facility{ Vector2{-1.0, -1.0} }; // You might want to handle this case differently based on your requirements
}

std::vector<Vector2> generatePoints(int nbCustomers, int nbFacilities) {
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<Vector2> points;
    for (int i = 0; i < nbCustomers; ++i)
        points.push_back(Vector2{ distribution(generator), distribution(generator) });

    for (int i = 0; i < nbFacilities; ++i)
        points.push_back(Vector2{ distribution(generator), distribution(generator) });

    return points;
}

void drawPoint(sf::RenderWindow& window, Vector2 point, sf::Color color)
{
    sf::CircleShape shape(POINT_RADIUS);
    shape.setPosition(sf::Vector2f(point.x - POINT_RADIUS, 1 - point.y - POINT_RADIUS));
    shape.setFillColor(color);
    window.draw(shape);
}

void drawEdge(sf::RenderWindow& window, Vector2 origin, Vector2 destination, sf::Color color)
{
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(origin.x, 1.0f - origin.y), color),
        sf::Vertex(sf::Vector2f(destination.x, 1.0f - destination.y), color)
    };
    window.draw(line, 2, sf::Lines);
}

void drawPoints(sf::RenderWindow& window, VoronoiDiagram& diagram)
{
    for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
        drawPoint(window, diagram.getSite(i)->point, sf::Color(100, 250, 50));
}

void drawDiagram(sf::RenderWindow& window, VoronoiDiagram& diagram)
{
    for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
    {
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        Vector2 center = site->point;
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;
        if (halfEdge == nullptr)
            continue;
        while (halfEdge->prev != nullptr)
        {
            halfEdge = halfEdge->prev;
            if (halfEdge == face->outerComponent)
                break;
        }
        VoronoiDiagram::HalfEdge* start = halfEdge;
        while (halfEdge != nullptr)
        {
            if (halfEdge->origin != nullptr && halfEdge->destination != nullptr)
            {
                Vector2 origin = (halfEdge->origin->point - center) * OFFSET + center;
                Vector2 destination = (halfEdge->destination->point - center) * OFFSET + center;
                drawEdge(window, origin, destination, sf::Color::Red);
            }
            halfEdge = halfEdge->next;
            if (halfEdge == start)
                break;
        }
    }
}

std::vector<Customer> generateCustomers(int nbCustomers) {
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<Customer> customers;
    for (int i = 0; i < nbCustomers; ++i)
        customers.emplace_back(Customer{ Vector2{distribution(generator), distribution(generator)} });

    return customers;
}

std::vector<Facility> generateFacilities(int nbFacilities) {
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<Facility> facilities;
    for (int i = 0; i < nbFacilities; ++i)
        facilities.emplace_back(Facility{ Vector2{distribution(generator), distribution(generator)} });

    return facilities;
}



VoronoiDiagram generateRandomDiagram(const std::vector<Customer>& customers, const std::vector<Facility>& facilities) {
    // Combine customer and facility points for Voronoi diagram generation
    std::vector<Vector2> allPoints;
    for (const auto& customer : customers) {
        allPoints.push_back(customer.position);
    }
    for (const auto& facility : facilities) {
        allPoints.push_back(facility.position);
    }

    // Construct diagram
    FortuneAlgorithm algorithm(allPoints);
    auto start = std::chrono::steady_clock::now();
    algorithm.construct();
    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << "construction: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';

    // (rest of the function remains unchanged)

    // Bound the diagram
    start = std::chrono::steady_clock::now();
    algorithm.bound(Box{ -0.05, -0.05, 1.05, 1.05 }); // Take the bounding box slightly bigger than the intersection box
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "bounding: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    VoronoiDiagram diagram = algorithm.getDiagram();

    // Intersect the diagram with a box
    start = std::chrono::steady_clock::now();
    bool valid = diagram.intersect(Box{ 0.0, 0.0, 1.0, 1.0 });
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    if (!valid)
        throw std::runtime_error("An error occurred in the box intersection algorithm");

    return diagram;

}

std::vector<std::vector<Vector2>> extractVertices(VoronoiDiagram& diagram)
{
    std::vector<std::vector<Vector2>> vertices;

    for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
    {
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;

        if (halfEdge == nullptr)
            continue;

        std::vector<Vector2> cellVertices;
        VoronoiDiagram::HalfEdge* start = halfEdge;
        std::size_t iterationCounter = 0;

        do
        {
            if (halfEdge->origin != nullptr)
            {
                Vector2 vertex = halfEdge->origin->point;
                cellVertices.push_back(vertex);
            }

            halfEdge = halfEdge->next;
            ++iterationCounter;

            if (iterationCounter >= 1000)
            {
                std::cerr << "Warning: Maximum iteration limit reached for Voronoi cell " << i << ". Possible construction issue.\n";
                // Break or handle the situation as appropriate
                break;
            }

        } while (halfEdge != nullptr && halfEdge != start);

        vertices.push_back(cellVertices);
    }

    return vertices;
}



Vector2 calculateMean(const std::vector<Vector2>& vertices)
{
    Vector2 sum = { 0.0, 0.0 };

    for (const auto& vertex : vertices)
    {
        sum.x += vertex.x;
        sum.y += vertex.y;
    }

    if (!vertices.empty())
    {
        sum.x /= vertices.size();
        sum.y /= vertices.size();
    }

    return sum;
}




int main()
{
    std::size_t nbCustomers = 50;
    std::size_t nbFacilities = 10;

    // Generate customers and facilities
    std::vector<Customer> customers = generateCustomers(nbCustomers);
    std::vector<Facility> facilities = generateFacilities(nbFacilities);

    // Generate Voronoi diagram using customer and facility points
    VoronoiDiagram diagram = generateRandomDiagram(customers, facilities);

    std::size_t nbPoints = 100;

    // Display the diagram
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Fortune's algorithm", sf::Style::Default, settings);
    window.setView(sf::View(sf::FloatRect(-0.1f, -0.1f, 1.2f, 1.2f)));
    const int MAX_ITERATIONS = 10000;  // Set a reasonable maximum iteration count
    int iterationCount = 0;

    while (window.isOpen() && iterationCount < MAX_ITERATIONS)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::N)
            {
                // Generate customers and facilities
                customers = generateCustomers(nbCustomers);
                facilities = generateFacilities(nbFacilities);

                // Generate Voronoi diagram using customer and facility points
                diagram = generateRandomDiagram(customers, facilities);
            }
        }

        // Rest of your loop code...

        window.clear(sf::Color::Black);

        drawDiagram(window, diagram);
        drawPoints(window, diagram);

        // Extract vertices and calculate mean for each cell
        auto vertices = extractVertices(diagram);
        for (const auto& cellVertices : vertices)
        {
            Vector2 mean = calculateMean(cellVertices);
            std::cout << "Mean of Voronoi cell: (" << mean.x << ", " << mean.y << ")\n";
        }

        // Find the region and corresponding facility for the overall mean
        Vector2 overallMean = calculateMean(vertices[0]);  // Assuming you want the mean of the first cell
        Facility regionFacility = findRegionAndFacility(overallMean, vertices, facilities);

        if (regionFacility.position.x >= 0.0 && regionFacility.position.y >= 0.0)
        {
            std::cout << "Facility in the region: (" << regionFacility.position.x << ", " << regionFacility.position.y << ")\n";
        }
        else
        {
            std::cout << "No matching facility found for the overall mean.\n";
        }

        // Increment iteration count
        ++iterationCount;

        window.display();
    }

    return 0;
}
