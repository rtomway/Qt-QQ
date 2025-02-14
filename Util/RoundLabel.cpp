// RoundedLabel.cpp
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include "RoundLabel.h"


RoundedLabel::RoundedLabel(QWidget* parent)
    :QLabel(parent)
{
    setScaledContents(true);
}

void RoundedLabel::setPixmap(const QPixmap& pixmap) {
    m_pixmap = pixmap;
    update();  // 触发重新绘制
}

void RoundedLabel::paintEvent(QPaintEvent* event) {
    if (m_pixmap.isNull()) {
        QLabel::paintEvent(event);
        return;
    }

    // 创建一个与标签大小相同的临时图像
    QPixmap scaled = m_pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 创建目标图像
    QPixmap dest(this->size());
    dest.fill(Qt::transparent);

    // 使用 QPainter 绘制圆形图片
    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform);  // 启用高质量缩放

    // 创建一个圆形路径
    QPainterPath path;
    path.addEllipse(0, 0, this->width(), this->height());

    // 设置剪裁区域为圆形
    painter.setClipPath(path);

    // 绘制图片
    painter.drawPixmap(0, 0, scaled);

    // 将绘制好的图像设置到 QLabel 中
    QLabel::setPixmap(dest);

    // 调用基类的 paintEvent
    QLabel::paintEvent(event);
}