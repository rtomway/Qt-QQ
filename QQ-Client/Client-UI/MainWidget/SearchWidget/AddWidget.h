#ifndef ADDWIDGET_H_
#define ADDWIDGET_H_

#include <QWidget>
#include <QJsonObject>
#include <QPixmap>
#include "GlobalTypes.h"

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
	void setGroup(const QJsonObject& obj, const QPixmap& pixmap);
private:
	void onAdd();
private:
	Ui::AddWidget* ui{};
	LineEditwithButton* m_grouping{};
	QString m_addName{};
	QString m_addId{};
	QPixmap m_addHeadPix{};
	bool m_isSend{};
	ChatType m_type{ ChatType::User };
};


#endif // !ADDWIDGET_H_