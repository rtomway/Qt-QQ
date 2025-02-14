#ifndef CONTACTDETAILWIDGET_H_
#define CONTACTDETAILWIDGET_H_

#include <QWidget>
#include "LineEditwithButton.h"
#include "AngleRoundedWidget.h"

namespace Ui { class ContactDetailWidget; }

class ContactDetailWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	ContactDetailWidget(QWidget* parent = nullptr);
	~ContactDetailWidget();
	void init();
private:
	Ui::ContactDetailWidget* ui{};
private:
	QLabel* m_editDetail{};
	QPushButton* m_exitBtn{};
	QLabel* m_headLab{};
	LineEditwithButton* m_nickNameEdit{};
	LineEditwithButton* m_signaltureEdit{};
	LineEditwithButton* m_genderEdit{};
	LineEditwithButton* m_birthdayEdit{};
	LineEditwithButton* m_countryEdit{};
	LineEditwithButton* m_provinceEdit{};
	LineEditwithButton* m_areaEdit{};
};



#endif // !CONTACTDETAILWIDGET_H_