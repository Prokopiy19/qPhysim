#ifndef SIMULATION_H
#define SIMULATION_H

#include <box2d/box2d.h>
#include <box2d/b2_settings.h>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

class Simulation {
public:
    #define X_SHAPES \
        X(square, "Square") \
        X(circle, "Circle") \
        X(triangle, "Triangle")

    enum class Shape {
        #define X(a, b) a,
            X_SHAPES
        #undef X
    } shape = Shape::square;

    #define X(a, b) + 1
    static constexpr int shape_count = X_SHAPES;
    #undef X

    static constexpr float time_step = 1.0f / 60;

    Shape change_shape();

    void set_dynamic(bool flag) { body_def.type = flag ? b2_dynamicBody : b2_staticBody; }
    void set_size_slider(const QSlider *slider) { size_slider = slider; }

    void reset();

    Simulation();
    void update();
    void destroy_far_bodies();

    const b2Body *get_body_list() const { return world.GetBodyList(); }
    int body_count() const { return world.GetBodyCount(); }

    constexpr float width() const { return 16; }
    constexpr float height() const { return 16; }

    void spawn_body(float x, float y);
    void destroy_body(float x, float y);
    void explosion(float x, float y);
private:
    void world_init();

    b2BodyDef body_def;
    float size = 1;
    QElapsedTimer timer;
    const QSlider *size_slider;
    float t = 0;
    b2World world;
};

#endif // SIMULATION_H
