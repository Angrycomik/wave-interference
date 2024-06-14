#pragma once
#ifndef GUI_hpp
#define GUI_hpp

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include "SimulationPlane.hpp"
#include <Windows.h>



void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, bool& isPaused,bool& canDrag);
void NextFrame(SimulationPlane& simulationPlane, sf::Clock& clock, sf::Time& pausedTime);
void Rotate(SimulationPlane& simulationPlane, sf::Event& event, bool &isDraggin, sf::Vector2i& mouse_pos, bool& canDrag);
void CopyToClipboard(const sf::Texture& texture);



#endif