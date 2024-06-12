#include "GUI.hpp"

void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, bool& isPaused) {
    ImGui::Begin("Control Panel");
    ImGui::Checkbox("Animation", &isAnimationRunning);
    if (ImGui::Button("Next Frame")) {
        if (!isAnimationRunning) {
            NextFrame(simulationPlane, clock, pausedTime);
        }
    }

    ImGui::End();

    if (isAnimationRunning && isPaused) {
        clock.restart();
        isPaused = false;
    }
    else if (!isAnimationRunning && !isPaused) {
        pausedTime += clock.getElapsedTime();
        isPaused = true;
    }
    if (isAnimationRunning) {
        float dt = (clock.getElapsedTime() + pausedTime).asSeconds();
        simulationPlane.simulate(dt*10);
    }
}
void NextFrame(SimulationPlane& simulationPlane, sf::Clock& clock, sf::Time& pausedTime) {
    float dt = (clock.getElapsedTime() + pausedTime).asSeconds();
    simulationPlane.simulate(dt * 10);
    pausedTime += clock.getElapsedTime();
}

void Rotate(SimulationPlane& simulationPlane, sf::Event& event, bool &isDraggin,sf::Vector2i &mouse_pos) {
    //update rotation
    if (isDraggin) {
        simulationPlane.Rotate((double)(sf::Mouse::getPosition().x - mouse_pos.x) / (double)1000, 0, 0);
        simulationPlane.Rotate(0, 0, (double)(sf::Mouse::getPosition().y - mouse_pos.y) / (double)1000);
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        //if left button pressed
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (!isDraggin) mouse_pos = sf::Mouse::getPosition();
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

