#include <SFML/Graphics.hpp>
#include "SimulationPlane.hpp"
#include "GUI.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#ifdef NDEBUG
#include <Windows.h>
#endif

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!"); //later gotta implement scaling ?
    ImGui::SFML::Init(window);
    SimulationPlane simulationPlane;
    simulationPlane.initialize(70);

    bool isPaused = false;
    bool isAnimationRunning = false;
    bool isDraggin = false;
    bool canDrag = false; // by korzystac z ui
    sf::Vector2i mouse_pos;

    sf::Clock clock;
    sf::Clock deltaClock;
    sf::Time pausedTime = sf::Time::Zero;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();

         Rotate(simulationPlane,event,isDraggin,mouse_pos,canDrag);
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        DrawGUI(window, simulationPlane,isAnimationRunning,clock, pausedTime, isPaused,canDrag);

        window.clear();
	    simulationPlane.SortFaces();
        window.draw(simulationPlane);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
