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
};

#endif // !ANGlEROUNDEDWIDGET_H_