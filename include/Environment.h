#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <memory>
#include <vector>

using namespace std;

class b2World;
class b2Body;

namespace sf {
    class RenderTarget;
    class Drawable;
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

    protected:
        shared_ptr<b2World> world;
        vector<shared_ptr<PhysicsObject>> objects;
};

#endif // ENVIRONMENT_H
