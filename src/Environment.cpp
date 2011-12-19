#include <Environment.h>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

/* Body Definitions */
b2BodyDef blockDef;

/* Shape Definitions */
b2PolygonShape boxShape;

/* Fixture Definitions */
b2FixtureDef boxFixture;

/* Settings */
const float pixelsPerMeter = 50;
const float zoomFactor = 1;

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
    CreateBox(1, 1, 1, 5, true);
    CreateBox(5, 1, 0, 0, false);
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
    view.SetFromRect(sf::FloatRect(0, 0, (int)((float)renderWidth / zoomFactor / pixelsPerMeter), -(int)((float)renderHeight / zoomFactor / pixelsPerMeter)));
    view.SetCenter(2.5, 2);
    target.SetView(view);

    /* Render objects */
    for (shared_ptr<PhysicsObject> &obj : objects) {
        obj->graphic->SetPosition(obj->body->GetPosition().x, obj->body->GetPosition().y );
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

    const auto outlineColor = sf::Color(0, 0, 0);
    auto fillColor = dynamic ? sf::Color(0, 0, 100, 100) : sf::Color(0, 100, 0, 100);
    const float borderThickness = 1; // In pixels

    // Counteracts strange glitch where, when border thickness is
    // negative (inside the shape), the shape's size shrinks by the
    // border thickness as well, leaving gaps between objects.
    float pixelWidth = width + (borderThickness / pixelsPerMeter);
    float pixelHeight = height + (borderThickness / pixelsPerMeter);

    auto shape = new sf::Shape();
    shape->AddPoint(0, 0, fillColor, outlineColor);
    shape->AddPoint(pixelWidth, 0, fillColor, outlineColor);
    shape->AddPoint(pixelWidth, pixelHeight, fillColor, outlineColor);
    shape->AddPoint(0, pixelHeight, fillColor, outlineColor);
    shape->SetOutlineWidth(-borderThickness / pixelsPerMeter);
    shape->EnableFill(true);
    shape->EnableOutline(true);

    boxShape.SetAsBox(width / 2, height / 2); // Parameters are half-width and half-height
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

/* Getters and Setters */

shared_ptr<b2World> Environment::GetWorld() {
    return world;
}
