#ifndef FRIENDSELECTEDWIDGET_H_
#define FRIENDSELECTEDWIDGET_H_

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QJsonObject>

#include "Friend.h"

class FriendSelectedWidget :public QWidget
{
	Q_OBJECT
public:
	FriendSelectedWidget(QWidget* parent = nullptr);
	void setUser(const QString& user_id);
	const QString& getUserId();
	void setChecked(bool isCheck);
	bool isChecked();
private:
	void init();
private:
	QHBoxLayout* m_layout{};
	QCheckBox* m_selected{};
	QLabel* m_headLab{};
	QLabel* m_nameLab{};
	QSharedPointer<Friend>m_oneself{};
	QJsonObject m_json{};
	QString m_userId{};
signals:
	void checkFriend(bool isChecked);
};

#endif // !FRIENDSELECTEDWIDGET_H_
