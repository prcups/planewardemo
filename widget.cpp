#include "widget.h"

//typedef (*nodeType)[ELEMENT_WIDTH_NUM] nodeMatrix;

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
    painter.setBrush(Qt::SolidPattern);
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
            case EvilBulletT:
                painter.drawRect(j * ELEMENT_EDGE_LENGTH, i * ELEMENT_EDGE_LENGTH,
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
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    myp->handleKey(e->key());
}
