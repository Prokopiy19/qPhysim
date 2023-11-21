#ifndef WINDOW_H
#define WINDOW_H

#include <QString>
#include <QWidget>

#include "render_area.h"

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QFrame;
class QPushButton;
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

private slots:
    void change_shape();
    void set_dynamic(bool flag);
    void reset_world();
private:
    QWidget* add_render_view(QWidget *parent);
    QWidget* add_control_panel(QWidget *parent);
    QWidget* add_click_group(QWidget *parent);
    QWidget* add_click_action(const QString& s, RenderArea::ClickAction action, QWidget *parent);
    QWidget* add_spawn_options(QWidget *parent);
    QWidget* add_shape_option(QWidget *parent);
    QWidget* add_dynamic_option(QWidget *parent);
    QWidget* add_size_slider(QWidget *parent);
    QWidget* add_reset_button(QWidget *parent);

    RenderArea *render_area;
    QButtonGroup *button_group;
    QPushButton *shape_button;
};


#endif // WINDOW_H
