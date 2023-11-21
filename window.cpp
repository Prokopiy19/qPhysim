#include "render_area.h"
#include "window.h"

#include <QtWidgets>

Window::Window()
{
    auto *mainLayout = new QHBoxLayout(this);

    setLayout(mainLayout);

    mainLayout->addWidget(add_render_view(this));
    mainLayout->addWidget(add_control_panel(this));

    setWindowTitle(tr("qPhysim"));
}

void Window::change_shape()
{
    auto shape = render_area->simulation.change_shape();
    switch (shape) {
        #define X(a, b) case Simulation::Shape::a: shape_button->setText(tr(b)); break;
            X_SHAPES
        #undef X
    }
}

void Window::set_dynamic(bool flag)
{
    render_area->simulation.set_dynamic(flag);
}

void Window::reset_world()
{
    render_area->simulation.reset();
}

QWidget* Window::add_render_view(QWidget *parent)
{
    auto frame = new QFrame(parent);
    frame->setFrameStyle(QFrame::Panel | QFrame::Plain);
    frame->setMinimumSize(320, 320);
    frame->setLayout(new QHBoxLayout(frame));

    render_area = new RenderArea(frame);

    frame->layout()->addWidget(render_area);
    frame->layout()->setContentsMargins(0, 0, 0, 0);

    return frame;
}

QWidget* Window::add_control_panel(QWidget *parent)
{
    auto control_panel = new QWidget(parent);
    auto *control_panel_layout = new QVBoxLayout(control_panel);
    control_panel->setLayout(control_panel_layout);
    control_panel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    control_panel_layout->addWidget(add_click_group(control_panel));
    control_panel_layout->addWidget(add_spawn_options(control_panel));
    control_panel_layout->addWidget(add_reset_button(control_panel));
    control_panel_layout->addStretch();

    return control_panel;
}

QWidget* Window::add_click_group(QWidget *parent)
{
    auto click_group = new QGroupBox(tr("Click action"), parent);
    click_group->setLayout(new QVBoxLayout(click_group));
    button_group = new QButtonGroup(click_group);

    add_click_action(tr("Spawn body"), RenderArea::ClickAction::spawn, click_group);
    add_click_action(tr("Destroy body"), RenderArea::ClickAction::destroy, click_group);
    add_click_action(tr("Explosion"), RenderArea::ClickAction::explosion, click_group);

    button_group->button(0)->click();

    render_area->set_button_group(button_group);

    return click_group;
}

QWidget* Window::add_click_action(const QString& s, RenderArea::ClickAction action, QWidget *parent)
{
    auto radio_button = new QRadioButton(s, parent);
    button_group->addButton(radio_button, static_cast<int>(action));
    parent->layout()->addWidget(radio_button);
    return radio_button;
}

QWidget* Window::add_spawn_options(QWidget *parent)
{
    auto spawn_group = new QGroupBox(tr("Spawn options"), parent);
    spawn_group->setLayout(new QVBoxLayout(spawn_group));

    spawn_group->layout()->addWidget(add_shape_option(spawn_group));
    spawn_group->layout()->addWidget(add_dynamic_option(spawn_group));
    spawn_group->layout()->addWidget(add_size_slider(spawn_group));

    constexpr int id = static_cast<int>(RenderArea::ClickAction::spawn);
    connect(button_group->button(id), &QAbstractButton::toggled, this,
            [spawn_group](bool on) { spawn_group->setDisabled(!on); });

    return spawn_group;
}

QWidget* Window::add_shape_option(QWidget *parent)
{
    auto shape_widget = new QWidget(parent);
    shape_widget->setLayout(new QHBoxLayout);

    auto label = new QLabel(tr("Shape:"), shape_widget);
    shape_button = new QPushButton(tr("Square"), shape_widget);
    label->setBuddy(shape_button);

    shape_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    shape_widget->layout()->addWidget(label);
    shape_widget->layout()->addWidget(shape_button);

    shape_widget->layout()->setContentsMargins(0, 0, 0, 0);

    connect(shape_button, &QAbstractButton::pressed, this, &Window::change_shape);

    return shape_widget;
}

QWidget* Window::add_dynamic_option(QWidget *parent)
{
    auto check_box = new QCheckBox(tr("Dynamic body"), parent);

    connect(check_box, &QAbstractButton::clicked, this, &Window::set_dynamic);

    check_box->click();

    return check_box;
}

QWidget* Window::add_size_slider(QWidget *parent)
{
    auto size_widget = new QWidget(parent);
    size_widget->setLayout(new QHBoxLayout(size_widget));

    auto label = new QLabel(tr("Size:"), size_widget);
    auto size_slider = new QSlider(Qt::Horizontal, size_widget);

    size_widget->layout()->addWidget(label);
    size_widget->layout()->addWidget(size_slider);

    size_widget->layout()->setContentsMargins(0, 0, 0, 0);

    size_slider->setMinimum(25);
    size_slider->setMaximum(100);
    size_slider->setValue(50);

    render_area->simulation.set_size_slider(size_slider);

    return size_widget;
}

QWidget* Window::add_reset_button(QWidget *parent)
{
    auto button = new QPushButton(tr("Reset"), parent);

    connect(button, &QAbstractButton::pressed, this, &Window::reset_world);

    return button;
}
