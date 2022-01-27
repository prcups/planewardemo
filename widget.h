#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRandomGenerator>

class Widget : public QWidget {
    Q_OBJECT

    QLabel *myplane, *yourplane;
    QThread myp_thread, yourp_thread;
    QTimer *m_clock_intr_device, *m_draw_intr_device;
    QRandomGenerator random;
    int mem[25][80];
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void paintEvent(QPaintEvent *e);

    void setupUi();
public:
    Widget(QWidget *parent = nullptr);
    void ClockIntrExec();
    void KeyIntrExec();
    ~Widget();
};
#endif // WIDGET_H
