#include "GUI.hpp"

void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, bool& isPaused) {
    ImGui::Begin("Control Panel");
    ImGui::Checkbox("Animation", &isAnimationRunning);
    if (ImGui::Button("Next Frame")) {
        if (!isAnimationRunning) {
            NextFrame(simulationPlane, clock, pausedTime);
        }
    }
    if (ImGui::Button("Make Screenshot")) {
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        if (texture.copyToImage().saveToFile("screen.png"))
        {
            CopyTextureToClipboard(texture);
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

void CopyTextureToClipboard(const sf::Texture& texture) {
    sf::Image image = texture.copyToImage();
    int width = image.getSize().x;
    int height = image.getSize().y;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // inaczej odwrocony
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    const sf::Uint8* pixels = image.getPixelsPtr();

    // konwersja z rgb na bgr
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

