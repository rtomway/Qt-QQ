#ifndef CONTACTPAGE_H_
#define CONTACTPAGE_H_

#include <QWidget>
#include <QJsonObject>

#include "ContactDetailWidget.h"
#include "AngleRoundedWidget.h"


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
	int m_likeCount{0};
	QString m_name;
	ContactDetailWidget* m_detailEditWidget{};
	QJsonObject m_json{};
public:
	void setUser(const QJsonObject&obj);
signals:
	void sendMessage(const QString& user_id);
};


#endif // !CONTACTPAGE_H_
