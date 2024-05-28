#include <SFML/Graphics.hpp>
#include "SimulationPlane.hpp"
#ifdef NDEBUG
#include <Windows.h>
#endif

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    SimulationPlane simulationPlane;
    simulationPlane.initialize(200, 1.0f);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.getElapsedTime().asSeconds();
        simulationPlane.simulate(dt * 5);

        window.clear();
        window.draw(simulationPlane);
        window.display();
    }

    return 0;
}