#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("飞机大战");
    this->setFixedSize(ELEMENT_WIDTH_NUM * ELEMENT_EDGE_LENGTH, ELEMENT_HEIGHT_NUM * ELEMENT_EDGE_LENGTH);
    mem = new QSharedMemory("mem");
    mem->create(ELEMENT_HEIGHT_NUM * ELEMENT_WIDTH_NUM * sizeof(nodeType));
    memdata = static_cast<nodeType (*)[ELEMENT_WIDTH_NUM]>(mem->data());
    mem->lock();
    memset(memdata, 0, sizeof((nodeType **) memdata));
    mem->unlock();

    myPlaneImage = new QImage(":/MyPlane.ico");
    evilPlaneImage = new QImage(":/EvilPlane.ico");

    m_clock_intr_device = new QTimer(this);
    myp = new MyPlane(24, 12, MyPlaneT);
    evilp = new EvilPlane(0, 13, EvilPlaneT);

    connect(m_clock_intr_device, &QTimer::timeout, this, &Widget::ClockIntrExec);
    connect(evilp, &Plane::statusChanged, this, &Widget::handleStatus);
    connect(myp, &Plane::statusChanged, this, &Widget::handleStatus);

    m_status = Running;
    m_clock_intr_device->start(500);
}

Widget::~Widget() {
    delete evilp;
    delete myp;
    delete m_clock_intr_device;
    delete mem;
    delete myPlaneImage;
    delete evilPlaneImage;
}

void Widget::paintEvent(QPaintEvent *e) {
    painter.begin(this);
    for (int i = 0; i < ELEMENT_HEIGHT_NUM; ++i) {
        for (int j = 0; j < ELEMENT_WIDTH_NUM; ++j) {
            switch (memdata[i][j]) {
            case MyPlaneT:
                painter.drawImage(QRect{j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
                                        ELEMENT_EDGE_LENGTH, ELEMENT_EDGE_LENGTH},
                                  *myPlaneImage);
                break;
            case EvilPlaneT:
                painter.drawImage(QRect{j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
                                        ELEMENT_EDGE_LENGTH, ELEMENT_EDGE_LENGTH},
                                  *evilPlaneImage);
                break;
            case MyBulletT:
                painter.setBrush(Qt::blue);
                painter.drawEllipse(j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
                                 ELEMENT_EDGE_LENGTH, ELEMENT_EDGE_LENGTH);
                break;
            case EvilBulletT:
                painter.setBrush(Qt::green);
                painter.drawEllipse(j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
                                 ELEMENT_EDGE_LENGTH, ELEMENT_EDGE_LENGTH);
                break;
            case MultiBulletT:
                painter.setBrush(Qt::black);
                painter.drawEllipse(j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
                                 ELEMENT_EDGE_LENGTH, ELEMENT_EDGE_LENGTH);
                break;
            case Empty:
                break;
            }

        }
    }
    painter.end();
}

void Widget::ClockIntrExec() {
    myp->start();
    evilp->start();
    this->update();
}

void Widget::handleStatus(statusType status) {
    if (status != Running) {
        m_clock_intr_device->stop();
        m_status = status;
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (m_status != Running) return;
    myp->handleKey(e->key());
    this->update();
}

void MyPlane::run() {
    if ((++ifEmit) % 2) {
        bullets.insert(bullets.end(), {position.x, position.y});
    }
    for (QList<node>::iterator i = bullets.begin(); i != bullets.end();) {
        if (i->x) {
            --i->x;
            mem->lock();
            if (memdata[i->x + 1][i->y] != MyPlaneT) memdata[i->x + 1][i->y] = Empty;
            switch (memdata[i->x][i->y]) {
            case EvilPlaneT:
                memdata[i->x][i->y] = MyBulletT;
                setStatus(Victory);
                break;
            case EvilBulletT:
                memdata[i->x][i->y] = MultiBulletT;
                break;
            case MyPlaneT:
                break;
            case MyBulletT:
                i = bullets.erase(i);
                continue;
            default:
                memdata[i->x][i->y] = MyBulletT;
                break;
            }
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

void MyPlane::handleKey(int key) {
    switch (key) {
    case Qt::Key_Up:
        if (position.x == 0) return;
        --position.x;
        mem->lock();
        memdata[position.x + 1][position.y] = Empty;
        mem->unlock();
        break;
    case Qt::Key_Down:
        if (position.x == 24) return;
        ++position.x;
        mem->lock();
        memdata[position.x - 1][position.y] = Empty;
        mem->unlock();
        break;
    case Qt::Key_Left:
        if (position.y == 0) return;
        --position.y;
        mem->lock();
        memdata[position.x][position.y + 1] = Empty;
        mem->unlock();
        break;
    case Qt::Key_Right:
        if (position.y == 24) return;
        ++position.y;
        mem->lock();
        memdata[position.x][position.y - 1] = Empty;
        mem->unlock();
        break;
    }
    mem->lock();
    if (memdata[position.x][position.y] == EvilBulletT || memdata[position.x][position.y] == EvilPlaneT) {
        setStatus(Failure);
    }
    else memdata[position.x][position.y] = MyPlaneT;
    mem->unlock();
}

void EvilPlane::run() {
    position.y = (position.y + 1) % ELEMENT_WIDTH_NUM;
    mem->lock();
    memdata[position.x][(position.y - 1 + ELEMENT_WIDTH_NUM) % ELEMENT_WIDTH_NUM] = Empty;
    if (memdata[position.x][position.y] == MyPlaneT) {
        memdata[position.x][position.y] = EvilPlaneT;
        setStatus(Failure);
    } else if (memdata[position.x][position.y] == MyBulletT) {
        setStatus(Victory);
    }
    else memdata[position.x][position.y] = EvilPlaneT;
    mem->unlock();
    if ((++ifEmit) % 2) {
        bullets.insert(bullets.end(), {position.x, position.y});
    }
    for (QList<node>::iterator i = bullets.begin(); i != bullets.end();) {
        if (i->x != 24) {
            ++i->x;
            mem->lock();
            if (memdata[i->x - 1][i->y] != EvilPlaneT) memdata[i->x - 1][i->y] = Empty;
            switch (memdata[i->x][i->y]) {
            case MyPlaneT:
                memdata[i->x][i->y] = EvilBulletT;
                setStatus(Failure);
                break;
            case MyBulletT:
                memdata[i->x][i->y] = MultiBulletT;
                break;
            case EvilPlaneT:
                break;
            case EvilBulletT:
                i = bullets.erase(i);
                continue;
            default:
                memdata[i->x][i->y] = EvilBulletT;
                break;
            }
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

Plane::Plane(int x, int y, nodeType t): m_status(NotStart), mem(new QSharedMemory("mem")), position({x, y}), myType(t) {
    mem->attach();
    memdata = (nodeType (*)[ELEMENT_WIDTH_NUM]) mem->data();
    mem->lock();
    memdata[position.x][position.y] = myType;
    mem->unlock();
}
