/*
main.cpp
SFML Box2D Integration Test
Copyright (c) 2011 Drew Gottlieb

This file is part of SFML-Box2D-Test.

SFML-Box2D-Test is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SFML-Box2D-Test is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SFML-Box2D-Test.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include <include/Environment.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/* Functions */

bool init();
void mainLoop();
void render();
void logic();
void processEvent(sf::Event *event);
void selectObject();
void startDrag();
void stopDrag();
void toggleCursor();
void cleanup();

/* Globals */

shared_ptr<sf::RenderWindow> window;
shared_ptr<sf::Input> input;
shared_ptr<Environment> env;
shared_ptr<sf::Shape> previewRect = nullptr;
shared_ptr<PhysicsObject> selectedObject = nullptr;
bool isDragging = false;
bool showingCursor = true;

/* Settings */

const bool fullscreen = false;
const string windowTitle = "SFML/Box2D Test";
const float borderSize = 30;

/*
 * Main - program entrypoint
 */
int main() {
	if (init())
		mainLoop();
    cleanup();

	return EXIT_SUCCESS;
}

/*
 * Init - Loads resources and initializes logic
 */
bool init() {
	/* Window */
    if (fullscreen)
        window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode::GetDesktopMode(), windowTitle, sf::Style::Fullscreen));
    else
        window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(800, 600, 32), windowTitle, sf::Style::Close));

    /* Show/Hide Cursor */
    window->ShowMouseCursor(showingCursor);

	/* Input */
	input = shared_ptr<sf::Input>((sf::Input*)&window->GetInput());

	/* Environment */
    env = shared_ptr<Environment>(new Environment());

    /* Initial render */
    render();

    /* Create static ground */
    b2Vec2 size = env->ScreenToWorldSize(b2Vec2(window->GetWidth(), borderSize));
    b2Vec2 pos = env->ScreenToWorldPosition(b2Vec2(0, window->GetHeight() - borderSize));
    pos.x += size.x / 2;
    pos.y -= size.y / 2;
    env->CreateBox(size.x, size.y, pos.x, pos.y, false);

	return true;
}

/*
 * Runs the main program loop
 */
void mainLoop() {
	while(window->IsOpened()) {
		/* Logic */
		logic();

		/* Handle window events and updating */
		sf::Event event;
		while (window->GetEvent(event))
			processEvent(&event);
		render();
	}
}

/*
 * Renders the current frame
 */
void render() {
	/* Clear */
	window->Clear(sf::Color(100, 149, 237));

	/* Render environment */
	env->Render(*window, window->GetWidth(), window->GetHeight());

    /* Reset view in case it's been changed */
    window->SetView(window->GetDefaultView());

	/* Render preview rectangle */
	if (previewRect)
        window->Draw(*previewRect);

	/* Display */
	window->Display();
}

/*
 * Main per-loop logic
 */
void logic() {
    /* Get mouse position */
	auto xMouse = input->GetMouseX(), yMouse = input->GetMouseY();

    /* Step simulation */
	env->Step(window->GetFrameTime());

	/* Update preview rectangle to mouse position */
	if (isDragging) {
		sf::Vector2f vec;
		vec = previewRect->GetPointPosition(1);
		previewRect->SetPointPosition(1, vec.x, yMouse);
		previewRect->SetPointPosition(2, xMouse, yMouse);
		vec = previewRect->GetPointPosition(3);
		previewRect->SetPointPosition(3, xMouse, vec.y);
	}
}

/*
 * Processes Window events
 */
void processEvent(sf::Event *event) {
	switch (event->Type) {
		case sf::Event::Closed:
			window->Close();
			break;
		case sf::Event::KeyPressed:
            switch (event->Key.Code) {
                case sf::Key::Escape:
                    window->Close();
                case sf::Key::Space:
                    toggleCursor();
            }
			break;
        case sf::Event::MouseButtonPressed:
            if (event->MouseButton.Button == sf::Mouse::Button::Left)
                startDrag();
                // selectObject();
            break;
        case sf::Event::MouseButtonReleased:
            stopDrag();
            break;
	}
}

/*
 * Selects an object at the cursor position
 */
void selectObject() {
    if (selectedObject != nullptr) {
        selectedObject->graphic->SetColor(sf::Color::White);
        selectedObject = nullptr;
    }

    b2Vec2 worldCoords = env->ScreenToWorldPosition(b2Vec2(input->GetMouseX(), input->GetMouseY()));
}

/*
 * Starts dragging out a rectangle
 */
void startDrag() {
    if (isDragging) stopDrag();

    auto xMouse = input->GetMouseX(), yMouse = input->GetMouseY();

    const auto outlineColor = sf::Color::Black;
    const auto fillColor = sf::Color::White;

    sf::Shape *shape = new sf::Shape();
    shape->AddPoint(xMouse, yMouse, fillColor, outlineColor);
    shape->AddPoint(xMouse, yMouse + 1, fillColor, outlineColor);
    shape->AddPoint(xMouse + 1, yMouse + 1, fillColor, outlineColor);
    shape->AddPoint(xMouse + 1, yMouse, fillColor, outlineColor);
    shape->SetOutlineWidth(1);
    shape->EnableFill(false);
    shape->EnableOutline(true);

    previewRect = shared_ptr<sf::Shape>(shape);
    isDragging = true;
}

/*
 * Stops dragging out a rectangle
 */
void stopDrag() {
    if (!isDragging || !previewRect) return;

    sf::Vector2f topLeft = previewRect->GetPointPosition(0);
    sf::Vector2f bottomRight = previewRect->GetPointPosition(2);

    if (abs(bottomRight.x - topLeft.x) <= 1 || abs(bottomRight.y - topLeft.y) <= 1) {
        cout << "Object too small!" << endl;
        previewRect = nullptr;
        isDragging = false;
        return;
    }

    b2Vec2 topLeftWorld = env->ScreenToWorldPosition(b2Vec2(topLeft.x, topLeft.y));
    b2Vec2 bottomRightWorld = env->ScreenToWorldPosition(b2Vec2(bottomRight.x, bottomRight.y));

    float boxX = (topLeftWorld.x + bottomRightWorld.x) / 2;
    float boxY = (topLeftWorld.y + bottomRightWorld.y) / 2;
    float boxWidth = abs(bottomRightWorld.x - topLeftWorld.x);
    float boxHeight = abs(topLeftWorld.y - bottomRightWorld.y);

    cout << "Created BOX: (" << boxX << ", " << boxY << ") Size: " << boxWidth << " x " << boxHeight << endl;

    env->CreateBox(boxWidth, boxHeight, boxX, boxY, true);

    previewRect = nullptr;
    isDragging = false;
}

/*
 * Toggles showing the mouse cursor
 */
void toggleCursor() {
    window->ShowMouseCursor(showingCursor = !showingCursor);
}

/*
 * Cleans up neccesary objects
 */
void cleanup() {
    cout << "Cleaning up..." << endl;
}
