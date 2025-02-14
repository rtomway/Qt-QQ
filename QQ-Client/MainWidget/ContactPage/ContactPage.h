#ifndef CONTACTPAGE_H_
#define CONTACTPAGE_H_

#include <QWidget>
#include "ContactDetailWidget.h"


namespace Ui { class ContactPage; }

class ContactPage :public QWidget
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
};


#endif // !CONTACTPAGE_H_
