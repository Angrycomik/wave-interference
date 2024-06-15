#pragma once
#ifndef GUI_hpp
#define GUI_hpp

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "SimulationPlane.hpp"
#include <Windows.h>



void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, sf::Time& timeSteps, bool& isPaused);
void NextFrame(SimulationPlane& simulationPlane, sf::Clock& clock, sf::Time& pausedTime, sf::Time& timeSteps);
void CopyToClipboard(const sf::Texture& texture);



#endif