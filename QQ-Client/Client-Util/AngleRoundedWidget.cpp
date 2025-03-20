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
	//QPainter painter(this); // 创建QPainter对象
	//painter.setRenderHint(QPainter::Antialiasing); // 设置反锯齿
	//painter.setBrush(QBrush(QColor(255, 255, 255))); // 设置画刷颜色为白色
	//painter.setPen(Qt::transparent); // 设置画笔颜色为透明
	//QRect rect = this->rect(); // 获取当前窗口的矩形区域
	//painter.drawRoundedRect(rect, 10, 10); // 绘制带有圆角的矩形，圆角半径为15px
	QStyleOption opt; // 创建QStyleOption对象
	opt.rect=this->rect(); // 初始化QStyleOption对象
	QPainter p(this); // 创建QPainter对象
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // 使用当前窗口的绘图风格绘制小部件
}
