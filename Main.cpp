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
int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!"); //later gotta implement scaling ?
    SimulationPlane simulationPlane;
    simulationPlane.initialize(70);

    bool isDraggin = false;
    sf::Vector2i mouse_pos;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

	    //update rotation
	    if(isDraggin) {
		    simulationPlane.Rotate((double)(sf::Mouse::getPosition().x - mouse_pos.x)/(double)1000, 0, 0);
		    simulationPlane.Rotate(0, 0, (double)(sf::Mouse::getPosition().y - mouse_pos.y)/(double)1000);
	    }

            if (event.type == sf::Event::MouseButtonPressed)
		{
			//if left button pressed
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if(!isDraggin) mouse_pos = sf::Mouse::getPosition();
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) isDraggin = true;
			}
		}

            if (event.type == sf::Event::MouseButtonReleased)
		{
			//if left button pressed
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) isDraggin = false;
			}
		}
        }

        float dt = clock.getElapsedTime().asSeconds();
        simulationPlane.simulate(dt*10);

        window.clear();
	simulationPlane.SortFaces();
        window.draw(simulationPlane);
        window.display();
    }

    return 0;
}
