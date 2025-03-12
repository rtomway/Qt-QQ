#ifndef CONTACTPAGE_H_
#define CONTACTPAGE_H_

#include <QWidget>
#include <QJsonObject>

#include "ContactDetailWidget.h"
#include "AngleRoundedWidget.h"
#include "Friend.h"


namespace Ui { class ContactPage; }

class ContactPage :public AngleRoundedWidget
{
	Q_OBJECT
public:
	ContactPage(QWidget* parent = nullptr);
	~ContactPage();
	void init();
private:
	Ui::ContactPage* ui{};
private:
	int m_likeCount{ 0 };
	QString m_userId;
	QString m_name;
	ContactDetailWidget* m_detailEditWidget{};
	QJsonObject m_json{};
	QSharedPointer<Friend>m_oneself{};
public:
	void setUser(const QJsonObject& obj);
	void clearWidget();
signals:
	void sendMessage(const QString& user_id);
};


#endif // !CONTACTPAGE_H_
