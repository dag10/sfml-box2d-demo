#include <Environment.h>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include <iostream>
using namespace std;

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
sf::Vector2f viewCenter = {0, 0};
sf::View view;

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
}

/*
 * Step the physics simulation
 */
void Environment::Step(float frameTime) {
    const int velocityIterations = 8; // How strongly to correct velocity
    const int positionIterations = 4; // How strongly to correct position

    world->Step(frameTime, velocityIterations, positionIterations);
}

/*
 * Render to an SFML RenderTarget
 */
void Environment::Render(sf::RenderTarget &target, int renderWidth, int renderHeight) {
    /* Set View
        Note: The RenderTarget only uses the default view. This view is for screen<->world translations. */
    view.SetFromRect(sf::FloatRect(0, 0, (int)(((float)renderWidth) / zoomFactor / pixelsPerMeter),
                                   -(int)(((float)renderHeight) / zoomFactor / pixelsPerMeter))); // Invert Y axis
    view.SetCenter(viewCenter.x, viewCenter.y);

    /* Render objects */
    for (shared_ptr<PhysicsObject> &obj : objects) {
        b2Vec2 pos = WorldToScreenPosition(obj->body->GetPosition());
        obj->graphic->SetScale(pixelsPerMeter * zoomFactor, pixelsPerMeter * zoomFactor);
        obj->graphic->SetPosition(pos.x, pos.y);
        obj->graphic->SetRotation(rad2deg(obj->body->GetAngle())); // Converted from rad to deg
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

    boxShape.SetAsBox(width / 2, height / 2); // Parameters require half-width and half-height
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
b2Vec2 Environment::ScreenToWorldPosition(b2Vec2 vec) {
    vec.x /= pixelsPerMeter;
    vec.y /= -pixelsPerMeter;

    vec.x -= (view.GetCenter().x + view.GetHalfSize().x);
    vec.y -= (-view.GetCenter().y + view.GetHalfSize().y);

    return vec;
}

/*
 * Translate world coordinates into screen coordinates
 */
b2Vec2 Environment::WorldToScreenPosition(b2Vec2 vec) {
    vec.x += (view.GetCenter().x + view.GetHalfSize().x);
    vec.y += (-view.GetCenter().y + view.GetHalfSize().y);

    vec.x *= pixelsPerMeter;
    vec.y *= -pixelsPerMeter;

    return vec;
}

/*
 * Translate screen dimensions into world dimensions
 */
b2Vec2 Environment::ScreenToWorldSize(b2Vec2 vec) {
    vec.x /= pixelsPerMeter;
    vec.y /= pixelsPerMeter;

    return vec;
}

/*
 * Translate world dimensions into screen dimensions
 */
b2Vec2 Environment::WorldToScreenSize(b2Vec2 vec) {
    vec.x *= pixelsPerMeter;
    vec.y *= pixelsPerMeter;

    return vec;
}

/* Getters and Setters */

shared_ptr<b2World> Environment::GetWorld() {
    return world;
}
