#include "AngleRoundedWidget.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

AngleRoundedWidget::AngleRoundedWidget(QWidget* parent)
{
	init();
}

void AngleRoundedWidget::init()
{
	this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_TranslucentBackground); //子窗口圆角父类背景需透明
}

void AngleRoundedWidget::paintEvent(QPaintEvent* ev)
{
	QStyleOption opt; // 创建QStyleOption对象
	opt.rect=this->rect(); // 初始化QStyleOption对象
	QPainter p(this); // 创建QPainter对象
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // 使用当前窗口的绘图风格绘制小部件
}
