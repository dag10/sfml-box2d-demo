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
