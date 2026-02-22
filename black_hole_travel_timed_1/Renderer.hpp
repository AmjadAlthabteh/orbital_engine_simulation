#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Body.hpp"

// Responsible for all visuals.
class Renderer
{
private:
    std::vector<sf::Vector2f> stars;

    void generateStarfield(unsigned width, unsigned height);
    void drawStarfield(sf::RenderWindow& window);
    void drawTrail(sf::RenderWindow& window, const Body& body);
    void drawGlow(sf::RenderWindow& window, const Body& body);

public:
    Renderer(unsigned width, unsigned height);

    void draw(sf::RenderWindow& window,
        const std::vector<Body>& bodies);
};