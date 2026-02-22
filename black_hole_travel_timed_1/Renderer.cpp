#include "Renderer.hpp"
#include <random>

// Create background stars.
Renderer::Renderer(unsigned width, unsigned height)
{
    generateStarfield(width, height);
}

void Renderer::generateStarfield(unsigned width, unsigned height)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(0.f, (float)width);
    std::uniform_real_distribution<float> yDist(0.f, (float)height);

    for (int i = 0; i < 400; ++i)
        stars.emplace_back(xDist(rng), yDist(rng));
}

void Renderer::drawStarfield(sf::RenderWindow& window)
{
    for (auto& star : stars)
    {
        sf::CircleShape dot(1.2f);
        dot.setPosition(star);
        dot.setFillColor(sf::Color(200, 200, 255));
        window.draw(dot);
    }
}

void Renderer::drawTrail(sf::RenderWindow& window,
    const Body& body)
{
    for (size_t i = 1; i < body.trail.size(); ++i)
    {
        float alpha =
            255.f * (float)i / body.trail.size();

        sf::Vertex line[] =
        {
            sf::Vertex(body.trail[i - 1],
                       sf::Color(body.color.r,
                                 body.color.g,
                                 body.color.b,
                                 (sf::Uint8)alpha)),
            sf::Vertex(body.trail[i],
                       sf::Color(body.color.r,
                                 body.color.g,
                                 body.color.b,
                                 (sf::Uint8)alpha))
        };

        window.draw(line, 2, sf::Lines);
    }
}

void Renderer::drawGlow(sf::RenderWindow& window,
    const Body& body)
{
    if (!body.isBlackHole) return;

    // Event horizon glow ring.
    sf::CircleShape glow(body.radius + 10.f);
    glow.setOrigin(body.radius + 10.f,
        body.radius + 10.f);
    glow.setPosition(body.position);
    glow.setFillColor(sf::Color::Transparent);
    glow.setOutlineThickness(4.f);
    glow.setOutlineColor(sf::Color(150, 0, 255));
    window.draw(glow);
}

void Renderer::draw(sf::RenderWindow& window,
    const std::vector<Body>& bodies)
{
    drawStarfield(window);

    for (const auto& body : bodies)
    {
        drawTrail(window, body);

        drawGlow(window, body);

        sf::CircleShape shape(body.radius);
        shape.setOrigin(body.radius, body.radius);
        shape.setPosition(body.position);
        shape.setFillColor(body.color);

        window.draw(shape);
    }
}