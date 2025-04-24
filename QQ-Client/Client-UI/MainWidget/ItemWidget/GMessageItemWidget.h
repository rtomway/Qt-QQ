#ifndef GMESSAGEITEMWIDGET_H_
#define GMESSAGEITEMWIDGET_H_

#include "ItemWidget.h"
#include "GlobalTypes.h"
#include "Group.h"
#include <QLabel>


class GMessageItemWidget :public ItemWidget
{
	Q_OBJECT
public:
	explicit GMessageItemWidget(QWidget* parent = nullptr);
	// 重写基类的纯虚函数
	void setItemWidget(const QString& id) override;
	void clearUnRead();
	void updateUnReadMessage(const QString& user_id, const QString& message, const QString& time);
private:
	void init();
private:
	QSharedPointer<Group>m_group = nullptr;
	QString m_groupId{};
	QLabel* m_timeLab = nullptr;
	QLabel* m_countLab = nullptr;
	QStringList m_unReadMesssage = {};
	QString m_lastTime{};
	QString m_sender{};
};

#endif // !GMESSAGEITEMWIDGET_H_
