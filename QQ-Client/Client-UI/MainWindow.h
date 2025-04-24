#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QtWidgets>
#include <QPropertyAnimation>

#include "MainWidget.h"
#include "LoginWidget.h"

class MainWindow :public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	void init();
private:
	QStackedWidget* m_stackedWidget{};
	LoginWidget* m_loginWidget{};
	MainWidget* m_mainWidget{};
	QPropertyAnimation* m_expandAnimation{};
public slots:
	void showLoginWidget();
	void showMainWidget();
protected:
	void resizeEvent(QResizeEvent* ev)override;
	void mousePressEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* event)override;
	void mouseMoveEvent(QMouseEvent* event)override;
	void showEvent(QShowEvent* event)override;
	void enableMouseTracking(QWidget* w);
private:
	QPoint m_pressPos{};
private:
	int countFlag(QPoint p, int row);
	void setCursorType(int flag);
	int countRow(QPoint p);

private:
	int m_curPos;
	QPoint pLast;

	bool m_bPressed;
};


#endif // !,MAINWINDOW_H_
