#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("飞机大战");
    this->setFixedSize(400, 400);

    m_draw_intr_device = new QTimer(this);
    m_clock_intr_device = new QTimer(this);

    connect(m_draw_intr_device, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    connect(m_clock_intr_device, &QTimer::timeout, this, &Widget::ClockIntrExec);

    m_draw_intr_device->start(40);
    m_clock_intr_device->start(400);
}

Widget::~Widget() {
    delete m_draw_intr_device;
    delete m_clock_intr_device;
}

void Widget::keyPressEvent(QKeyEvent *e){

}

void Widget::paintEvent(QPaintEvent *e) {
    painter.begin(this);
    painter.setBrush(Qt::SolidPattern);
    memLock.lockForRead();
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            if (mem[i][j] == 1) {
                painter.drawText(i * 16, j * 16, "\u2708");
            }
        }
    }
    memLock.unlock();
    painter.end();
}

void Widget::ClockIntrExec() {
    memLock.lockForWrite();
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            mem[i][j] = random.bounded(2);
        }
    }
    memLock.unlock();
}
