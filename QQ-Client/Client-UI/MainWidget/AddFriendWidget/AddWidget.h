#ifndef ADDWIDGET_H_
#define ADDWIDGET_H_

#include <QWidget>
#include <QJsonObject>
#include <QPixmap>

#include "LineEditwithButton.h"

namespace Ui { class AddWidget; }

class AddWidget :public QWidget
{
	Q_OBJECT
public:
	AddWidget(QWidget* parent = nullptr);
	~AddWidget();
private:
	void init();
public:
	void setUser(const QJsonObject& obj, const QPixmap& pixmap);
private:
	Ui::AddWidget* ui{};
	LineEditwithButton* m_grouping{};
	QString m_friendName{};
	QString m_friendId{};
	QPixmap m_friendHead{};
	bool m_isSend{};
};


#endif // !ADDWIDGET_H_