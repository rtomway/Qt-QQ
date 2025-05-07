#ifndef ANGlEROUNDEDWIDGET_H_
#define ANGlEROUNDEDWIDGET_H_

#include <QWidget>

class AngleRoundedWidget :public QWidget
{
public:
	AngleRoundedWidget(QWidget* parent = nullptr);
	void init();
protected:
	void paintEvent(QPaintEvent* ev)override;
private:
	QPoint m_dragPos;
	bool m_dragging = false;
	int m_shadowWidth = 5;
	int m_radius = 10;
};

#endif // !ANGlEROUNDEDWIDGET_H_