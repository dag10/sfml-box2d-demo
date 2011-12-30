/*
Environment.h
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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <memory>
#include <vector>

using namespace std;

class b2World;
class b2Body;
struct b2Vec2;

namespace sf {
    class RenderTarget;
    class Drawable;
    class View;
}

typedef struct {
    shared_ptr<sf::Drawable> graphic;
    shared_ptr<b2Body> body;
} PhysicsObject;

class Environment {
    public:
        Environment();
        ~Environment() {};

        void Step(float frameTime);
        void Render(sf::RenderTarget &target, int renderWidth, int renderHeight);

        shared_ptr<PhysicsObject> CreateBox(float width,
                                            float height,
                                            float x,
                                            float y,
                                            bool dynamic);

        shared_ptr<b2World> GetWorld();

        static b2Vec2 ScreenToWorldPosition(b2Vec2 vec);
        static b2Vec2 WorldToScreenPosition(b2Vec2 vec);

        static b2Vec2 ScreenToWorldSize(b2Vec2 vec);
        static b2Vec2 WorldToScreenSize(b2Vec2 vec);

    protected:
        shared_ptr<b2World> world;
        vector<shared_ptr<PhysicsObject>> objects;
};

#endif // ENVIRONMENT_H
