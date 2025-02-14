#include "RotationLabel.h"


RotationLabel::RotationLabel(QWidget* parent)
	:QLabel(parent)
    ,m_animation(new QPropertyAnimation(this,"rotation"))
{
    m_rotation = { 0.0 };
    m_agale = true;
    this->setProperty("rotation", 0.0); // 确保属性 "rotation" 被安装到这个对象上
    this->setScaledContents(true);
    this->setFixedSize(15, 15);
   
    qreal ratio = this->devicePixelRatioF();
    QPixmap pixmap(":/icon/Resource/Icon/rightarrow.png");
    pixmap.setDevicePixelRatio(ratio); 
    setPixmap(pixmap.scaled(size() * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   
}

qreal RotationLabel::rotation() const
{
    return m_rotation;
}

void RotationLabel::setRotation(qreal rotation)
{
    if (m_rotation != rotation) {
        m_rotation = rotation;
        // 触发重绘
        update();
    }
}

void RotationLabel::setAgale()
{
    m_agale=!m_agale;
}

void RotationLabel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 保存原始变换矩阵
    painter.save();

    // 计算旋转中心点
    int w = this->width();
    int h = this->height();
    painter.translate(w / 2, h / 2);
    painter.rotate(m_rotation);
    painter.translate(-w / 2, -h / 2);

    // 绘制文本或图片
    if (!pixmap().isNull()) {
        painter.drawPixmap(0, 0, pixmap());
    }
    else {
        painter.drawText(rect(), alignment(), text());
    }

    // 恢复原始变换矩阵
    painter.restore();
}

void RotationLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_animation->setDuration(500); // 动画持续1秒
        if (!m_agale)
        {
            m_animation->setStartValue(0); 
            m_animation->setEndValue(90); 
        }
        else {
            m_animation->setStartValue(90); 
            m_animation->setEndValue(0); 
        }
        m_animation->setEasingCurve(QEasingCurve::Linear); // 线性变化
        m_animation->start(); // 启动动画
    }
    QLabel::mousePressEvent(event); // 调用基类的实现
}
