#include "SimulationPlane.hpp"

void SimulationPlane::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();
	for (auto& vertex : m_vertices) {
		minZ = std::min(minZ, vertex.z);
		maxZ = std::max(maxZ, vertex.z);
	}

	sf::VertexArray vertexArray(sf::Points, m_vertices.size());
	for (size_t i = 0; i < m_vertices.size(); ++i) {
		float ratio = (m_vertices[i].z - minZ) / (maxZ - minZ);
		sf::Uint8 red = static_cast<sf::Uint8>(255 * ratio);
		sf::Uint8 blue = static_cast<sf::Uint8>(255 * (1 - ratio));

		vertexArray[i].position = sf::Vector2f(m_vertices[i].x * 5, m_vertices[i].y * 5);
		vertexArray[i].color = sf::Color(red, 0, blue);
	}
	target.draw(vertexArray, states);
}

void SimulationPlane::initialize(size_t size, float step) {
	srand(time(NULL));
	m_vertices.clear();
	for (size_t x = 0; x < size; x++) {
		for (size_t y = 0; y < size; y++) {
			m_vertices.push_back(sf::Vector3f(x * step, y * step, x + y));
		}
	}

	m_waveSources.clear();
	m_waveSources.push_back(WaveSource(sf::Vector2f(50.0f, 50.0f), 1.0f, 1.0f));
	m_waveSources.push_back(WaveSource(sf::Vector2f(100.0f, 100.0f), 1.0f, 0.1f));
	m_waveSources.push_back(WaveSource(sf::Vector2f(50.0f, 100.0f), 1.0f, 0.5f));
}

void SimulationPlane::simulate(float dt) {
	for (auto& vertex : m_vertices) {
		float newZ = 0.0f;
		for (const auto& waveSource : m_waveSources) {
			float distance = std::hypot(vertex.x - waveSource.origin.x, vertex.y - waveSource.origin.y);
			newZ += waveSource.amplitude * std::sin(waveSource.frequency * dt - distance);
		}
		vertex.z = newZ;
	}
}