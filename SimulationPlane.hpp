#ifndef SimulationPlane_hpp
#define SimulationPlane_hpp

#include <SFML/Graphics.hpp>
#include <vector>

struct WaveSource {
	sf::Vector2f origin;
	float amplitude;
	float frequency;

	WaveSource() : origin(0.0f, 0.0f), amplitude(0.0f), frequency(0.0f) {}
	WaveSource(sf::Vector2f origin, float amplitude, float frequency) : origin(origin), amplitude(amplitude), frequency(frequency) {}
};

class SimulationPlane : public sf::Drawable {
private:
	std::vector<sf::Vector3f> m_vertices;
	std::vector<WaveSource> m_waveSources;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:
	void initialize(size_t size, float step);
	void simulate(float dt);
};

#endif
