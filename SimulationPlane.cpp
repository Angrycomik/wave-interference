#include "SimulationPlane.hpp"

void SimulationPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Matrix projectionMatrix = {};
    Matrix cameraMatrix = {};

    Projection::createLookAtMatrix(m_cameraPos, { 0, 0, 0 }, cameraMatrix);

    sf::Vector2u renderSize = target.getSize();

    Projection::createProjectionMatrix(m_fieldOfView, m_near, m_far, projectionMatrix);

    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (auto& vertex : m_vertices) {
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    size_t size = static_cast<size_t>(std::sqrt(m_vertices.size()));
    std::vector<Triangle> triangles;

    for (size_t x = 0; x < size - 1; ++x) {
        for (size_t y = 0; y < size - 1; ++y) {
            size_t i1 = x * size + y;
            size_t i2 = i1 + 1;
            size_t i3 = i1 + size;
            size_t i4 = i3 + 1;

            triangles.push_back({ {i1, i2, i3}, (m_vertices[i1].z + m_vertices[i2].z + m_vertices[i3].z) / 3 });
            triangles.push_back({ {i3, i2, i4}, (m_vertices[i3].z + m_vertices[i2].z + m_vertices[i4].z) / 3 });
        }
    }

    std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b) {
        return a.averageDepth > b.averageDepth;
        });

    sf::VertexArray vertexArray(sf::Triangles);

    for (const auto& triangle : triangles) {
        std::vector<sf::Vector3f> projectedPoints(3);
        for (size_t i = 0; i < 3; ++i) {
            sf::Vector3f camera, projected;
            Projection::multiply(m_vertices[triangle.indices[i]], camera, cameraMatrix);
            Projection::multiply(camera, projected, projectionMatrix);

            if (projected.z != 0) {
                projected.x /= -projected.z;
                projected.y /= -projected.z;
            }

            float x = (projected.x + 1) * renderSize.x / 2;
            float y = (1 - projected.y) * renderSize.y / 2;

            projectedPoints[i] = { x, y, projected.z };
        }

        auto addVertex = [&](size_t idx) {
            float ratio = (m_vertices[triangle.indices[idx]].z - minZ) / (maxZ - minZ);
            sf::Uint8 red = static_cast<sf::Uint8>(255 * ratio);
            sf::Uint8 blue = static_cast<sf::Uint8>(255 * (1 - ratio));
            vertexArray.append(sf::Vertex(sf::Vector2f(projectedPoints[idx].x, projectedPoints[idx].y), sf::Color(red, 0, blue)));
            };

        addVertex(0);
        addVertex(1);
        addVertex(2);
    }

    target.draw(vertexArray, states);
}

void SimulationPlane::initialize(size_t size, float step) {
	m_vertices.clear();
    for (size_t x = 0; x < size; x++) {
        for (size_t y = 0; y < size; y++) {
            float centeredX = (x - size / 2.0f) * step;
            float centeredY = (y - size / 2.0f) * step;
            m_vertices.push_back(sf::Vector3f(centeredX, centeredY, 0));
        }
    }

	m_waveSources.clear();
	m_waveSources.push_back(WaveSource(sf::Vector2f(0, 0), 1, 0.75));
	m_waveSources.push_back(WaveSource(sf::Vector2f(10, 10), 1, 0.75));
}

void SimulationPlane::addWaveSource(sf::Vector2f position, float amplitude, float frequency) {
    m_waveSources.push_back(WaveSource(position, amplitude, frequency));
}

const std::vector<WaveSource>& SimulationPlane::getWaveSources() const {
    return m_waveSources;
}

void SimulationPlane::removeWaveSource(size_t index) {
    if (index < m_waveSources.size()) {
        m_waveSources.erase(m_waveSources.begin() + index);
    }
}

void SimulationPlane::simulate(float dt) {
    for (auto& vertex : m_vertices) {
        float newZ = 0.0f;
        for (const auto& waveSource : m_waveSources) {
            float distance = std::hypot(vertex.x - waveSource.origin.x, vertex.y - waveSource.origin.y);
            float timeDelay = distance / 1.0f;
            newZ += waveSource.amplitude * std::sin(waveSource.frequency * (dt - timeDelay));
        }
        vertex.z = newZ + offsetZ;
    }
}

void SimulationPlane::handleMouseButtonPressed(const sf::Event::MouseButtonEvent& event) {
    if (event.button == sf::Mouse::Left) {
        m_isMouseButtonPressed = true;
        m_lastMousePosition = sf::Vector2i(event.x, event.y);
    }
}

void SimulationPlane::handleMouseButtonReleased(const sf::Event::MouseButtonEvent& event) {
    if (event.button == sf::Mouse::Left) {
        m_isMouseButtonPressed = false;
    }
}

void SimulationPlane::handleMouseMoved(const sf::Event::MouseMoveEvent& event) {
    if (m_isMouseButtonPressed) {
        sf::Vector2i newMousePosition(event.x, event.y);
        sf::Vector2i delta = newMousePosition - m_lastMousePosition;
        m_lastMousePosition = newMousePosition;

        float sensitivity = 0.005f;
        float theta = sensitivity * delta.x;
        float phi = sensitivity * delta.y;

        float radius = std::sqrt(m_cameraPos.x * m_cameraPos.x + m_cameraPos.y * m_cameraPos.y + m_cameraPos.z * m_cameraPos.z);
        float currentTheta = std::atan2(m_cameraPos.y, m_cameraPos.x);
        float currentPhi = std::acos(m_cameraPos.z / radius);

        currentTheta += theta;
        currentPhi += phi;

        currentPhi = std::max(static_cast<float>(M_PI) / 2.0f + 0.01f, std::min(currentPhi, static_cast<float>(M_PI) - 0.01f));

        m_cameraPos.x = radius * std::sin(currentPhi) * std::cos(currentTheta);
        m_cameraPos.y = radius * std::sin(currentPhi) * std::sin(currentTheta);
        m_cameraPos.z = radius * std::cos(currentPhi);

        if (m_cameraPos.z > 0.0f) {
            m_cameraPos.z = 0.0f;
        }
    }
}