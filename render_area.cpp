#include "render_area.h"
#include "qpainterpath.h"

#include <QButtonGroup>
#include <QPainter>
#include <QTimer>

QSize RenderArea::sizeHint() const
{
    return QSize(480, 480);
}

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    adjust_view();
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, QOverload<>::of(&RenderArea::update));

    timer->start(simulation.time_step * 1000);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RenderArea::paintEvent(QPaintEvent *)
{
    adjust_view();
    simulation.update();
    QPainter painter(this);

    painter.setBrush(Qt::BrushStyle::SolidPattern);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    for (auto body = simulation.get_body_list(); body; body = body->GetNext()) {
        painter.save();
        auto pos = body->GetPosition();
        if (body->GetType() == b2_dynamicBody) {
            painter.setBrush(Qt::green);
        }
        else {
            painter.setBrush(Qt::gray);
        }
        painter.translate(screen_x(pos.x), screen_y(pos.y));
        painter.rotate(to_degrees(body->GetAngle()));

        for (auto fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            auto shape = fixture->GetShape();
            switch (shape->m_type) {

                case b2Shape::e_polygon: {
                    auto polygon = static_cast<const b2PolygonShape *>(shape);
                    QPoint points[b2_maxPolygonVertices];
                    for (int i = 0; i < polygon->m_count; ++i) {
                        points[i].setX(scale_x(polygon->m_vertices[i].x));
                        points[i].setY(scale_y(polygon->m_vertices[i].y));
                    }
                    painter.drawPolygon(points, polygon->m_count);
                    break;
                }

                case b2Shape::e_circle: {
                    auto circle = static_cast<const b2CircleShape *>(shape);
                    QPointF point(scale_x(circle->m_p.x), scale_y(circle->m_p.y));
                    painter.drawEllipse(point, scale_x(circle->m_radius), scale_y(circle->m_radius));
                    break;
                }

                case b2Shape::e_chain: {
                    auto chain = static_cast<const b2ChainShape *>(shape);
                    const auto n = chain->GetChildCount();
                    for (int32 i = 0; i < n; ++i) {
                        b2EdgeShape edge;
                        chain->GetChildEdge(&edge, i);
                        draw_edge(painter, edge);
                    }
                    break;
                }

                case b2Shape::e_edge: {
                    auto edge = static_cast<const b2EdgeShape *>(shape);
                    draw_edge(painter, *edge);
                    break;
                }

                default: break;
            }
        }
        painter.restore();
    }
    QString s;
    QTextStream out(&s, QIODevice::WriteOnly);
    out << "Body count: " << simulation.body_count();
    painter.drawText(5, 0, width(), 15, Qt::AlignLeft, s);
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        const float x = game_x(event->position().x());
        const float y = game_y(event->position().y());
        switch(static_cast<ClickAction>(button_group->checkedId())) {
            case ClickAction::spawn: {
                simulation.spawn_body(x, y);
                break;
            }
            case ClickAction::destroy: {
                simulation.destroy_body(x, y);
                break;
            }
            case ClickAction::explosion: {
                simulation.explosion(x, y);
                break;
            }
        }
    }
}

void RenderArea::draw_edge(QPainter& painter, const b2EdgeShape& edge)
{
    painter.drawLine(
        scale_x(edge.m_vertex1.x), scale_y(edge.m_vertex1.y),
        scale_x(edge.m_vertex2.x), scale_y(edge.m_vertex2.y)
    );
}

void RenderArea::adjust_view()
{
    if (width() * simulation.height() > simulation.width() * height()) {
        view.w = simulation.height() * width() / height();
        view.h = simulation.height();
        view.x = (simulation.width() - view.w) * 0.5;
        view.y = 0;
    }
    else {
        view.w = simulation.width();
        view.h = simulation.width() * height() / width();
        view.x = 0;
        view.y = simulation.height() - view.h;
    }
}
