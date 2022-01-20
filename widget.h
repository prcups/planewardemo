#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QPainter>

class Clock_intr: public QTimer {
    Q_OBJECT

};

class Draw_intr: public QPainter {

};

class Key_intr: public QWidget {


};

class Widget : public QWidget
{
    Q_OBJECT

    int pos [25][80];
    QLabel *myplane, *yourplane;
    QThread myp_thread, yourp_thread;
    Clock_intr m_clock_intr;
    Draw_intr m_draw_intr;
    Key_intr m_key_intr;
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void setupUi();
    void initFunc();
    void keyIntr();
    void clockIntr();
};
#endif // WIDGET_H
