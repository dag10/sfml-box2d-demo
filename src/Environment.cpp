#include <Environment.h>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#define rad2deg(x) x*180/PI

/* Body Definitions */
b2BodyDef blockDef;

/* Shape Definitions */
b2PolygonShape boxShape;

/* Fixture Definitions */
b2FixtureDef boxFixture;

/* Settings and Constants */
const float pixelsPerMeter = 50;
const float zoomFactor = 1;
const double PI = 3.141592;

/* View parameters */
sf::Vector2f viewCenter = {2.5, 4};

/*
 * Constructor
 */
Environment::Environment() {
    /* Body Definitions */
    blockDef.type = b2_dynamicBody;
    blockDef.angle = 0;

    /* Shape Definitions */
    boxShape.SetAsBox(1, 1);   // 1x1 meters

    /* Fixture Definitions */
    boxFixture.shape = &boxShape;
    boxFixture.density = 1;

    /* World Creation */
    world = shared_ptr<b2World>(new b2World(b2Vec2(0, -9.8))); // Normal earth gravity (9.8 m/s/s)

    /* Object Creation */
    CreateBox(5, 1, 0, 0, false);
    CreateBox(2, 1, -0.2, 8, true);
    CreateBox(1, 1, 1, 5, true);
}

/*
 * Step the physics simulation
 */
void Environment::Step(float frameTime) {
    const int velocityIterations = 8; // How strongly to correct velocity
    const int positionIterations = 3; // How strongly to correct position

    world->Step(frameTime, velocityIterations, positionIterations);
}

/*
 * Render to an SFML RenderTarget
 */
void Environment::Render(sf::RenderTarget &target, int renderWidth, int renderHeight) {
    /* Set View */
    sf::View view;
    view.SetFromRect(sf::FloatRect(0, 0, (int)((float)renderWidth / zoomFactor / pixelsPerMeter),
                                   -(int)((float)renderHeight / zoomFactor / pixelsPerMeter))); // Invert Y axis
    viewCenter = objects.at(0)->graphic->GetPosition();
    view.SetCenter(viewCenter.x, viewCenter.y);
    target.SetView(view);

    /* Render objects */
    for (shared_ptr<PhysicsObject> &obj : objects) {
        obj->graphic->SetPosition(obj->body->GetPosition().x, obj->body->GetPosition().y);
        obj->graphic->SetRotation(rad2deg(-obj->body->GetAngle())); // Converted from rad to deg, and inverted
        target.Draw(*obj->graphic);
    }
}

/*
 * Shortcut for creating a PhysicsObject in the world
 */
shared_ptr<PhysicsObject> Environment::CreateBox(float width,
                                    float height,
                                    float x,
                                    float y,
                                    bool dynamic) {

    auto fillColor = dynamic ? sf::Color(0, 0, 100, 100) : sf::Color(0, 100, 0, 100);

    auto shape = new sf::Shape();
    shape->AddPoint(0, 0, fillColor);
    shape->AddPoint(width, 0, fillColor);
    shape->AddPoint(width, height, fillColor);
    shape->AddPoint(0, height, fillColor);
    shape->SetOutlineWidth(0);
    shape->EnableFill(true);
    shape->EnableOutline(false);
    shape->SetCenter(width / 2, height / 2);

    boxShape.SetAsBox(width / 2, height / 2, b2Vec2(0, 0), 0); // Parameters are half-width and half-height
    blockDef.type = ( dynamic ? b2_dynamicBody : b2_staticBody );
    blockDef.position.Set(x, y);
    auto body = shared_ptr<b2Body>(world->CreateBody(&blockDef),
                                   [this](b2Body *ptr) { world->DestroyBody(ptr); }); // Will be automatically destroyed by world
    body->CreateFixture(&boxFixture);

    auto obj = shared_ptr<PhysicsObject>(new PhysicsObject());
    obj->graphic = shared_ptr<sf::Drawable>(shape);
    obj->body = shared_ptr<b2Body>(body);
    objects.push_back(obj);

    return obj;
}

/*
 * Translate screen coordinates into world coordinates
 */
b2Vec2 Environment::ScreenToWorld(b2Vec2 vec) {
    vec.y /= -pixelsPerMeter;
    vec.x /= pixelsPerMeter;
}

/*
 * Translate world coordinates into screen coordinates
 */
b2Vec2 Environment::WorldToScreen(b2Vec2 vec) {
    vec.y *= -pixelsPerMeter;
    vec.x *= pixelsPerMeter;
}

/* Getters and Setters */

shared_ptr<b2World> Environment::GetWorld() {
    return world;
}
