#ifndef TIPMESSAGEITEMWIDGET_H_
#define TIPMESSAGEITEMWIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include "ChatPage.h"

class TipMessageItemWidget :public QWidget
{
	Q_OBJECT
public:
	TipMessageItemWidget(const QString& text, QWidget* parent = nullptr);
	void init();
private:
	QLabel* m_label{};
	QHBoxLayout* m_layout{};
};

#endif // !TIPMESSAGEITEMWIDGET_H_
