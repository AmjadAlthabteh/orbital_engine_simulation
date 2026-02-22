
// spacehsip and more like mor epn prject based -- enhance two projects 
// Pleas elie in doeth program for the spaceship and liek do math and more 
// opengl and c++ code to show somethng 
#include <SFML/Graphics.hpp>
#include <vector>
#include "Body.hpp"
#include "PhysicsEngine.hpp"
#include "Renderer.hpp"

int main()
{
    const unsigned WIDTH = 1400;
    const unsigned HEIGHT = 900;

    sf::RenderWindow window(
        sf::VideoMode(WIDTH, HEIGHT),
        "Orbital N-Body Simulation");

    window.setFramerateLimit(60);

    std::vector<Body> bodies;

    // Central black hole.
    bodies.emplace_back(
        15000.f,
        35.f,
        sf::Vector2f(WIDTH / 2.f, HEIGHT / 2.f),
        sf::Vector2f(0.f, 0.f),
        sf::Color::Black,
        true);

    // Planet 1
    bodies.emplace_back(
        15.f,
        10.f,
        sf::Vector2f(WIDTH / 2.f + 300.f, HEIGHT / 2.f),
        sf::Vector2f(0.f, -280.f),
        sf::Color::Cyan);

    // Planet 2
    bodies.emplace_back(
        20.f,
        12.f,
        sf::Vector2f(WIDTH / 2.f - 450.f, HEIGHT / 2.f),
        sf::Vector2f(0.f, 220.f),
        sf::Color(255, 150, 50));

    PhysicsEngine physics(6000.f);
    Renderer renderer(WIDTH, HEIGHT);

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        physics.update(bodies, dt);

        window.clear(sf::Color(5, 5, 15));
        renderer.draw(window, bodies);
        window.display();
    }

    return 0;
}