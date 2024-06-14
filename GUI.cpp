#include "GUI.hpp"

void DrawGUI(sf::RenderWindow& window, SimulationPlane& simulationPlane, bool& isAnimationRunning, sf::Clock& clock, sf::Time& pausedTime, bool& isPaused,bool& canDrag) {
    static int count = 0;
    static bool showLimitText = false;
    static bool showNextFrameText = false;
    ImGui::Begin("Control Panel");
    ImGui::Checkbox("Animation", &isAnimationRunning);
    ImGui::SameLine();
    ImGui::Checkbox("Rotate Plane", &canDrag);
    if (ImGui::Button("Next Frame")) {
        if (!isAnimationRunning && count == 2) {
            NextFrame(simulationPlane, clock, pausedTime);
            showNextFrameText = false;
        }
        if (count != 2) {
            showNextFrameText = true;
        }
    }

    if (showNextFrameText) {
        ImGui::SameLine();
        ImGui::Text("You need 2 wave sources! :c");
    }

    if (ImGui::Button("Make Screenshot")) {
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        if (texture.copyToImage().saveToFile("screen.png"))
        {
            CopyToClipboard(texture);
        }
    }

    if (ImGui::Button("Add Node")) {
        if (count < 2) { ImGui::OpenPopup("My Popup"); }
        else { showLimitText = true; }
    }
    if (showLimitText) {
        //ImGui::SameLine();
        ImGui::Text("Too much wave sources! :)");
    }

    if (ImGui::BeginPopupContextItem("My Popup"))
    {
        static float positionX = 0.2f; 
        static float positionY = 0.3f;
        static float amplitude = 0.04f;
        static float frequency = 1.0f;

        ImGui::Text("Example position and frequency will be set by clicking 'Add' 2 times.");
        ImGui::InputFloat("Position X", &positionX);  // Zrobic wszedzie int, a potem odpowiednio skalowac dla wygody?
        ImGui::InputFloat("Position Y", &positionY);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 1.0f); 
        ImGui::SliderFloat("Frequency", &frequency, 0.0f, 1.0f); 
      
        if (ImGui::Button("Add"))
        {
            simulationPlane.AddWaveSource(sf::Vector2f(positionX, positionY), amplitude, frequency);
            
            count++;
            
            positionX = 0.0f;
            positionY = 0.0f;
            amplitude = 0.1f;
            frequency = 0.5f;
            
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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

void Rotate(SimulationPlane& simulationPlane, sf::Event& event, bool &isDraggin,sf::Vector2i &mouse_pos, bool& canDrag) {
    //update rotation
    if (isDraggin && canDrag) {
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

void CopyToClipboard(const sf::Texture& texture) {
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

