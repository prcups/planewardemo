#include "widget.h"
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi();
}

void Widget::setupUi() {
    this->setWindowTitle("\u1F680大战");
    this->resize(640, 480);

    myplane = new QLabel;
    myplane->setText("\u1F680");
    myplane->setParent(this);
    myplane->setVisible(false);

    yourplane = new QLabel;
    yourplane->setText("\u1f605");
    yourplane->setParent(this);
    yourplane->setVisible(false);
}

void Widget::initFunc() {

}

Widget::~Widget()
{
    delete myplane;
    delete yourplane;
}

