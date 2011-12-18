/*
 * SFML Box2D Integration Test
 * Copyright (c) 2011 Drew Gottlieb
 */

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <string>

using namespace std;

/* Functions */

bool init();
void mainLoop();
void render();
void logic();
void processEvent(sf::Event *event);

/* Globals */

unique_ptr<sf::RenderWindow> window;
unique_ptr<sf::Input> input;

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
	// Init window
    if (fullscreen)
        window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode::GetDesktopMode(), windowTitle, sf::Style::Fullscreen));
    else
        window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(800, 600, 32), windowTitle));

	// Initialize everything else

	input = unique_ptr<sf::Input>((sf::Input*)&window->GetInput());

	return true;
}

/*
 * Runs the main program loop
 */
void mainLoop() {
	while(window->IsOpened()) {
		// Logic
		logic();

		// Handle window events and updating
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
	// Clear
	window->Clear(sf::Color(100, 149, 237));

	// TODO: Render objects

	// Display
	window->Display();
}

/*
 * Main per-loop logic
 */
void logic() {
	// TODO: Physics logic
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
	}
}
