#ifndef SimulationPlane_hpp
#define SimulationPlane_hpp

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

#include "Projection.hpp"

struct WaveSource {
	sf::Vector2f origin;
	float amplitude;
	float frequency;

	WaveSource() : origin(0.0f, 0.0f), amplitude(0.0f), frequency(0.0f) {}
	WaveSource(sf::Vector2f origin, float amplitude, float frequency) : origin(origin), amplitude(amplitude), frequency(frequency) {}
};

struct Triangle {
	size_t indices[3];
	float averageDepth;
};

class SimulationPlane : public sf::Drawable {
private:
	std::vector<sf::Vector3f> m_vertices;
	std::vector<WaveSource> m_waveSources;

	sf::Vector3f m_cameraPos = { 50, 0, -50 };
	bool m_isMouseButtonPressed = false;
	sf::Vector2i m_lastMousePosition;

	float m_fieldOfView = 60.0f;
	float m_near = 0.1f;
	float m_far = 100.0f;

	float offsetZ = -5.0f;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:
	void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& event);
	void handleMouseButtonReleased(const sf::Event::MouseButtonEvent& event);
	void handleMouseMoved(const sf::Event::MouseMoveEvent& event);

	void addWaveSource(sf::Vector2f position, float amplitude, float frequency);
	const std::vector<WaveSource>& getWaveSources() const;
	void removeWaveSource(size_t index);

	void initialize(size_t size, float step);
	void simulate(float dt);
};

#endif