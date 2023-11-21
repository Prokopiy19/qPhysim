#ifndef RENDER_AREA_H
#define RENDER_AREA_H

#include <numbers>

#include <QMouseEvent>
#include <QWidget>

#include "simulation.h"

QT_BEGIN_NAMESPACE
class QButtonGroup;
QT_END_NAMESPACE


class RenderArea : public QWidget
{
    Q_OBJECT

public:
    enum class ClickAction {
        spawn, destroy, explosion
    };

    explicit RenderArea(QWidget *parent = nullptr);

    void set_button_group(const QButtonGroup *group) { button_group = group; }

    QSize sizeHint() const override;

    Simulation simulation;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void draw_edge(QPainter& painter, const b2EdgeShape& edge);

    float screen_x(float x) const {
        return scale_x(x - view.x);
    };
    float screen_y(float y) const {
        return scale_y(y - view.y);
    };
    float scale_x(float x) const {
        return width() * x / view.w;
    }
    float scale_y(float y) const {
        return height() * y / view.h;
    }
    float game_x(float x) const {
        return view.x + x * view.w / width();
    }
    float game_y(float y) const {
        return view.y + y * view.h / height();
    }
    static float to_degrees(float a) {
        return a * 180.0f / std::numbers::pi;
    }

    struct {
        float x, y, w, h;
    } view;
    void adjust_view();

    const QButtonGroup *button_group;
};


#endif // RENDER_AREA_H
