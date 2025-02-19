#ifndef ADDWIDGET_H_
#define ADDWIDGET_H_

#include <QWidget>
#include <QJsonObject>
#include "LineEditwithButton.h"

namespace Ui { class AddWidget; }

class AddWidget :public QWidget
{
	Q_OBJECT
public:
	AddWidget(QWidget* parent = nullptr);
	~AddWidget();
	void init();
	void setUser(const QJsonObject&obj);
private:
	Ui::AddWidget* ui{};
	LineEditwithButton* m_grouping{};
	QString m_userName{};
	QString m_user_id{};
	QPixmap m_userHead{};
	bool m_isSend{};
signals:
	
};


#endif // !ADDWIDGET_H_