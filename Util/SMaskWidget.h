#ifndef SMASKWIDGET_H_
#define  SMASKWIDGET_H_

#include <QWidget>

class SMaskWidget : public QWidget
{
	Q_OBJECT
public:
	enum PopPosition {
		LeftWidget,
		MiddleWidget,
		RightWidget,
	};
public:
	SMaskWidget(QWidget* parent = nullptr);
	static SMaskWidget* instance();
	void setMainWidget(QWidget* widget);
	void addDialog(QWidget* dialog);
	void popUp(QWidget* dialog);
	void setPopPostion(PopPosition position);
	void setPopGeometry(const QRect&rect);
	QSize getMainWidgetSize();
protected:
	bool eventFilter(QObject* object, QEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
	void onResize();
private:
	QWidget* m_MainWidget{};
	QWidgetSet m_dialogs;
	QWidget* m_currentPopUp{};
	PopPosition m_pos;
};

#endif // !SMASKWIDGET_H_
