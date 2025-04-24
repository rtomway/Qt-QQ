#ifndef ROTATIONLABEL
#define ROTATIONLABEL

#include <QLabel>
#include <QPainter>
#include <QPropertyAnimation>
#include <QMouseEvent>

class RotationLabel :public QLabel
{
	Q_OBJECT
		Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
public:
	RotationLabel(QWidget* parent = nullptr);
	qreal rotation()const;
	void setRotation(qreal rotation);
	void setAgale();
private:
	qreal m_rotation{};
	QPropertyAnimation* m_animation{};
	bool m_agale{};
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
};

#endif // !ROTATIONLABEL
