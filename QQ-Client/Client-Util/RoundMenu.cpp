#include "RoundMenu.h"
#include <QtGui/QPainter>
#include <QtCore/QtMath>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>

namespace
{
    const int SHADOW_WIDTH = 5;
    const int RADIUS = 15;
    const QString QSS = "QMenu{background-color:white;margin:7px;}";
}

RoundMenu::RoundMenu(QWidget* parent) :QMenu(parent)
{
    init();
}

void RoundMenu::init()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setStyleSheet(QSS);
    setAttribute(Qt::WA_TranslucentBackground);
}
