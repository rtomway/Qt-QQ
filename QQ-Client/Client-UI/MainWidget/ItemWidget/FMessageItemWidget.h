#ifndef FMESSAGEITEMWIDGET_H_
#define FMESSAGEITEMWIDGET_H_

#include "ItemWidget.h"
#include "GlobalTypes.h"
#include <QLabel>

class FMessageItemWidget :public ItemWidget
{
	Q_OBJECT
public:
	explicit FMessageItemWidget(QWidget* parent = nullptr);
	// 重写基类的纯虚函数
	void setItemWidget(const QString& id) override;
	void clearUnRead();
private:
	void init();
private:
	QString m_friendId{};
	QSharedPointer<Friend>m_friend = nullptr;
	QJsonObject m_friendJson{};
	QLabel* m_timeLab = nullptr;
	QLabel* m_countLab = nullptr;
};

#endif // !FMESSAGEITEMWIDGET_H_
