#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QReadWriteLock>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QImage>
#include <QSharedMemory>
#include <QRect>
#include <QDebug>

#define ELEMENT_HEIGHT_NUM 25
#define ELEMENT_WIDTH_NUM 25
#define ELEMENT_EDGE_LENGTH 32

enum nodeType {
    Empty = 0,
    MyPlaneT,
    EvilPlaneT,
    MyBulletT,
    EvilBulletT,
    MultiBulletT
};

enum statusType {
    NotStart = 0,
    Running,
    Victory,
    Failure
};
Q_DECLARE_METATYPE(statusType);

struct node {
    int x, y;
};

class Plane: public QThread {
    Q_OBJECT
    statusType m_status;
protected:
    QSharedMemory *mem;
    nodeType (*memdata)[ELEMENT_WIDTH_NUM];
    QList<node> bullets;
    nodeType myType;
    node position;
    int ifEmit = 0;
public:
    Plane(int x, int y, nodeType t);
    ~Plane() {
        delete mem;
    }
signals:
    void statusChanged(statusType status);
public slots:
    void setStatus(statusType newStatus) {
        if (newStatus != m_status) {
            m_status = newStatus;
            emit statusChanged(newStatus);
        }
    }
};

class MyPlane: public Plane {
    using Plane::Plane;
public:
    virtual void run();
    void handleKey(int key);
};

class EvilPlane: public Plane {
    using Plane::Plane;
public:
    virtual void run();
};

class Widget : public QWidget {
    Q_OBJECT

    QTimer *m_clock_intr_device;

    QSharedMemory *mem;
    nodeType (*memdata)[ELEMENT_WIDTH_NUM];
    QImage *myPlaneImage, *evilPlaneImage;

    QPainter painter;

    MyPlane *myp;
    EvilPlane *evilp;

    statusType m_status;

    void ClockIntrExec();
    void handleStatus(statusType status);

    virtual void paintEvent(QPaintEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
};
#endif // WIDGET_H
