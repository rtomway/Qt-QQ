#ifndef CONTACTDETAILWIDGET_H_
#define CONTACTDETAILWIDGET_H_

#include <QWidget>
#include "LineEditwithButton.h"
#include "AngleRoundedWidget.h"
#include <QJsonObject>
#include <memory>
#include<QCalendarWidget>
#include<QWidgetAction>

namespace Ui { class ContactDetailWidget; }

class ContactDetailWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	ContactDetailWidget(QWidget* parent = nullptr);
	void init();
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
private:
	QPixmap m_headPix;
	QString m_avatarOldPath{};
	QString m_avatarNewPath{};
	bool m_avatarIsChange{false};
	QJsonObject m_json{};
	std::unique_ptr<QWidgetAction>m_calendarAction;
	std::unique_ptr<QCalendarWidget>m_calendarWidget;
public:
	void setUser(const QJsonObject& obj);
	const QJsonObject& getUser()const;
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
};



#endif // !CONTACTDETAILWIDGET_H_