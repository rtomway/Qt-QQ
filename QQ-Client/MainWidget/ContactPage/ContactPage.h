#ifndef CONTACTPAGE_H_
#define CONTACTPAGE_H_

#include <QWidget>
#include "ContactDetailWidget.h"
#include <QJsonObject>
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
};


#endif // !CONTACTPAGE_H_
