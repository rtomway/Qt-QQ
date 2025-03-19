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
	QString m_friendId;
	QSharedPointer<Friend>m_oneself{};
	QJsonObject m_json{};
	ContactDetailWidget* m_detailEditWidget{};
private:
	bool m_isBlockedComboBox{true};
public:
	void setUser(const QString& user_id);
	void clearWidget();
};


#endif // !CONTACTPAGE_H_
