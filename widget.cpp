#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi();
}

void Widget::setupUi() {
    this->setWindowTitle("\u2708大战");
    this->setFixedSize(1280, 400);

    m_draw_intr_device = new QTimer(this);
    m_clock_intr_device = new QTimer(this);

    connect(m_draw_intr_device, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    connect(m_clock_intr_device, &QTimer::timeout, this, &Widget::ClockIntrExec);

    m_clock_intr_device->start();
}

void Widget::keyPressEvent(QKeyEvent *e){

}

void paintEvent(QPaintEvent *e) {

    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 80; ++j) {
            if (a[i][j] == 1) {

            }
        }
    }
}

void Widget::ClockIntrExec() {

}
