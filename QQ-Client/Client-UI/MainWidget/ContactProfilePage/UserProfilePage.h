#ifndef USERPROFILEPAGE_H_
#define USERPROFILEPAGE_H_

#include <QWidget>
#include <QJsonObject>
#include "AngleRoundedWidget.h"

namespace Ui { class UserProfilePage; }

class UserProfilePage :public AngleRoundedWidget
{
	Q_OBJECT
public:
	UserProfilePage(QWidget* parent = nullptr);
	~UserProfilePage();
	void init();
	void setUserProfilePage(const QJsonObject& obj, bool isFriend);
private:
	Ui::UserProfilePage* ui{};
	bool m_isFriend{ false };
	QJsonObject m_json{};
	QString m_user_id{};
};

#endif // !USERPROFILEPAGE_H_
