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
    //FramelessWindowHint、WA_TranslucentBackground会让addAction(QAction)失效（hover动画失效）
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setStyleSheet(QSS);
    setAttribute(Qt::WA_TranslucentBackground);
   
}

//void RoundMenu::paintEvent(QPaintEvent* event)
//{
//    QMenu::paintEvent(event);
//    //绘制阴影
//    QPainterPath path;
//    path.setFillRule(Qt::WindingFill);
//    path.addRoundedRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - SHADOW_WIDTH * 2, this->height() - SHADOW_WIDTH * 2, RADIUS,RADIUS);
//
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.fillPath(path, QBrush(Qt::white));
//
//    QColor color(150, 150, 150, 55);
//    for (int i = 0; i != SHADOW_WIDTH; ++i)
//    {
//        QPainterPath path;
//        path.setFillRule(Qt::WindingFill);
//        path.addRoundedRect(SHADOW_WIDTH - i, SHADOW_WIDTH - i, this->width() - (SHADOW_WIDTH - i) * 2, this->height() - (SHADOW_WIDTH - i) * 2, RADIUS,RADIUS);
//        color.setAlpha(180 - qSqrt(i) * 80);
//        painter.setPen(color);
//        painter.drawPath(path);
//    }
//}
//void RoundMenu::paintEvent(QPaintEvent* event)
//{
    
        // 调用基类的 paintEvent 以保留默认绘制行为
       // QMenu::paintEvent(event);

        // 绘制圆角矩形背景                                                                                                                                                                                                                 
        //QPainter painter(this);
        ////painter.setRenderHint(QPainter::Antialiasing, true);
        //QPainterPath path;
        //path.setFillRule(Qt::WindingFill);
        //path.addRoundedRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - SHADOW_WIDTH * 2, this->height() - SHADOW_WIDTH * 2, RADIUS, RADIUS);

        //painter.fillPath(path, QBrush(Qt::white));
      
        // //绘制阴影
        //QColor color(150, 150, 150);
        //for (int i = 0; i < SHADOW_WIDTH; ++i)
        //{
        //    QPainterPath shadowPath = path.translated(-i, -i).translated(i, i); // 复制并移动路径
        //    int alpha = qBound(0, 180 - static_cast<int>(qSqrt(static_cast<qreal>(i)) * 80), 255);
        //    color.setAlpha(alpha);
        //    painter.setPen(color);
        //    painter.drawPath(shadowPath);
        //}
 /*       QMenu::paintEvent(event);
}*/