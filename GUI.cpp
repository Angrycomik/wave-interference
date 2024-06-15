#include "GUI.hpp"

void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, sf::Time& timeSteps, bool& isPaused) {
    ImGui::Begin("Control Panel");

    float availableWidth = ImGui::GetContentRegionAvail().x;

    ImGui::Checkbox("Show Animation", &isAnimationRunning);
    float buttonWidth = ImGui::CalcTextSize("Next Step").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    float buttonOffsetFromRight = availableWidth - buttonWidth;

    if (buttonOffsetFromRight > 0)
        ImGui::SameLine(buttonOffsetFromRight + 8);

    if (ImGui::Button("Next Step")) {
        if (!isAnimationRunning) {
            NextFrame(simulationPlane, clock, pausedTime, timeSteps);
            clock.restart();
        }
    }

    if (ImGui::Button("Save window to Clipboard", ImVec2(availableWidth, 0.0f))) {
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        if (texture.copyToImage().saveToFile("screen.png"))
        {
            CopyToClipboard(texture);
        }
    }

    if (ImGui::Button("Add Wave Source", ImVec2(availableWidth, 0.0f)))
    {
        ImGui::OpenPopup("WaveSourceContext");
    }

    if (ImGui::BeginPopup("WaveSourceContext"))
    {
        ImGui::Text("Wave Source Configuration");
        ImGui::Separator();

        ImGui::Text("Origin");
        ImGui::Text("X");
        ImGui::SameLine();
        static float origin[2] = { 0.0f, 0.0f };
        ImGui::SliderFloat("##originX", &origin[0], -50.0f, 50.0f, "%.1f");

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SliderFloat("##originY", &origin[1], -50.0f, 50.0f, "%.1f");

        ImGui::Text("Amplitude");
        ImGui::Text("A");
        ImGui::SameLine();
        static float amplitude = 1.0f;
        ImGui::SliderFloat("##amplitude", &amplitude, 0.0f, 2.0f, "%.01f");

        ImGui::Text("Frequency");
        ImGui::Text("f");
        ImGui::SameLine();
        static float frequency = 1.0f;
        ImGui::SliderFloat("##frequency", &frequency, 0.0f, 2.0f, "%.01f");

        ImGui::Separator();

        if (ImGui::Button("Add Wave Source", ImVec2(availableWidth, 0.0f)))
        {
            simulationPlane.addWaveSource(sf::Vector2f(origin[0], origin[1]), amplitude, frequency);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    std::vector<WaveSource> waveSources = simulationPlane.getWaveSources();
    int i = 0;
    for (const auto& waveSource : waveSources) {
        ImGui::PushID(i);

        availableWidth = ImGui::GetContentRegionAvail().x;

        ImGui::BeginChild("WaveSourceChild", ImVec2(0, 110), true);

        ImGui::Text("Wave Source #%d", i + 1);
        ImGui::Separator();
        ImGui::Text("Origin: (%.1f, %.1f)", waveSource.origin.x, waveSource.origin.y);
        ImGui::Text("Amplitude: %.1f", waveSource.amplitude);
        ImGui::Text("Frequency: %.1f", waveSource.frequency);
        if (ImGui::Button("Delete Wave Source", ImVec2(availableWidth - 16, 0.0f)))
        {
            simulationPlane.removeWaveSource(i);
        }

        ImGui::EndChild();

        ImGui::PopID();
        i++;

        ImGui::Spacing();
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
        float dt = (clock.getElapsedTime() + pausedTime + timeSteps).asSeconds();
        simulationPlane.simulate(dt * 5);
    }
}

void NextFrame(SimulationPlane& simulationPlane, sf::Clock& clock, sf::Time& pausedTime, sf::Time& timeSteps) {
    sf::Time timeStep = sf::seconds(1.0f / 10.0f);
    timeSteps += timeStep;
    float dt = timeSteps.asSeconds() + pausedTime.asSeconds();
    simulationPlane.simulate(dt * 5);
}

void CopyToClipboard(const sf::Texture& texture) {
    sf::Image image = texture.copyToImage();
    int width = image.getSize().x;
    int height = image.getSize().y;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    const sf::Uint8* pixels = image.getPixelsPtr();

    std::vector<sf::Uint8> converted(width * height * 4);
    for (int i = 0; i < width * height; i++) {
        converted[i * 4 + 0] = pixels[i * 4 + 2];
        converted[i * 4 + 1] = pixels[i * 4 + 1];
        converted[i * 4 + 2] = pixels[i * 4 + 0];
        converted[i * 4 + 3] = pixels[i * 4 + 3];
    }

    if (OpenClipboard(NULL)) {
        EmptyClipboard();

        HGLOBAL bitmap = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + (width * height * 4));
        if (bitmap) {
            void* lock = GlobalLock(bitmap);
            if (lock) {
                memcpy(lock, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
                memcpy((BYTE*)lock + sizeof(BITMAPINFOHEADER), converted.data(), width * height * 4);

                GlobalUnlock(bitmap);
                SetClipboardData(CF_DIB, bitmap);
            }
            GlobalFree(bitmap);
        }
        CloseClipboard();
    }
}