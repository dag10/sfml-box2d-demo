/*
 * SFML Box2D Integration Test
 * Copyright (c) 2011 Drew Gottlieb
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

/* Globals */

unique_ptr<sf::RenderWindow> window;
unique_ptr<sf::Input> input;
unique_ptr<Environment> env;
shared_ptr<PhysicsObject> selectedObject = nullptr;

/* Settings */

const bool fullscreen = false;
const string windowTitle = "SFML/Box2D Test";

/*
 * Main - program entrypoint
 */
int main() {
	if (init())
		mainLoop();

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
        window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(800, 600, 32), windowTitle));

	/* Input */
	input = unique_ptr<sf::Input>((sf::Input*)&window->GetInput());

	/* Environment */
    env = unique_ptr<Environment>(new Environment());

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

	/* Display */
	window->Display();
}

/*
 * Main per-loop logic
 */
void logic() {
    /* Step simulation */
	env->Step(window->GetFrameTime());
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
			if (event->Key.Code == sf::Key::Escape)
				window->Close();
			break;
        case sf::Event::MouseButtonPressed:
            if (event->MouseButton.Button == sf::Mouse::Button::Left)
                selectObject();
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

    b2Vec2 worldCoords = env->ScreenToWorld(b2Vec2(input->GetMouseX(), input->GetMouseY()));
}
