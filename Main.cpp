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
    sf::RenderWindow window(sf::VideoMode(800, 800), "wave-interference");
    ImGui::SFML::Init(window);
    SimulationPlane simulationPlane;
    simulationPlane.initialize(100, 0.5f);

    bool isPaused = false;
    bool isAnimationRunning = true;

    sf::Clock clock;
    sf::Clock deltaClock;
    sf::Time pausedTime = sf::Time::Zero;
    sf::Time timeSteps = sf::Time::Zero;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            ImGuiIO& io = ImGui::GetIO();
            bool isImGuiUsingMouse = io.WantCaptureMouse;

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (!isImGuiUsingMouse) {
                if (event.type == sf::Event::MouseButtonPressed)
                    simulationPlane.handleMouseButtonPressed(event.mouseButton);
                if (event.type == sf::Event::MouseButtonReleased)
                    simulationPlane.handleMouseButtonReleased(event.mouseButton);
                if (event.type == sf::Event::MouseMoved)
                    simulationPlane.handleMouseMoved(event.mouseMove);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        DrawGUI(window, simulationPlane, isAnimationRunning, clock, pausedTime, timeSteps, isPaused);

        window.clear(sf::Color::White);
        window.draw(simulationPlane);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
