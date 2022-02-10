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
    EvilBulletT
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
    Plane(int x, int y, nodeType t): m_status(NotStart), mem(new QSharedMemory("mem")), position({x, y}), myType(t) {
        mem->attach();
        memdata = (nodeType (*)[ELEMENT_WIDTH_NUM]) mem->data();
        mem->lock();
        memdata[position.x][position.y] = myType;
        mem->unlock();
    }
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
    virtual void run() {
        if ((++ifEmit) % 2) {
            bullets.insert(bullets.end(), {position.x, position.y});
        }
        for (QList<node>::iterator i = bullets.begin(); i != bullets.end();) {
            if (i->x) {
                --i->x;
                mem->lock();
                if (memdata[i->x + 1][i->y] == EvilBulletT) memdata[i->x + 1][i->y] = Empty;
                if (memdata[i->x][i->y] == EvilPlaneT) setStatus(Victory);
                else if (memdata[i->x][i->y == MyPlaneT]) setStatus(Failure);
                else memdata[i->x][i->y] = EvilBulletT;
                mem->unlock();
                ++i;
            } else {
                mem->lock();
                memdata[i->x][i->y] = Empty;
                mem->unlock();
                i = bullets.erase(i);
            }
        }
    }
    void handleKey(int key) {
        switch (key) {
        case Qt::Key_Up:
            --position.x;
            mem->lock();
            memdata[position.x + 1][position.y] = Empty;
            memdata[position.x][position.y] = MyPlaneT;
            mem->unlock();
            break;
        case Qt::Key_Down:
            ++position.x;
            mem->lock();
            memdata[position.x - 1][position.y] = Empty;
            memdata[position.x][position.y] = MyPlaneT;
            mem->unlock();
            break;
        case Qt::Key_Left:
            --position.y;
            mem->lock();
            memdata[position.x][position.y + 1] = Empty;
            memdata[position.x][position.y] = MyPlaneT;
            mem->unlock();
            break;
        case Qt::Key_Right:
            ++position.y;
            mem->lock();
            memdata[position.x][position.y - 1] = Empty;
            memdata[position.x][position.y] = MyPlaneT;
            mem->unlock();
            break;
        }
    }
};

class EvilPlane: public Plane {
    using Plane::Plane;
public:
    virtual void run() {
        if ((++ifEmit) % 2) {
            bullets.insert(bullets.end(), {position.x, position.y});
        }
        for (QList<node>::iterator i = bullets.begin(); i != bullets.end();) {
            if (i->x != 24) {
                ++i->x;
                mem->lock();
                if (memdata[i->x - 1][i->y] == EvilBulletT) memdata[i->x - 1][i->y] = Empty;
                if (memdata[i->x][i->y] == EvilPlaneT) setStatus(Victory);
                else if (memdata[i->x][i->y == MyPlaneT]) setStatus(Failure);
                else memdata[i->x][i->y] = EvilBulletT;
                mem->unlock();
                ++i;
            } else {
                mem->lock();
                memdata[i->x][i->y] = Empty;
                mem->unlock();
                i = bullets.erase(i);
            }
        }
    }
};

class Widget : public QWidget {
    Q_OBJECT

    QTimer *m_clock_intr_device;

    QSharedMemory *mem;
    nodeType (*memdata)[ELEMENT_WIDTH_NUM];
    QImage *myPlaneImage, *evilPlaneImage;

    QPainter painter;
    QRandomGenerator random;

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
