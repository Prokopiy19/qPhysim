#include "simulation.h"

#include <QSlider>

#include <cmath>
#include <numbers>

Simulation::Simulation() : world({0, 10})
{
    world_init();
    timer.start();
}

void Simulation::world_init()
{
    b2ChainShape chain;
    b2Vec2 vertices[4] = {
                          { 0, 0 },
                          { 0, height() },
                          { width(), height() },
                          { width(), 0 },
                          };
    b2Vec2 prev = { -width(), 0 };
    b2Vec2 next = { 2 * width(), 0 };
    chain.CreateChain(vertices, 4, prev, next);

    b2BodyDef walls;
    auto body = world.CreateBody(&walls);
    b2FixtureDef fixture_def;
    fixture_def.shape = &chain;
    fixture_def.restitution = 0.2;
    fixture_def.friction = 0.3;
    body->CreateFixture(&fixture_def);
}

void Simulation::update()
{
    constexpr float nano = 1e-9;
    t += timer.nsecsElapsed() * nano;
    timer.start();
    for ( ; t >= 0; t -= time_step) {
        constexpr int32 velocity_iterations = 8;
        constexpr int32 position_iterations = 3;
        world.Step(time_step, velocity_iterations, position_iterations);
    }
    destroy_far_bodies();
}

void Simulation::destroy_far_bodies()
{
    for (auto body = world.GetBodyList(); body; ) {
        auto next = body->GetNext();
        if (body->GetPosition().y > 2 * height()) {
            world.DestroyBody(body);
        }
        body = next;
    }
}

void Simulation::spawn_body(float x, float y)
{
    body_def.position = { x, y };
    auto body = world.CreateBody(&body_def);
    b2FixtureDef fixture_def;
    fixture_def.density = 1;
    fixture_def.friction = 0.3;
    fixture_def.restitution = 0.2;
    const float sz = 0.01 * size_slider->value();

    b2PolygonShape polygon;
    b2CircleShape circle;
    switch(shape) {
        case Shape::square: {
            polygon.SetAsBox(sz, sz);
            fixture_def.shape = &polygon;
            break;
        }
        case Shape::circle: {
            circle.m_radius = sz;
            fixture_def.shape = &circle;
            break;
        }
        case Shape::triangle: {
            constexpr float pi = std::numbers::pi_v<float>;
            const float y0 = -sz / std::sin(pi / 3);
            const float y1 = sz / std::tan(pi / 3);
            const b2Vec2 points[3] = {
                { 0, y0 },
                { sz, y1 },
                { -sz, y1 },
            };
            polygon.Set(points, 3);
            fixture_def.shape = &polygon;
        }
    }
    body->CreateFixture(&fixture_def);
}

namespace {

struct Destroyer : public b2QueryCallback {
    Destroyer(b2World *world_, float x_, float y_) : world(world_), x(x_), y(y_) {}
    virtual bool ReportFixture(b2Fixture *fixture) override;
    b2World *world;
    const float x, y;
};
bool Destroyer::ReportFixture(b2Fixture *fixture)
{
    if (fixture->TestPoint({ x, y })) {
        world->DestroyBody(fixture->GetBody());
        return false;
    }
    return true;
}

struct Exploder : public b2QueryCallback {
    Exploder(b2World *world_, float x, float y, float r_) : world(world_), point(x, y), r(r_) {}
    virtual bool ReportFixture(b2Fixture *fixture) override;
    b2World *world;
    const b2Vec2 point;
    const float r;
};
bool Exploder::ReportFixture(b2Fixture *fixture)
{
    b2MassData data;
    auto transform = fixture->GetBody()->GetTransform();
    fixture->GetMassData(&data);
    auto center = b2Mul(transform, data.center);

    b2Vec2 impulse_vector = center - point;
    if (impulse_vector.Length() < b2_epsilon) {
        return true;
    }
    float dist = impulse_vector.Normalize();
    if (dist <= r) {
        constexpr float impulse = 7;
        constexpr bool wake = true;
        fixture->GetBody()->ApplyLinearImpulse(impulse * (r - dist) * impulse_vector, center, wake);
    }
    return true;
}

} // namespace

void Simulation::destroy_body(float x, float y)
{
    b2AABB aabb;
    constexpr float a = 1;
    aabb.lowerBound = { x - a, y - a };
    aabb.upperBound = { x + a, y + a };
    Destroyer destroyer(&world, x, y);
    world.QueryAABB(&destroyer, aabb);
}

void Simulation::explosion(float x, float y)
{
    constexpr float r = 3;
    b2AABB aabb;
    aabb.lowerBound = { x - r, y - r };
    aabb.upperBound = { x + r, y + r };
    Exploder exploder(&world, x, y, r);
    world.QueryAABB(&exploder, aabb);
}

Simulation::Shape Simulation::change_shape()
{
    int tmp = static_cast<int>(shape) + 1;
    tmp = (tmp == shape_count) ? 0 : tmp;
    shape = static_cast<Shape>(tmp);
    return shape;
}

void Simulation::reset()
{
    for (auto body = world.GetBodyList(); body; ) {
        auto next = body->GetNext();
        world.DestroyBody(body);
        body = next;
    }
    world_init();
}
