/**

    @file      SimulationPlane.hpp
    @brief	   SimulationPlane class has the functions to draw and fully control the simulation, as well as some other processes like mouse rotations. 

**/
#pragma once
#ifndef SimulationPlane_hpp
#define SimulationPlane_hpp

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

#include "Projection.hpp"

/**
	@struct WaveSource
	@brief  structure which represents wave source in a simulation
**/
struct WaveSource {
	sf::Vector2f origin; ///< origin point of the wave source
	float amplitude; ///< amplitude of the wave source
	float frequency; ///< frequency of the wave source

	/**
		@brief WaveSource object constructor, initializes the wave source with zero amplitude, frequency and origin
	**/
	WaveSource() : origin(0.0f, 0.0f), amplitude(0.0f), frequency(0.0f) {}

	/**
		@brief WaveSource object constructor, initializes the wave source with given amplitude,frequency and origin
		@param origin origin point of the wave source
		@param amplitude amplitude of the wave source
		@param frequency frequency of the wave source
	**/
	WaveSource(sf::Vector2f origin, float amplitude, float frequency) : origin(origin), amplitude(amplitude), frequency(frequency) {}
};

/**
	@struct Triangle
	@brief  structure representing small triangles, which mesh is made from
**/
struct Triangle {
	size_t indices[3]; ///< an array for storing triangle's vertices
	float averageDepth; ///< average depth of the triangle
};

/**
	@class SimulationPlane
	@brief  class that represents plane and has the functions to draw and control it
**/
class SimulationPlane : public sf::Drawable {
private:
	std::vector<sf::Vector3f> m_vertices; ///< vertices of the plane
	std::vector<WaveSource> m_waveSources; ///< array that holds created wave sources

	sf::Vector3f m_cameraPos = { 50, 0, -50 }; ///< camera position
	bool m_isMouseButtonPressed = false; ///< boolean value for checking if mouse button is pressed
	sf::Vector2i m_lastMousePosition; ///< last mouse position

	float m_fieldOfView = 60.0f; ///< camera's FOV
	float m_near = 0.1f; ///< near clipping
	float m_far = 100.0f; ///< far clipping 
	float offsetZ = -5.0f; ///< Z offset

	/**
		@brief draws the plane on given window
		@param target window to draw the plane on
		@param states render states
	**/
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:
	/**
		@brief handles the mouse button pressed event, changes the value of boolean variable
		@param event the mouse button event
	**/
	void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& event);

	/**
		@brief handles the mouse button released event, changes the value of boolean variable
		@param event mouse button event
	**/
	void handleMouseButtonReleased(const sf::Event::MouseButtonEvent& event);

	/**
		@brief handles the mouse movement
		@param event mouse move event
	**/
	void handleMouseMoved(const sf::Event::MouseMoveEvent& event);

	/**
		@brief adds a wave source to the simulation plane
		@param position position of the wave source
		@param amplitude amplitude of the wave source
		@param frequency frequency of the wave source
	**/
	void addWaveSource(sf::Vector2f position, float amplitude, float frequency);

	/**
		@brief function that returns wavesources array
		@return WaveSource array
	**/
	const std::vector<WaveSource>& getWaveSources() const;

	/**
		@brief function to remove WaveSource from WaveSource array
		@param index index of the wave source to remove
	**/
	void removeWaveSource(size_t index);

	/**
		@brief initializes the plane and adds example wavesources to it
		@param size  size of the plane
		@param step step size for the simulation
	**/
	void initialize(size_t size, float step);

	/**
		@brief runs the animation for given 
		@param dt timestep for simulation
	**/
	void simulate(float dt);
};

#endif